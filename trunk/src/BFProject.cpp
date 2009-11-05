/**
 * Name:        BFProject.cpp
 * Purpose:     BFProject class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-22
 *              (based on BFRootTask)
 * Copyright:   (c) 2009 Christian Buhtz <blackfisk@web.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "BFProject.h"

#include <wx/tokenzr.h>

#include "BFApp.h"
#include "blackfisk.h"

#define BFPROJECT_DEFAULT_NAME _("unnamed backup")

BFProject BFProject::sProject_;

BFProject& BFProject::Instance ()
{
    return sProject_;
}

BFProject::BFProject ()
         : strName_(BFPROJECT_DEFAULT_NAME),
           bModified_(false),
           oidLast_(BFInvalidOID)
{
}


/*virtual*/BFProject::~BFProject ()
{
    ClearTaskVector();
}


BFTaskVector& BFProject::GetAllTasks (BFTaskVector& rVec)
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        rVec.push_back(*itVec);

    return rVec;
}

void BFProject::ClearTaskVector ()
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        delete (*itVec);

    vecTasks_.clear();
}

long BFProject::FindTask (BFTask* pTask)
{
    BFTaskVectorIt  itVec;
    long            lPos;

	for (lPos = 0, itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec, ++lPos)
        if ((*itVec) == pTask)
            return lPos;

    return -1;
}

BFTask* BFProject::GetTask(BFoid oid)
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        if ((*itVec)->GetOID() == oid)
            return (*itVec);

    return NULL;
}

BFTask* BFProject::GetNextTask (BFTask* pTask)
{
    if (vecTasks_.empty())
        return NULL;

    // current task position
    long lPos = FindTask(pTask);

    if (lPos == -1 || (lPos+1) == (long)vecTasks_.size())
        return vecTasks_[0];

    return vecTasks_[lPos+1];
}

BFTask* BFProject::GetLastTask ()
{
    if (vecTasks_.empty())
        return NULL;

    return vecTasks_[vecTasks_.size()-1];
}

bool BFProject::DeleteTask (BFoid oid, bool bBroadcast /*= true*/)
{
    BFTaskVectorIt itVec;

    //
	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
    {
        // does the OID match?
        if ((*itVec)->GetOID() == oid)
        {
            //
            BFTask* pTask = (BFTask*)(*itVec);

            // erase the task from vector
            vecTasks_.erase(itVec);

            // kill the task itself
            delete pTask;

            // the the project modified
            SetModified(true);

            // broadcast the observers
            if (bBroadcast)
                broadcastObservers();

            //
            return true;
        }
    }

    //
    return false;
}

void BFProject::DeleteTasks (BFTaskVector& vecTasks)
{
    BFTaskVectorIt it;

    for (it = vecTasks.begin();
         it != vecTasks.end();
         ++it)
    {
        // delete the task but don't broadcast the observers
        DeleteTask((*it)->GetOID(), false);
    }

    broadcastObservers();
}

bool BFProject::HasTask(BFoid oid)
{
    return (GetTask(oid) != NULL);
}

bool BFProject::IsModified ()
{
    return bModified_;
}

void BFProject::SetModified (bool bModified /*= true*/)
{
    bModified_ = bModified;
}

bool BFProject::Serialize (jbSerialize& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    // init
    bool            rc = true;
    BFTaskVectorIt  itVec;

    rA.EnterObject();
    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        rA << strName_;
        settings_.Serialize(rA);
        rA << (int)GetTaskCount();

        for (itVec = vecTasks_.begin();
             itVec != vecTasks_.end();
             ++itVec)
        {
            rc = (*itVec)->Serialize(rA);

            if (!rc)
                break;
        }

        SetModified(false);
    }
    else
    // ** serialize FROM file **
    {
        SetModified(false);

        int iObjCount;

        rA >> strName_;
        settings_.Serialize(rA);
        rA >> iObjCount;

        for (int i = 0; i < iObjCount; ++i)
        {
            // create a task with an invalid oid
            BFTask* pTask = new BFTask();

            rc = pTask->Serialize(rA);

            if (rc && pTask != NULL)
                vecTasks_.push_back(pTask);
            else
                break;
        }

        broadcastObservers();
    }
    rA.LeaveObject();

    return rc;
}


long BFProject::GetTaskCount ()
{
    return vecTasks_.size();
}

void BFProject::SetName (const wxString& strName)
{
    strName_ = strName;

    SetModified();
}


const wxString& BFProject::GetName ()
{
    return strName_;
}


BFProjectSettings& BFProject::GetSettings ()
{
    return settings_;
}

long BFProject::FindLastTaskWithDestination(const wxString& strDestination)
{
    long rc = -1;

	for (BFTaskVector::size_type i = 0;
		 i != vecTasks_.size();
		 ++i)
    {
		wxString strD = vecTasks_[i]->GetDestination();

		if ( !(strD.EndsWith(wxFILE_SEP_PATH)) )
			strD << wxFILE_SEP_PATH;

		if (strDestination == vecTasks_[i]->GetDestination()
			|| strDestination.StartsWith(strD))
            rc = i;
    }

    return rc;
}


