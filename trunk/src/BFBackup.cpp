/**
 * Name:        BFBackup.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-22
 *              (based on BFRootTaskApp)
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


#include "BFBackup.h"

#include <wx/wfstream.h>
#include <wx/textfile.h>

#include "BFTask.h"
#include "BFProject.h"
#include "BFBackupLog.h"
#include "Progress.h"
#include "BFwxLog.h"
#include "BFBackupProgressDlg.h"
#include "BFCore.h"
#include "BFThread_BackupRunner.h"
#include "BFApp.h"
#include "BFBackupTree.h"
#include "blackfisk.h"
#include "BFIconTable.h"
#include "BFTaskListCtrl.h"
#include "BFSettings.h"
#include "BFSound.h"
#include "BFEnvironment.h"

/*static*/ BFBackup BFBackup::sInstance_(&(BFProject::Instance()));


//
BFBackup::BFBackup (BFProject* pProject)
             : pProject_(pProject),
               lRunningOperation_(-1),
               bStopBackup_(false),
               bStopOperation_(false),
               pBackupLog_(NULL),
               pProgressTotal_(NULL),
               pProgressTask_(NULL)
{
}


//
/*virtual*/ BFBackup::~BFBackup ()
{
}

void BFBackup::Reset ()
{
    pProject_->Reset();
	pProject_->GetSettings() = BFSettings::Instance().GetDefaultProjectSettings();

    ClearOperationVector();
    strCurrentFilename_ = wxEmptyString;
    bStopBackup_        = false;
    bStopOperation_     = false;
    lRunningOperation_  = -1;
    pBackupLog_         = NULL;
}

void BFBackup::ClearOperationVector ()
{
    BFOperationVectorIt itVec;

	for (itVec = vecOperations_.begin();
		 itVec != vecOperations_.end();
		 ++itVec)
        delete (*itVec);

    vecOperations_.clear();
}

long BFBackup::GetTaskPosition (BFOperation* pOperation)
{
    return pProject_->FindTask(pOperation->Task());
}

long BFBackup::GetTaskCount ()
{
    return pProject_->GetTaskCount();
}

Progress* BFBackup::GetProgressTotal ()
{
    if (pProgressTotal_ == NULL)
        pProgressTotal_ = new ProgressTotal(pProject_->GetTaskCount(), GetProgressTask());

    return pProgressTotal_;
}


ProgressWithMessage* BFBackup::GetProgressTask ()
{
    if (pProgressTask_ == NULL)
        pProgressTask_ = new ProgressWithMessage();

    return pProgressTask_;
}

const wxString& BFBackup::GetProjectName()
{
    return pProject_->GetName();
}

BF_VerboseLevel BFBackup::GetVerboseLevel ()
{
    return pProject_->GetSettings().GetVerboseLevel();
}

/*static*/ BF_StopLevel BFBackup::GetStopLevel (BFMessageType type)
{
    // get the stop level
    switch (type)
    {
        case MsgWARNING:
            return Instance().pProject_->GetSettings().GetStopLevelOnWarning();

        case MsgERROR:
            return Instance().pProject_->GetSettings().GetStopLevelOnError();

        case MsgFATAL:
            return Instance().pProject_->GetSettings().GetStopLevelOnFatal();

        // there is now "stop level" for other message types
        default:
            return BFDO_UNKNOWN;
    };
}

/*static*/ wxString BFBackup::GetStopLevelString (BF_StopLevel stopLevel)
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

        default:
            return _("unknown stopLevel");
    };
}

bool BFBackup::IsProjectModified ()
{
    return pProject_->IsModified();
}

bool BFBackup::Run_Start ()
{
    if (BFBackupProgressDlg::Instance() == NULL)
        return false;

    // init
    bStopBackup_        = false;
    bStopOperation_     = false;
    pBackupLog_         = new BFBackupLog();
    lRunningOperation_  = -1;

    // create operation list
    BFTask* pTask = NULL;

    do
    {
        pTask = pProject_->GetNextTask (pTask);
        vecOperations_.push_back ( new BFOperation(pTask) );
    }
    while ( pTask != pProject_->GetLastTask() );


    // mark the backup start in the logfile
    pBackupLog_->BackupStarted();
    // the core need to create backup messages
    BFCore::Instance().BackupStarted();

    return Run_NextOperation ();
}

