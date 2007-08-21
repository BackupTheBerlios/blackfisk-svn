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

#include <wx/msgdlg.h>

#include "BFSystem.h"
#include "BFCore.h"
#include "BFProjectSettings.h"
#include "BFRootTask.h"
#include "BFBackupQuestionDlg.h"

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
                stop = BFRootTask::Instance().GetSettings().GetStopLevelOnWarning();
                break;

            case MsgERROR:
                stop = BFRootTask::Instance().GetSettings().GetStopLevelOnError();
                break;

            case MsgFATAL:
                stop = BFRootTask::Instance().GetSettings().GetStopLevelOnFatal();
                break;

            // don't handle other message types while a backup
            default:
                return;
                break;
        };

        // act depending on the stop level
        if (stop == BFDO_ASK)
            stop = BFBackupQuestionDlg::Ask(strMsg, pSys->GetLastType());

        switch (stop)
        {
            case BFDO_IGNORE:
                return;
                break;

            case BFDO_STOPPRJ:
                BFRootTask::Instance().StopProject();
                return;
                break;

            case BFDO_STOPTSK:
                BFRootTask::Instance().StopCurrentTask();
                return;
                break;
        }
    }
    else
    // ** DEFAULT **
    {
        ::wxMessageBox(strMsg,
                       BFSystem::GetTypeString(pSys->GetLastType()),
                       wxOK | BFSystem::GetMsgStyle(pSys->GetLastType()));
    }
}
