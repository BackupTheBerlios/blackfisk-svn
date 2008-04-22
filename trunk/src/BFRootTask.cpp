/**
 * Name:        BFRootTask.cpp
 * Purpose:     BFRootTask class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFRootTask.h"

#include <wx/tokenzr.h>

#include "blackfisk.h"

#define BFROOTTASK_DEFAULT_NAME _("unnamed")

BFRootTask BFRootTask::sRootTask_;

BFRootTask& BFRootTask::Instance ()
{
    return sRootTask_;
}

BFRootTask::BFRootTask ()
              : strName_(BFROOTTASK_DEFAULT_NAME),
                bModified_(false),
                oidLast_(BFInvalidOID)
{
}


/*virtual*/BFRootTask::~BFRootTask ()
{
    ClearTaskVector();
}


BFTaskVector& BFRootTask::GetAllTasks (BFTaskVector& rVec)
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        rVec.push_back(*itVec);

    return rVec;
}

void BFRootTask::ClearTaskVector ()
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        delete (*itVec);

    vecTasks_.clear();
}

bool BFRootTask::Has (BFProjectSettings* pPrjSet)
{
    if (pPrjSet == &projectSettings_)
        return true;

    return false;
}

long BFRootTask::FindTask (BFTask* pTask)
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

BFTask* BFRootTask::GetTask(BFoid oid)
{
    BFTaskVectorIt itVec;

	for (itVec = vecTasks_.begin();
		 itVec != vecTasks_.end();
		 ++itVec)
        if ((*itVec)->GetOID() == oid)
            return (*itVec);

    return NULL;
}

BFTask* BFRootTask::GetNextTask (BFTask* pTask)
{
    if (vecTasks_.empty())
        return NULL;

    // current task position
    long lPos = FindTask(pTask);

    if (lPos == -1 || (lPos+1) == vecTasks_.size())
        return vecTasks_[0];

    return vecTasks_[lPos+1];
}

BFTask* BFRootTask::GetLastTask ()
{
    if (vecTasks_.empty())
        return NULL;

    return vecTasks_[vecTasks_.size()-1];
}

bool BFRootTask::DeleteTask (BFoid oid, bool bBroadcast /*= true*/)
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

bool BFRootTask::DeleteTasks (BFTaskVector& vecTasks)
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

bool BFRootTask::HasTask(BFoid oid)
{
    return (GetTask(oid) != NULL);
}

bool BFRootTask::IsModified ()
{
    return bModified_;
}

void BFRootTask::SetModified (bool bModified /*= true*/)
{
    bModified_ = bModified;
}

bool BFRootTask::Serialize (jbSerialize& rA)
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
        projectSettings_.Serialize(rA);
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
        projectSettings_.Serialize(rA);
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


long BFRootTask::GetTaskCount ()
{
    return vecTasks_.size();
}

void BFRootTask::SetName (const wxChar* strName)
{
    if (strName == NULL)
        return;

    strName_ = strName;

    SetModified();
}


const wxChar* BFRootTask::GetName ()
{
    return strName_;
}


BFProjectSettings& BFRootTask::GetSettings ()
{
    return projectSettings_;
}

long BFRootTask::FindLastTaskWithDestination(const wxChar* destination)
{
    if (destination == NULL)
        return -1;

    long rc = -1;
    wxString strDest(destination);

	for (BFTaskVector::size_type i = 0;
		 i != vecTasks_.size();
		 ++i)
    {
        if (strDest == vecTasks_[i]->GetDestination())
            rc = i;
    }

    return rc;
}

BFTaskVector& BFRootTask::FindAllTasksWithPlaceholders (BFTaskVector& rVec)
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

BFoid BFRootTask::AppendTask (BFTask& rTask)
{
    if ( !(rTask.IsValid()) )
        return BFInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return BFInvalidOID;

    // backup a backup?
    long idx = FindLastTaskWithDestination(rTask.GetSource());

    /* if the source-dir of rTask created by any other
       task (source == destination_of_other_task), rTask
       should be executed after the source is created by
       the other task(s) */
    if (idx == -1 || idx == (vecTasks_.size()-1))
    {
        vecTasks_.push_back(&rTask);
    }
    else
    {
        idx++;
        vecTasks_.insert(vecTasks_.begin()+idx, &rTask);
    }

    SetModified();

    return rTask.GetOID();
}

BFoid BFRootTask::AppendTask (BFTaskType type,
                             const wxChar* strSource,
                             const wxChar* strDestination,
                             const wxChar* strName,
                             bool bVerify,
                             bool bVerifyContent,
                             wxArrayString* pArrExclude /*= NULL*/,
                             BFArchiveFormat archive /*= CompressNOTUSED*/)
{
    // check parameters
    if ( strSource == NULL
      || strDestination == NULL
      || strName == NULL
      || (type == TaskARCHIVE && archive == CompressNOTUSED))
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
                        arrExclude
                    );

    BFoid rc = AppendTask(*pTask);

    if (rc == BFInvalidOID)
        delete pTask;

    return rc;
}


BFoid BFRootTask::CreateOID ()
{
    ++oidLast_;

    if (HasTask(oidLast_) || oidLast_ == BFInvalidOID)
        return CreateOID();

    return oidLast_;
}


void BFRootTask::Close ()
{
    ClearTaskVector();
    oidLast_            = BFInvalidOID;
    SetName             (BFROOTTASK_DEFAULT_NAME);
    GetSettings()       .SetDefault();
    SetModified         (false);
    broadcastObservers();
}


wxArrayString BFRootTask::GetDestinations ()
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
