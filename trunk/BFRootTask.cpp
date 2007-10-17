/**
 * Name:        BFRootTask.cpp
 * Purpose:     BFRootTask and BFRootTaskData class implementation
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

#include <wx/wfstream.h>
#include <wx/tokenzr.h>

#include "BFBackupProgressDlg.h"
#include "BFBackupTree.h"
#include "Progress.h"
#include "BFIconTable.h"
#include "BFBackupLog.h"
#include "blackfisk.h"
#include "BFwxLog.h"
#include "BFThread_ProjectRunner.h"

#define BFROOTTASK_DEFAULT_NAME _("unnamed")

BFRootTaskData::BFRootTaskData ()
              : strName_(BFROOTTASK_DEFAULT_NAME),
                bModified_(false)
{
}


/*virtual*/BFRootTaskData::~BFRootTaskData ()
{
    ClearTaskVector();
}

void BFRootTaskData::ClearTaskVector ()
{
    BFTaskVectorIt itVec;

	for (itVec = TaskVector().begin();
		 itVec != TaskVector().end();
		 itVec++)
        delete (*itVec);

    vecTasks_.clear();
}

bool BFRootTaskData::Has (BFProjectSettings* pPrjSet)
{
    if (pPrjSet == &projectSettings_)
        return true;

    return false;
}

long BFRootTaskData::FindTask (BFTask* pTask)
{
    BFTaskVectorIt  itVec;
    long            lPos;

	for (lPos = 0, itVec = TaskVector().begin();
		 itVec != TaskVector().end();
		 itVec++, lPos++)
        if ((*itVec) == pTask)
            return lPos;

    return -1;
}

BFTask* BFRootTaskData::GetTask(BFoid oid)
{
    BFTaskVectorIt itVec;

	for (itVec = TaskVector().begin();
		 itVec != TaskVector().end();
		 itVec++)
        if ((*itVec)->GetOID() == oid)
            return (*itVec);

    return NULL;
}

BFTask* BFRootTaskData::GetNextTask (BFTask* pTask)
{
    if (TaskVector().empty())
        return NULL;

    // current task position
    long lPos = FindTask(pTask);

    if (lPos == -1 || (lPos+1) == TaskVector().size())
        return TaskVector()[0];

    return TaskVector()[lPos+1];
}

BFTask* BFRootTaskData::GetLastTask ()
{
    if (TaskVector().empty())
        return NULL;

    return TaskVector()[TaskVector().size()-1];
}

bool BFRootTaskData::DeleteTask (BFoid oid)
{
    BFTaskVectorIt itVec;

	for (itVec = TaskVector().begin();
		 itVec != TaskVector().end();
		 itVec++)
    {
        if ((*itVec)->GetOID() == oid)
        {
            BFTask* pTask = (BFTask*)(*itVec);
            TaskVector().erase(itVec);
            delete pTask;
            SetModified(true);
            broadcastObservers();
            return true;
        }
    }

    return false;
}

bool BFRootTaskData::HasTask(BFoid oid)
{
    return (GetTask(oid) != NULL);
}

bool BFRootTaskData::IsModified ()
{
    return bModified_;
}

void BFRootTaskData::SetModified (bool bModified /*= true*/)
{
    bModified_ = bModified;
}

bool BFRootTask::StoreToFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in BFRootTask::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileOutputStream  out(strFilename);
    jbSerialize           archive(out, BF_PROJECT_CURRENT_VERSION);

    if (Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        SetModified(false);
        return true;
    }

    return false;
}


bool BFRootTask::ReadFromFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFRootTask::ReadFromFile()"));
        return false;
    }

    wxFileInputStream   in(strFilename);
    jbSerialize         archive(in, BF_PROJECT_CURRENT_VERSION);

    if (Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        return true;
    }

    return false;
}


