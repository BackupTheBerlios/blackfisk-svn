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
           oidLast_(BFInvalidOID),
           lRetryHours_(0),
           lRetryMinutes_(5)
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
        rA << strOrgCrontabline_;
        rA << bInRetryMode_;
        rA << strRetryCrontabline_;
        rA << lRetryHours_;
        rA << lRetryMinutes_;

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

        // implemented at version 1030
        if (rA.GetVersion() < 1030)
        {
            /* strOrgCrontabline_
               is set in BFBackup::StoreToFile() */
            bInRetryMode_ = false;
        }
        else
        {
            rA >> strOrgCrontabline_;
            rA >> bInRetryMode_;
        }

        /// implemented at version 1040
        if (rA.GetVersion() < 1040)
        {
            strRetryCrontabline_ = wxEmptyString;
            lRetryHours_ = 0;
            lRetryMinutes_ = 5;
        }
        else
        {
            rA >> strRetryCrontabline_;
            int i = 0;
            rA >> i;
            lRetryHours_ = i;
            rA >> i;
            lRetryMinutes_ = i;
        }

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


void BFProject::SetOriginalCrontabLine (const wxString& strOrgLine)
{
    strOrgCrontabline_ = strOrgLine;
}

const wxString& BFProject::GetOriginalCrontabLine ()
{
    return strOrgCrontabline_;
}


void BFProject::SetInRetryMode (bool bInRetry)
{
    bInRetryMode_ = bInRetry;
}