bool BFBackup::Run_NextOperation ()
{
    // finished the last runned task
    if (lRunningOperation_ != -1)
    {
        // increment task progress
        GetProgressTotal()->IncrementActual();

        // check how the task ended
        if (GetStopCurrentOperation())
        {
            pBackupLog_->TaskStoped();
            bStopOperation_ = false;
        }
        else
        {
            pBackupLog_->TaskFinished();
        }
    }

    // all operations runned or stoped?
    if ( lRunningOperation_ == (((long)vecOperations_.size())-1) || GetStopBackup() )
        return Run_Finished();

    // get the next task
    lRunningOperation_ += 1;

    if (!GetStopBackup())
    {
        // log running task
        pBackupLog_->TaskStarted( *(vecOperations_[lRunningOperation_]->Task()) );
        //
        BFBackupProgressDlg::Instance()->SetCurrentOperation( vecOperations_[lRunningOperation_] );
        BFThread_BackupRunner::Run( vecOperations_[lRunningOperation_] );
    }

    return true;
}

bool BFBackup::Run_Finished ()
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

    BFSound::Finish();

    BFBackupProgressDlg::Instance()->Close();

    ClearOperationVector();
    bStopBackup_        = false;
    bStopOperation_     = false;
    lRunningOperation_  = -1;
    pBackupLog_         = NULL;

    /* If the project was started automaticly from
       commandline or scheduler, blackfisk need
       to close after the backup. */
    if ( BFEnvironment::IsProjectScheduled() )
        wxGetApp().Exit();

    return true;
}


void BFBackup::StopCurrentOperation ()
{
    bStopOperation_ = true;
    bStopBackup_    = false;

    BFSystem::Backup(_("try to stop the current running task"));

    vecOperations_[lRunningOperation_]->StopOperation();
}

void BFBackup::StopBackup ()
{
    bStopOperation_ = true;
    bStopBackup_    = true;

    BFSystem::Backup(_("try to stop the current running backup"));

    vecOperations_[lRunningOperation_]->StopOperation();
}

bool BFBackup::GetStopCurrentOperation ()
{
    return bStopOperation_;
}

bool BFBackup::GetStopBackup ()
{
    return bStopBackup_;
}

const wxString& BFBackup::GetCurrentFilename ()
{
    return strCurrentFilename_;
}

wxString BFBackup::GetBackupLogLocation ()
{
    return pProject_->GetSettings().GetBackupLogLocation();
}

void BFBackup::ClearLastLogFiles ()
{
    arrLastLogFiles_.Clear();
}

void BFBackup::SetProjectLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Insert(strFilename, 0);
}

void BFBackup::AddTaskLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Add(strFilename);
}

const wxArrayString& BFBackup::GetLastLogFiles ()
{
    return arrLastLogFiles_;
}