bool BFRootTaskData::Serialize (jbSerialize& rA)
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

        for (itVec = TaskVector().begin();
             itVec != TaskVector().end();
             itVec++)
        {
            rc = (*itVec)->Serialize(rA);

            if (!rc)
                break;
        }
    }
    else
    // ** serialize FROM file **
    {
        int iObjCount;

        rA >> strName_;
        projectSettings_.Serialize(rA);
        rA >> iObjCount;

        // ** DEBUG **

        for (int i = 0; i < iObjCount; ++i)
        {
            // create a task with an invalid oid
            BFTask* pTask = new BFTask();

            rc = pTask->Serialize(rA);

            if (rc && pTask != NULL)
                TaskVector().push_back(pTask);
            else
                break;
        }

        broadcastObservers();
    }
    rA.LeaveObject();

    SetModified(false);

    return rc;
}


long BFRootTaskData::GetTaskCount ()
{
    return vecTasks_.size();
}

void BFRootTaskData::SetName (const wxChar* strName)
{
    if (strName == NULL)
        return;

    strName_ = strName;

    SetModified();
}


const wxChar* BFRootTaskData::GetName ()
{
    return strName_;
}


BFProjectSettings& BFRootTaskData::GetSettings ()
{
    return projectSettings_;
}

void BFRootTaskData::ModifyDestination (const wxString& strOldDestination,
                                        const wxString& strNewDestination)
{
    BFTaskVectorIt it;
    bool bMod = false;

    for (it = TaskVector().begin();
         it != TaskVector().end();
         it++)
    {
        if (strOldDestination == (*it)->GetDestination())
        {
            (*it)->SetDestination(strNewDestination);
            bMod = true;
        }
    }

    if (bMod)
        SetModified();
}


BFoid BFRootTaskData::AppendTask (BFTask& rTask)
{
    if ( !(rTask.IsPlausible()) )
        return BFInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return BFInvalidOID;

    // remember the task
    vecTasks_.push_back(&rTask);

    SetModified();
}

BFoid BFRootTaskData::AppendTask (BFTaskType type,
                             const wxChar* strSource,
                             const wxChar* strDestination,
                             const wxChar* strName,
                             bool bVerify,
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
                        archive,
                        arrExclude
                    );

    // remember task
    vecTasks_.push_back(pTask);

    SetModified();
    //broadcastObserver();

    return pTask->GetOID();
}



BFRootTask BFRootTask::sRootTask_;

BFRootTask& BFRootTask::Instance ()
{
    return sRootTask_;
}

BFRootTask::BFRootTask()
          : oidLast_(BFInvalidOID),
            bStopProject_(false),
            bStopTask_(false),
            pRunningTask_(NULL),
            pBackupLog_(NULL),
            pProgressTotal_(NULL),
            pProgressTask_(NULL)
{
}

/*virtual*/ BFRootTask::~BFRootTask()
{
}

BFoid BFRootTask::CreateOID ()
{
    ++oidLast_;

    if (HasTask(oidLast_) || oidLast_ == BFInvalidOID)
        return CreateOID();

    return oidLast_;
}

BFCore& BFRootTask::Core ()
{
    return BFCore::Instance();
}

void BFRootTask::Close ()
{
    ClearTaskVector();
    SetName             (BFROOTTASK_DEFAULT_NAME);
    GetSettings()       .SetDefault();
    SetModified         (false);
    strCurrentFilename_ = wxEmptyString;
    oidLast_            = BFInvalidOID;
    bStopProject_       = false;
    bStopTask_          = false;
    pRunningTask_       = NULL;
    pBackupLog_         = NULL;
    broadcastObservers();
}


Progress* BFRootTask::GetProgressTotal ()
{
    if (pProgressTotal_ == NULL)
        pProgressTotal_ = new ProgressTotal(GetTaskCount(), GetProgressTask());

    return pProgressTotal_;
}


ProgressWithMessage* BFRootTask::GetProgressTask ()
{
    if (pProgressTask_ == NULL)
        pProgressTask_ = new ProgressWithMessage();

    return pProgressTask_;
}