bool BFProject::IsInRetryMode ()
{
    return bInRetryMode_;
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


bool BFProject::HasNoDependencyParadoxons (const wxString& strSrc,
										   const wxString& strDest,
										   long& rIdxAfter,
										   long& rIdxBefore)
{
    /* If the specified source-dir 'strSrc' is created by any other
       task (strSrc == destination_of_other_task), a task with a
	   source like 'strSrc' should be executed after the source is
	   created by the other task(s) */
    rIdxAfter = FindLastTaskWithDestination(strSrc);

	/* If the specified destination-dir 'strDest' is backuped up by
	   another task (destination == source of_other_task), a task
	   with a destination like 'strDest' should be executed before
	   its destination is backedup by the other task(s) */
	rIdxBefore = FindLastTaskWithSource(strDest);

	// check
	if ( rIdxAfter != -1
	  && rIdxBefore != -1
	  && rIdxAfter >= rIdxBefore)
		return false;

	return true;
}

BFoid BFProject::AppendTask (BFTask& rTask)
{
    if ( !(rTask.IsValid()) )
        return BFInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return BFInvalidOID;

    // ## backup a backup ##

    long idxAfter;
	long idxBefore;

	// check
	if ( false == HasNoDependencyParadoxons (rTask.GetSource(), rTask.GetDestination(), idxAfter, idxBefore) )
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

bool BFProject::ModifyDestination (const wxString& strOldDestination,
                                   const wxString& strNewDestination)
{
    wxString strCurrDest;
	wxString strOldDest = strOldDestination;
	wxString strNewDest = strNewDestination;
	BFTaskVector vecModTasks;

	if ( strOldDest.Last() == wxFILE_SEP_PATH )
		strOldDest.RemoveLast();

	if ( strNewDest.Last() == wxFILE_SEP_PATH )
		strNewDest.RemoveLast();


	// check for tasks to modify
    for (BFTaskVectorIt it = vecTasks_.begin();
         it != vecTasks_.end();
         ++it)
    {
        // the destination of the current task
        strCurrDest = (*it)->GetDestination();

        // has the destiantion to modify?
        if (strCurrDest.StartsWith(strOldDest))
			vecModTasks.push_back((*it));
    }

	// tasks to modify?
	if ( !(vecModTasks.empty()) )
	{
		long idxAfter, idxBefore;

		// iterate over all tasks to modify
		for (BFTaskVectorIt it = vecModTasks.begin();
			 it != vecModTasks.end();
			 ++it)
		{
			// the destination of the current task
			strCurrDest = (*it)->GetDestination();

			// replace old with new destination
			strCurrDest.Replace(strOldDest, strNewDest);

			while ( strCurrDest.EndsWith(wxFILE_SEP_PATH) )
				strCurrDest.RemoveLast(1);

			strCurrDest << wxFILE_SEP_PATH;

			// check for dependency paradoxons
			if ( HasNoDependencyParadoxons ((*it)->GetSource(), strCurrDest, idxAfter, idxBefore) )
			{
				// set destination to the task
				(*it)->SetDestination(strCurrDest);

				// modify position in global vector
				MoveTaskInVector ((*it), idxAfter, idxBefore);
			}
		}

		// mark the project modified if needed
		SetModified();
	}

	return true;
}


bool BFProject::HandleNewDestination ( BFTask* pTask, wxString& strNewDestination )
{
	if ( !pTask )
		return false;

	// check if the methode is allowed to be called and run
	if ( false == pTask->IsWhileSetDestinationCall() )
	{
		BFSystem::Fatal
		(
			_("This methode is not called by BFTask::SetDestination() !" \
			  "\nPlease contact the developer!"),
			"BFProject::HandleNewDestination()"
		);
		return false;
	}

	long idxAfter, idxBefore;

	// check for dependency paradoxons
	if ( HasNoDependencyParadoxons (pTask->GetSource(), strNewDestination, idxAfter, idxBefore) )
	{
		// modify position in global vector
		MoveTaskInVector (pTask, idxAfter, idxBefore);

		return true;
	}

	return false;
}


long BFProject::GetVectorPosition (const BFTask* pTask)
{
	if (pTask == NULL)
		return -1;

	long idx = -1;

	for ( BFTaskVectorIt it = vecTasks_.begin();
		  it != vecTasks_.end();
		  ++it )
	{
		++idx;

		if ( (*it)->GetOID() == pTask->GetOID() )
			return idx;
	}

	return -1;
}

void BFProject::MoveTaskInVector (BFTask* pTask, long idxFrom, long idxTo)
{
	// parameter check
	if ( pTask == NULL
		|| (idxFrom != -1 && idxTo != -1 && idxFrom >= idxTo) )
	{
		BFSystem::Fatal
		(
			wxString::Format
			(
				_("Not able to move the a task in the vector between 'idxFrom'(%d) and 'idxTo'(%d)!" \
			      "\nPlease contact the developer."),
				  idxFrom,
				  idxTo
			),
			"BFProject::MoveTaskInVector()"
		);
		return;
	}

	// realy move?
	if ( idxFrom == -1 && idxTo == -1 )
		return;

	// find current position in global task vector
	long idx = GetVectorPosition (pTask);

	if ( idx == -1 )
	// the task is new -> don't move just insert
	{
		if ( idxFrom != -1 )
		{
			vecTasks_.insert(vecTasks_.begin()+idxFrom+1, pTask);
		}

		if ( idxTo != -1 )
		{
			vecTasks_.insert(vecTasks_.begin()+idxTo, pTask);
		}
	}
	// the task is still there -> move
	else
	{
		// find the task in the global vector...
		for (BFTaskVectorIt it = vecTasks_.begin();
			 it != vecTasks_.end();
			 ++it)
		{
			if ( pTask->GetOID() == (*it)->GetOID() )
			{
				// ...and erase it
				vecTasks_.erase(it);
				break;
			}
		}

		// move forward
		if ( idx <= idxFrom )
		{
			// recalculate idx because of earsing the element
			--idxFrom;

			// reinsert it
			vecTasks_.insert(vecTasks_.begin()+idxFrom+1, pTask);
		}

		// move backward
		if ( idx >= idxTo )
		{
			// reinsert it
			vecTasks_.insert(vecTasks_.begin()+idxTo, pTask);
		}
	}
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
