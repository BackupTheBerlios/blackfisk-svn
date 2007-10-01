/**
 * Name:        BFThread_ProjectRunner.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-28
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


#include "BFThread_ProjectRunner.h"
#include "BFRootTask.h"
#include "BFTask.h"
#include "BFCore.h"
#include "ctrlids.h"

DEFINE_EVENT_TYPE(BF_EVENT_THREAD_END)

//
BFThread_ProjectRunner::BFThread_ProjectRunner (BFTask* pTask)
                      : pTask_(pTask)
{
    Create();
    Run();
}


//
/*virtual*/ BFThread_ProjectRunner::~BFThread_ProjectRunner ()
{
}


/*virtual*/ void* BFThread_ProjectRunner::Entry()
{
    wxString str = pTask_->GetDestination();

    // create directory if needed
    if ( !(wxDir::Exists(BFTaskBase::FillBlackfiskPlaceholders(str))) )
        BFCore::Instance().CreatePath(str);

    // run the task
    pTask_->Run( *(BFRootTask::Instance().GetProgressTask()) );

    // finish this and run next task
    BFRootTask::Instance().Run_NextTask();

    // last task
    if ( !(BFCore::Instance().IsWhileBackup()) )
        // send pending event
        wxGetApp().MainFrame()->AddPendingEvent(wxCommandEvent(BF_EVENT_THREAD_END, BF_ID_MAINFRAME));

    return NULL;
}