long BFProject::FindLastTaskWithSource(const wxString& strSource)
{
    long rc = -1;
	wxString strS;

	for (BFTaskVector::size_type i = 0;
		 i != vecTasks_.size();
		 ++i)
    {
		strS = vecTasks_[i]->GetSource();

		if ( !(strS.EndsWith(wxFILE_SEP_PATH)) )
			strS << wxFILE_SEP_PATH;

        if (strSource == vecTasks_[i]->GetSource()
			|| strSource.StartsWith(strS))
            rc = i;
    }

    return rc;
}

BFTaskVector& BFProject::FindAllTasksWithPlaceholders (BFTaskVector& rVec)
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
    {
        if ( (*itVec)->GetName().Matches("*<*>*")
          || (*itVec)->GetSource().Matches("*<*>*")
          || (*itVec)->GetDestination().Matches("*<*>*") )
        {
            rVec.push_back(*itVec);
        }
    }

    return rVec;
}

BFoid BFProject::AppendTask (BFTask& rTask)
{
    if ( !(rTask.IsValid()) )
        return BFInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return BFInvalidOID;

    // ## backup a backup ##

    /* if the source-dir of rTask created by any other
       task (source == destination_of_other_task), rTask
       should be executed after the source is created by
       the other task(s) */
    long idxAfter = FindLastTaskWithDestination(rTask.GetSource());

	/* If the destination-dir of rTask is backuped up by
	   another task (destination == source of_other_task),
	   rTask should be executed before its destination is
	   backedup by the other task(s) */
	long idxBefore = FindLastTaskWithSource(rTask.GetDestination());

	// check
	if (idxAfter != -1 && idxBefore != -1 && idxAfter >= idxBefore)
	{
		BFSystem::Warning(_("This constellation of nested backup tasks is not possible!\nThe task won't be created."));
		return BFInvalidOID;
	}

	// calculating of position needed?
	if (idxAfter == -1 && idxBefore == -1)
	{
		// NO put it at the end
		vecTasks_.push_back(&rTask);
	}
	else
	{	// YES
		long idx = idxAfter;

		if ( (idxAfter + 1) == idxBefore )
		{
			// between after and before
			idx = idxBefore;
		}
		else
		{
			// no after specified
			if (idx == -1)
				// before
				idx = idxBefore;			
			else
				// after
				idx++;
		}

		/* insert 'rTask' before the element
		   specified with 'idx' */
		vecTasks_.insert(vecTasks_.begin()+idx, &rTask);
	}

    SetModified();

    return rTask.GetOID();
}

BFoid BFProject::AppendTask (BFTaskType type,
                             const wxString& strSource,
                             const wxString& strDestination,
                             const wxString& strName,
                             bool bVerify,
                             bool bVerifyContent,
                             wxArrayString* pArrExclude /*= NULL*/,
                             BFArchiveFormat archive /*= CompressNOTUSED*/)
{
    // check parameters
    if ( (type == TaskARCHIVE && archive == CompressNOTUSED))
        return BFInvalidOID;

    // check exclude vector
    wxArrayString arrExclude;
    if (pArrExclude != NULL)
        arrExclude = *pArrExclude;

    // create new task
    BFTask* pTask = new BFTask
                    (
                        type,
                        strSource,
                        strDestination,
                        strName,
                        bVerify,
                        bVerifyContent,
                        archive,
                        arrExclude,
                        true
                    );

    BFoid rc = AppendTask(*pTask);

    if (rc == BFInvalidOID)
        delete pTask;

    return rc;
}


BFoid BFProject::CreateOID ()
{
    ++oidLast_;

    if (HasTask(oidLast_) || oidLast_ == BFInvalidOID)
        return CreateOID();

    return oidLast_;
}


void BFProject::Reset ()
{
    ClearTaskVector();
    oidLast_            = BFInvalidOID;
    SetName             (BFPROJECT_DEFAULT_NAME);
    GetSettings()       .SetDefault();
    SetModified         (false);
    broadcastObservers();
}


wxArrayString BFProject::GetAllDestinations ()
{
    wxArrayString arr;

    // iterate throug the tasks
    BFTaskVectorIt itVec;

    for (itVec = vecTasks_.begin();
         itVec != vecTasks_.end();
         ++itVec)
        arr.Add((*itVec)->GetDestination());

    return arr;
}

wxArrayString BFProject::GetAllSources ()
{
    wxArrayString arr;

    // iterate throug the tasks
    BFTaskVectorIt itVec;

    for (itVec = vecTasks_.begin();
         itVec != vecTasks_.end();
         ++itVec)
        arr.Add((*itVec)->GetSource());

    return arr;
}

/*static*/ wxString BFProject::GetTypeDescription (BFTaskType type, BFArchiveFormat format /* = CompressNOTUSED*/)
{
    wxString strDesc;

    switch (type)
    {
        case TaskARCHIVE:
            strDesc << _("compress to a ") << GetArchiveExtension(format) << _(" archive");
            break;

        case TaskDIRCOPY:
            strDesc = _("copy the directory");
            break;

        case TaskFILECOPY:
            strDesc = _("copy the file");
            break;

        case TaskSYNC:
            strDesc = _("synchronise directories");
            break;

        default:
            strDesc = "unknown task type";
            break;
    };

    return strDesc;
}

/*static*/ wxString BFProject::GetArchiveExtension(BFArchiveFormat format)
{
    switch (format)
    {
        case CompressZIP:
            return "zip";
            break;
    }

    return "unknown";
}

wxString BFProject::GetDefaultProjectLocation ()
{
	return BFApp::ExtractCommunity(GetAllSources());
}
