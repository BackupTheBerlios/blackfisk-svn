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
#include "BFBackup.h"
#include "BFBackupProgressDlg.h"
#include "BFThread_BackupRunner.h"
#include "BFMessageDlg.h"
#include "BFEnvironment.h"
#include "blackfisk.h"

BFMsgObserver::BFMsgObserver ()
             : Observer(&(BFSystem::Instance()))
{
}

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
        strMsg = pSys->GetLastMessage() + "\nLocation: " + pSys->GetLastLocation();

    // contact the user
    if ( BFCore::Instance().IsWhileBackup() )
    // ** BACKUP **
    {
        // get stop level
        BF_StopLevel stop = BFBackup::GetStopLevel(pSys->GetLastType());

        // don't handle other message types while a backup
        if ( stop == BFDO_UNKNOWN )
            return;

        // act depending on the stop level
        if (stop == BFDO_ASK)
        {
            /* here we have to leave the backup-worker-thread and let
               the main thread ask the user what whe should do here
               - this backup-worker-thread send an event to the main-thread
               - the main-thread pause the backup-worker-thread
               - aks the user and store the answer somewhere
               - resume the backup-worker-thread
               - get the stored answer
            */
            // create an event
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, BF_BACKUPPROGRESSDLG_QUESTION);
            // message type
            event.SetExtraLong  (pSys->GetLastType());
            // the question
            event.SetString     (strMsg);
            //
            BFBackupProgressDlg::Instance()->GetMutex()->Lock();
            // send event
            BFBackupProgressDlg::Instance()->GetEventHandler()->AddPendingEvent(event);
            // wait for answer from main-thread/user
            BFBackupProgressDlg::Instance()->GetCondition()->Wait();
            // get the answer
            stop = BFThread_BackupRunner::CurrentlyRunning()->GetUsersStopAnswer();
        }

        switch (stop)
        {
            case BFDO_IGNORE:
                return;
                break;

            case BFDO_STOPPRJ:
                BFBackup::Instance().StopBackup();
                return;
                break;

            case BFDO_STOPTSK:
                BFBackup::Instance().StopCurrentOperation();
                return;
                break;

            default:
                // should never be reached
                break;

        }
    }
    else
    // ** DEFAULT **
    {
        long lTimerSec = pSys->GetLastTimerSec();

        /* If the backup was started by a scheduler all message dialogs
           need to wait some seconds. */
        if ( lTimerSec == 0 && BFEnvironment::IsProjectScheduled() )
            lTimerSec = BF_MSG_DEFAULT_TIMER;

        BFMessageDlg dlg(GetMsgStyle(pSys->GetLastType()),
                         strMsg,
                         BFSystem::GetTypeString(pSys->GetLastType()),
                         BF_MSGDLG_NOTUSED,
                         lTimerSec );
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
