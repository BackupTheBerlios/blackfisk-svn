/**
 * Name:        BFMsgObserver.h
 * Purpose:     BFMsgObserver class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-29
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

#include "BFMsgObserver.h"

#include "BFSystem.h"
#include "BFCore.h"
#include "BFProjectSettings.h"
#include "BFRootTaskApp.h"
#include "BFBackupProgressDlg.h"
#include "BFThread_ProjectRunner.h"
#include "BFMessageDlg.h"

//
BFMsgObserver::BFMsgObserver ()
             : Observer(&(BFSystem::Instance()))
{
}


//
/*virtual*/ BFMsgObserver::~BFMsgObserver ()
{
}


/*virtual*/ void BFMsgObserver::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    // check arguments
    if (pSys == NULL)
        return;

    /* don't show MsgLOG
       MsgLOG is logged by class BFLog in the application log file */
    if (pSys->GetLastType() == MsgLOG)
        return;

    // message and location
    wxString strMsg;

    if (pSys->GetLastLocation().Len() == 0)
        strMsg = pSys->GetLastMessage();
    else
        strMsg = pSys->GetLastMessage() + _T("\nLocation: ") + pSys->GetLastLocation();

    // contact the user
    if ( BFCore::Instance().IsWhileBackup() )
    // ** BACKUP **
    {
        // stop level
        BF_StopLevel stop = BFDO_ASK;

        // get the stop level
        switch (pSys->GetLastType())
        {
            case MsgWARNING:
                stop = BFRootTaskApp::Instance().GetStopLevelOnWarning();
                break;

            case MsgERROR:
                stop = BFRootTaskApp::Instance().GetStopLevelOnError();
                break;

            case MsgFATAL:
                stop = BFRootTaskApp::Instance().GetStopLevelOnFatal();
                break;

            // don't handle other message types while a backup
            default:
                return;
                break;
        };

        // act depending on the stop level
        if (stop == BFDO_ASK)
        {
            /* here we have to leave the backup-worker-thread and let
               the main thread ask the user what whe should do here
               - this backup-worker-thread send an event to the main-thread
               - the main-thread pause the backup-worker-thread
               - aks the user and store the answer somewhere
               - resume the backup-worker-thread
            */
            //stop = BFBackupQuestionDlg::Ask(strMsg, pSys->GetLastType());
            // create an event
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, BF_BACKUPPROGRESSDLG_QUESTION);
            // message type
            event.SetExtraLong  (pSys->GetLastType());
            // the question
            event.SetString     (strMsg);
            //
            BFBackupProgressDlg::Instance()->GetMutex()->Lock();
            // send event
            BFBackupProgressDlg::Instance()->AddPendingEvent(event);
            // want for answer from main-thread/user
            BFBackupProgressDlg::Instance()->GetCondition()->Wait();
            // get the answer
            stop = BFThread_ProjectRunner::CurrentlyRunning()->GetUsersStopAnswer();
        }

        switch (stop)
        {
            case BFDO_IGNORE:
                return;
                break;

            case BFDO_STOPPRJ:
                BFRootTaskApp::Instance().StopProject();
                return;
                break;

            case BFDO_STOPTSK:
                BFRootTaskApp::Instance().StopCurrentTask();
                return;
                break;
        }
    }
    else
    // ** DEFAULT **
    {
        BFMessageDlg dlg(GetMsgStyle(pSys->GetLastType()),
                         strMsg,
                         BFSystem::GetTypeString(pSys->GetLastType()) );
    }
}

/*static*/ BFMessageDlg_Type BFMsgObserver::GetMsgStyle (BFMessageType type)
{
    switch (type)
    {
        case MsgINFO:
            return BF_MSGDLG_INFO;

        case MsgWARNING:
            return BF_MSGDLG_WARNING;

        case MsgERROR:
            return BF_MSGDLG_ERROR;

        case MsgFATAL:
            return BF_MSGDLG_FATAL;

        case MsgLOG:
            return BF_MSGDLG_INFO;

        case MsgDEBUG:
            return BF_MSGDLG_INFO;

        default:
            return BF_MSGDLG_FATAL;
    };

    return BF_MSGDLG_FATAL;
}