void BFBackup::InitThat (BFBackupTree* pBackupTree)
{
    if (pBackupTree == NULL)
        return;

    // delete all items in the treeCtrl
    pBackupTree->DeleteAllItems();

    // add root
    pBackupTree->AddRoot(GetProjectName(), BFIconTable::logo);

    BFTaskVector vec;
    pProject_->GetAllTasks(vec);

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

void BFBackup::InitThat (BFTaskListCtrl* pTaskList)
{
    if (pTaskList == NULL)
        return;

    BFTaskVector vec;
    pProject_->GetAllTasks(vec);

    // init controls
    for (BFTaskVectorIt itVec = vec.begin();
         itVec != vec.end();
         ++itVec)
        pTaskList->Append((*itVec));
}

bool BFBackup::StoreToFile (const wxString& strFilename)
{
    wxFileOutputStream  out(strFilename);
    jbSerialize         archive(out, BF_PROJECT_CURRENT_VERSION);

    if (pProject_->Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        pProject_->SetModified(false);

        if (archive.GetVersion() < 1030)
            pProject_->SetOriginalCrontabLine(GetCrontabline());

        return true;
    }

    return false;
}


bool BFBackup::ReadFromFile (const wxString& strFilename)
{
    wxFileInputStream   in(strFilename);
    jbSerialize         archive(in, BF_PROJECT_CURRENT_VERSION);

    if (pProject_->Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        return true;
    }

    return false;
}
/*
void BFBackup::ModifyDestination (const wxString& strOldDestination,
                                  const wxString& strNewDestination)
{
    bool bMod = false;
    wxString strCurrDest;
	wxString strOldDest = strOldDestination;
	wxString strNewDest = strNewDestination;
    BFTaskVector vec;

	if ( strOldDest.Last() == wxFILE_SEP_PATH )
		strOldDest.RemoveLast();

	if ( strNewDest.Last() == wxFILE_SEP_PATH )
		strNewDest.RemoveLast();

    pProject_->GetAllTasks(vec);

    for (BFTaskVectorIt it = vec.begin();
         it != vec.end();
         ++it)
    {
        // the destination of the current task
        strCurrDest = (*it)->GetDestination();

        // has the destiantion to modify?
        if (strCurrDest.StartsWith(strOldDest))
        {
			// replace old with new destination
            strCurrDest.Replace(strOldDest, strNewDest);

			while ( strCurrDest.EndsWith(wxFILE_SEP_PATH) )
				strCurrDest.RemoveLast(1);

			strCurrDest << wxFILE_SEP_PATH;

            // set destination to the task
            (*it)->SetDestination(strCurrDest);
            // remember to mark the project as modified
            bMod = true;
        }
    }

    // mark the project modified if needed
    if (bMod)
        pProject_->SetModified();
}*/

bool BFBackup::PreBackupCheck ()
{
    BFSystem::Log(wxString::Format(_("PreBackup Check for %s ..."), GetCurrentFilename()));

    // get the tasks
    wxString str;
    BFTaskVector vec;
    pProject_->GetAllTasks(vec);

    // iterate on them
    for (BFTaskVectorIt it = vec.begin();
         it != vec.end();
         ++it)
    {
        // * check volume *
        str = (*it)->GetDestination();
        str = str.BeforeFirst(wxFILE_SEP_PATH);

        if ( wxDir::Exists( str ) == false )
        {
            BFSystem::Error
            (
                wxString::Format(_("The volume %s doesn't exsits!\nPlease connect the volume to the system!"), str),
                wxEmptyString,
                60  // The user has 60 seconds to connect the volume.
            );

            // try again
            if ( wxDir::Exists( str ) == false )
            {
                BFSystem::Error( wxString::Format(_("The volume %s doesn't exists!"), str) );

                return false;
            }
        }

        // * check source *
        str = (*it)->GetSource();

        if ((*it)->GetType() == TaskFILECOPY)
        {
            if ( ::wxFileExists( str ) == false
                && pProject_->FindLastTaskWithDestination(str) == -1)
            {
                BFSystem::Error(wxString::Format(_("The source %s doesn't exsits!"), str));
                return false;
            }
        }
        else
        {
            if ( wxDir::Exists( str ) == false
              && pProject_->FindLastTaskWithDestination(str) == -1)
            {
                BFSystem::Error(wxString::Format(_("The source %s doesn't exsits!"), str));
                return false;
            }
        }
    }

    return true;
}

wxString BFBackup::GetCrontabline ()
{
    // the crontab-file
    wxTextFile file(BFSettings::Instance().GetCrontab());

    // open it
    if ( file.Open() == false )
        return wxEmptyString;

    // iterate over the lines
    for (wxString strLine = file.GetFirstLine();
         !(file.Eof());
         strLine = file.GetNextLine())
    {
        if ( strLine.StartsWith("#") )
            continue;

        if ( strLine.Find(strCurrentFilename_) != wxNOT_FOUND
		  && strLine.Find(wxTheApp->argv[0]) != wxNOT_FOUND )
            return strLine;
    }

    return wxEmptyString;
}

/*static*/ wxArrayString& BFBackup::ParseCrontabline (const wxString& strLine, wxArrayString& arr)
{
    wxString        str(strLine);
    wxString        strCurrent;
    size_t          idx;

    arr.Clear();

    // extract the first five parameters
    while ( !(str.IsEmpty()) && arr.GetCount() < 5 )
    {
        // find white spaces at the beginning
        idx = 0;
        while (str[idx] == ' ' || idx == str.Length())
            ++idx;

        // erase white spaces at the beginning
        str = str.Mid(idx);

        //
        strCurrent = str.BeforeFirst(' ');
        str = str.Mid(strCurrent.Length());

        //
        arr.Add(strCurrent);
    }

    // remember the unparsed rest
    if ( !(str.IsEmpty()) )
        arr.Add(str);

    // enough parameters
    if (arr.GetCount() < 6)
        arr.Clear();

    // default?
    if ( arr.IsEmpty() )
		ParseCrontabline ( BFBackup::Instance().GetCrontablineDefault(), arr );

    return arr;
}

/*static*/ wxString& BFBackup::FillBlackfiskPlaceholders (wxString& rStr)
{
    // BFTASK_PLACEHOLDER_DATE
    rStr.Replace(BFTASK_PLACEHOLDER_DATE, BFCore::Instance().GetDateString());

    // BFTASK_PLACEHOLDER_TIME
    rStr.Replace(BFTASK_PLACEHOLDER_TIME, BFCore::Instance().GetTimeString());

    return rStr;
}


wxString BFBackup::GetCrontablineDefault ()
{
	return wxString::Format( BF_CRONTABLINE_DEFAULT,
							 wxTheApp->argv[0],
							 GetCurrentFilename() );
}
