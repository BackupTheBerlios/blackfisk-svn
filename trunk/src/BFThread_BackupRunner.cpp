/**
 * Name:        BFThread_BackupRunner.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-28
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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


#include "BFThread_BackupRunner.h"
#include "BFBackup.h"
#include "BFTask.h"
#include "BFCore.h"
#include "BFBackupProgressDlg.h"
#include "BFMainFrame.h"

DEFINE_EVENT_TYPE(BF_EVENT_THREAD_END)
//wxDEFINE_EVENT(BF_EVENT_THREAD_END, wxCommandEvent);

/*static*/ BFThread_BackupRunner* BFThread_BackupRunner::sp_project_runner_ = NULL;

/*static*/ bool BFThread_BackupRunner::Run (BFOperation* pOperation)
{
    BFMainFrame::Instance()->RememberThread( new BFThread_BackupRunner(pOperation) );

    return true;
}

BFThread_BackupRunner::BFThread_BackupRunner (BFOperation* pOperation)
                      : wxThread(wxTHREAD_JOINABLE),
                        pOperation_(pOperation)
{
    sp_project_runner_ = this;

    Create();
    wxThread::Run();
}


/*virtual*/ BFThread_BackupRunner::~BFThread_BackupRunner ()
{
    sp_project_runner_ = NULL;
}


/*virtual*/ void* BFThread_BackupRunner::Entry()
{
    // run the operation
    pOperation_->Run( *(BFBackup::Instance().GetProgressTask()) );

    // finish this and run next task
    BFBackup::Instance().Run_NextOperation();

    // last task
    if ( !(BFCore::Instance().IsWhileBackup()) )
        // send pending event
        BFMainFrame::Instance()->GetEventHandler()->AddPendingEvent(wxCommandEvent(BF_EVENT_THREAD_END, BF_ID_MAINFRAME));

    return NULL;
}


BF_StopLevel BFThread_BackupRunner::GetUsersStopAnswer ()
{
    return stopAnswer_;
}


void BFThread_BackupRunner::SetUsersStopAnswer (BF_StopLevel stop)
{
    stopAnswer_ = stop;
}
