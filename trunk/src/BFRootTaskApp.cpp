/**
 * Name:        BFRootTaskApp.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-23
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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


#include "BFRootTaskApp.h"

#include <wx/wfstream.h>

#include "BFTask.h"
#include "BFRootTask.h"
#include "BFBackupLog.h"
#include "Progress.h"
#include "BFwxLog.h"
#include "BFBackupProgressDlg.h"
#include "BFCore.h"
#include "BFThread_ProjectRunner.h"
#include "BFApp.h"
#include "BFBackupTree.h"
#include "blackfisk.h"
#include "BFIconTable.h"
#include "BFProcessMsgSubject.h"
#include "BFTaskListCtrl.h"

/*static*/ BFRootTaskApp BFRootTaskApp::sInstance_(&(BFRootTask::Instance()));


//
BFRootTaskApp::BFRootTaskApp (BFRootTask* pRootTask)
             : pRootTask_(pRootTask),
               bStopProject_(false),
               bStopTask_(false),
               pRunningTask_(NULL),
               pBackupLog_(NULL),
               pProgressTotal_(NULL),
               pProgressTask_(NULL)
{
}


//
/*virtual*/ BFRootTaskApp::~BFRootTaskApp ()
{
}

void BFRootTaskApp::Close ()
{
    pRootTask_->Close();
    strCurrentFilename_ = wxEmptyString;
    bStopProject_       = false;
    bStopTask_          = false;
    pRunningTask_       = NULL;
    pBackupLog_         = NULL;
}

long BFRootTaskApp::GetTaskPosition (BFTask* pTask)
{
    return pRootTask_->FindTask(pTask);
}

long BFRootTaskApp::GetTaskCount ()
{
    return pRootTask_->GetTaskCount();
}

Progress* BFRootTaskApp::GetProgressTotal ()
{
    if (pProgressTotal_ == NULL)
        pProgressTotal_ = new ProgressTotal(pRootTask_->GetTaskCount(), GetProgressTask());

    return pProgressTotal_;
}


ProgressWithMessage* BFRootTaskApp::GetProgressTask ()
{
    if (pProgressTask_ == NULL)
        pProgressTask_ = new ProgressWithMessage();

    return pProgressTask_;
}

const wxChar* BFRootTaskApp::GetProjectName()
{
    return pRootTask_->GetName();
}

BF_VerboseLevel BFRootTaskApp::GetVerboseLevel ()
{
    return pRootTask_->GetSettings().GetVerboseLevel();
}

BF_StopLevel BFRootTaskApp::GetStopLevelOnFatal ()
{
    return pRootTask_->GetSettings().GetStopLevelOnFatal();
}

BF_StopLevel BFRootTaskApp::GetStopLevelOnError ()
{
    return pRootTask_->GetSettings().GetStopLevelOnError();
}

BF_StopLevel BFRootTaskApp::GetStopLevelOnWarning ()
{
    return pRootTask_->GetSettings().GetStopLevelOnWarning();
}

/*static*/ wxString BFRootTaskApp::GetStopLevelString (BF_StopLevel stopLevel)
{
    switch (stopLevel)
    {
        case BFDO_ASK:
            return _("ask");

        case BFDO_IGNORE:
            return _("log");

        case BFDO_STOPTSK:
            return _("stop task");

        case BFDO_STOPPRJ:
            return _("stop project");
    };

    return _("unknown stopLevel");
}

bool BFRootTaskApp::IsProjectModified ()
{
    return pRootTask_->IsModified();
}

bool BFRootTaskApp::Run_Start ()
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

bool BFRootTaskApp::Run_NextTask ()
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
    if (pRunningTask_ == pRootTask_->GetLastTask() || GetStopProject())
        return Run_Finished();

    // get the next task
    pRunningTask_ = pRootTask_->GetNextTask(pRunningTask_);

    if (!GetStopProject())
    {
        // log running task
        pBackupLog_->TaskStarted(*pRunningTask_);
        //
        BFBackupProgressDlg::Instance()->SetCurrentTask(pRunningTask_);
        BFThread_ProjectRunner::Run(pRunningTask_);
    }

    return true;
}

bool BFRootTaskApp::Run_Finished ()
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


void BFRootTaskApp::StopCurrentTask ()
{
    bStopTask_ = true;
    bStopProject_ = false;

    BFSystem::Backup(_("try to stop the current running task"));

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

void BFRootTaskApp::StopProject ()
{
    bStopTask_ = true;
    bStopProject_ = true;

    BFSystem::Backup(_("try to stop the current running project"));

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

bool BFRootTaskApp::GetStopCurrentTask ()
{
    return bStopTask_;
}

bool BFRootTaskApp::GetStopProject ()
{
    return bStopProject_;
}

const wxString& BFRootTaskApp::GetCurrentFilename ()
{
    return strCurrentFilename_;
}

const wxString& BFRootTaskApp::GetBackupLogLocation ()
{
    return pRootTask_->GetSettings().GetBackupLogLocation();
}

void BFRootTaskApp::ClearLastLogFiles ()
{
    arrLastLogFiles_.Clear();
}

void BFRootTaskApp::SetProjectLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Insert(strFilename, 0);
}

void BFRootTaskApp::AddTaskLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Add(strFilename);
}