bool BFRootTask::Run_Start ()
{
    if (BFBackupProgressDlg::Instance() == NULL)
        return false;

    /* deactivate the default wxLog target
       and set a new one that handle messages
       with BFSystem */
    wxLog::SetActiveTarget(new BFwxLog);

    // init
    bStopProject_   = false;
    bStopTask_      = false;
    pBackupLog_     = new BFBackupLog();
    pRunningTask_   = NULL;

    // mark the backup start in the logfile
    pBackupLog_->BackupStarted();
    // the core need to create backup messages
    BFCore::Instance().BackupStarted();

    return Run_NextTask ();
}

bool BFRootTask::Run_NextTask ()
{
    // finished the last runned task
    if (pRunningTask_ != NULL)
    {
        // increment task progress
        GetProgressTotal()->IncrementActual();

        // check how the task ended
        if (GetStopCurrentTask())
        {
            pBackupLog_->TaskStoped();
            bStopTask_ = false;
        }
        else
        {
            pBackupLog_->TaskFinished();
        }
    }

    // all tasks runned or stoped?
    if (pRunningTask_ == GetLastTask() || GetStopProject())
        return Run_Finished();

    // get the next task
    pRunningTask_ = GetNextTask(pRunningTask_);

    if (!GetStopProject())
    {
        // log running task
        pBackupLog_->TaskStarted(*pRunningTask_);
        //
        BFBackupProgressDlg::Instance()->SetCurrentTaskName(pRunningTask_->GetName());
        BFThread_ProjectRunner::Run(pRunningTask_);
    }

    return true;
}

bool BFRootTask::Run_Finished ()
{
    // mark the backup end in the log files
    pBackupLog_->BackupFinished();
    // there is no need to create backup messages
    BFCore::Instance().BackupEnded();

    delete pBackupLog_;
    delete pProgressTask_;
    delete pProgressTotal_;
    pBackupLog_     = NULL;
    pProgressTask_  = NULL;
    pProgressTotal_ = NULL;

    // reset the default wxLog target
    delete wxLog::SetActiveTarget(NULL);

    wxGetApp().Sound_BackupFinished();

    BFBackupProgressDlg::Instance()->Close();

    return true;
}

void BFRootTask::InitThat (wxListBox& rListBox)
{
    BFTaskVectorIt itVec;

    for (itVec = TaskVector().begin();
         itVec != TaskVector().end();
         itVec++)
        rListBox.Append((*itVec)->GetName(), (*itVec));
}


void BFRootTask::InitThat (BFBackupTree& rBackupTree)
{
    // add root
    rBackupTree.AddRoot(GetName(), BFIconTable::logo);

    // iterate throug the tasks
    BFTaskVectorIt itVec;

    for (itVec = TaskVector().begin();
         itVec != TaskVector().end();
         itVec++)
    {
        // create all for the task needed items in the tree
        rBackupTree.AddTask
                    (
                        (*itVec)->GetOID(),
                        (*itVec)->GetType(),
                        (*itVec)->GetName(),
                        (*itVec)->GetDestination()
                    );
    }
}

wxArrayString BFRootTask::GetDestinations ()
{
    wxArrayString arr;

    // iterate throug the tasks
    BFTaskVectorIt itVec;

    for (itVec = TaskVector().begin();
         itVec != TaskVector().end();
         itVec++)
        arr.Add((*itVec)->GetDestination());

    return arr;
}

void BFRootTask::StopCurrentTask ()
{
    BFSystem::Backup(_("try to stop the current running task"));

    bStopTask_ = true;
    bStopProject_ = false;

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

void BFRootTask::StopProject ()
{
    BFSystem::Backup(_("try to stop the current running project"));

    bStopTask_ = true;
    bStopProject_ = true;

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

bool BFRootTask::GetStopCurrentTask ()
{
    return bStopTask_;
}

bool BFRootTask::GetStopProject ()
{
    return bStopProject_;
}

const wxString& BFRootTask::GetCurrentFilename ()
{
    return strCurrentFilename_;
}

void BFRootTask::ClearLastLogFiles ()
{
    arrLastLogFiles_.Clear();
}

void BFRootTask::SetProjectLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Insert(strFilename, 0);
}

void BFRootTask::AddTaskLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Add(strFilename);
}

const wxArrayString& BFRootTask::GetLastLogFiles ()
{
    return arrLastLogFiles_;
}