const wxArrayString& BFRootTaskApp::GetLastLogFiles ()
{
    return arrLastLogFiles_;
}

void BFRootTaskApp::InitThat (BFBackupTree* pBackupTree)
{
    if (pBackupTree == NULL)
        return;

    // delete all items in the treeCtrl
    pBackupTree->DeleteAllItems();

    // add root
    pBackupTree->AddRoot(GetProjectName(), BFIconTable::logo);

    BFTaskVector vec;
    pRootTask_->GetAllTasks(vec);

    // iterate throug the tasks
    for (BFTaskVectorIt itVec = vec.begin();
         itVec != vec.end();
         ++itVec)
    {
        // create all for the task needed items in the tree
        pBackupTree->AddTask
        (
            (*itVec)->GetOID(),
            (*itVec)->GetType(),
            (*itVec)->GetName(),
            (*itVec)->GetDestination()
        );
    }
}

void BFRootTaskApp::InitThat (BFTaskListCtrl* pTaskList)
{
    if (pTaskList == NULL)
        return;

    BFTaskVector vec;
    pRootTask_->GetAllTasks(vec);

    // init controls
    for (BFTaskVectorIt itVec = vec.begin();
         itVec != vec.end();
         ++itVec)
        pTaskList->Append((*itVec));
}

bool BFRootTaskApp::StoreToFile (const wxString& strFilename)
{
    wxFileOutputStream  out(strFilename);
    jbSerialize         archive(out, BF_PROJECT_CURRENT_VERSION);

    if (pRootTask_->Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        pRootTask_->SetModified(false);
        return true;
    }

    return false;
}


bool BFRootTaskApp::ReadFromFile (const wxString& strFilename)
{
    wxFileInputStream   in(strFilename);
    jbSerialize         archive(in, BF_PROJECT_CURRENT_VERSION);

    if (pRootTask_->Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        return true;
    }

    return false;
}

void BFRootTaskApp::ModifyDestination (const wxString& strOldDestination,
                                       const wxString& strNewDestination)
{
    bool bMod = false;
    wxString strCurrDest;
    BFTaskVector vec;

    pRootTask_->GetAllTasks(vec);

    for (BFTaskVectorIt it = vec.begin();
         it != vec.end();
         ++it)
    {
        // the destination of the current task
        strCurrDest = (*it)->GetDestination();

        // has the destiantion to modify?
        if (strCurrDest.StartsWith(strOldDestination))
        {
            // replace old with new destination
            strCurrDest.Replace(strOldDestination, strNewDestination);
            // set destination to the task
            (*it)->SetDestination(strCurrDest);
            // remember to mark the project as modified
            bMod = true;
        }
    }

    // mark the project modified if needed
    if (bMod)
        pRootTask_->SetModified();
}

bool BFRootTaskApp::PreBackupCheck (BFProcessMsgSubject* pMsg)
{
    // caption
    if (pMsg)
        pMsg->SetCaption(_("please wait..."));

    // get the tasks
    wxString str;
    BFTaskVector vec;
    pRootTask_->GetAllTasks(vec);

    // iterate on them
    for (BFTaskVectorIt it = vec.begin();
         it != vec.end();
         ++it)
    {
        // * check volume *
        str = (*it)->GetDestination();
        str = str.BeforeFirst(wxFILE_SEP_PATH);

        if (pMsg)
            pMsg->SetMsg(wxString::Format(_("current checking Task is %s\ncheck volume: %s"),
                                          (*it)->GetName(),
                                          str));

        if ( wxDir::Exists( str ) == false )
        {
            BFSystem::Error(wxString::Format(_("The destination %s doesn't exsits!"), str));
            return false;
        }

        // * check source *
        str = (*it)->GetSource();

        if (pMsg)
            pMsg->SetMsg(wxString::Format(_("current checking Task is %s\nvolume OK\ncheck source: %s"),
                                          (*it)->GetName(),
                                          str));

        if ((*it)->GetType() == TaskFILECOPY)
        {
            if ( ::wxFileExists( str ) == false
                && pRootTask_->FindLastTaskWithDestination(str) == -1)
            {
                BFSystem::Error(wxString::Format(_("The source %s doesn't exsits!"), str));
                return false;
            }
        }
        else
        {
            if ( wxDir::Exists( str ) == false
              && pRootTask_->FindLastTaskWithDestination(str) == -1)
            {
                BFSystem::Error(wxString::Format(_("The source %s doesn't exsits!"), str));
                return false;
            }
        }
    }

    return true;
}
