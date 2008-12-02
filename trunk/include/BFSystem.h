/**
 * Name:        BFSystem.h
 * Purpose:     BFSystem class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-21
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

#ifndef BFSYSTEM_H
#define BFSYSTEM_H

#include <wx/wx.h>
#include <wx/datetime.h>

#include "ObserverPattern.h"

#include "BFSystemBase.h"


/// manage handling of messages for errors and other messages
class BFSystem : public Subject
{
    private:
        /// type of the last message
        BFMessageType       lastType_;
        /// location of the last message
        wxString            strLastLocation_;
        /// the last message itself
        wxString            strLastMessage_;
        /// timestamp of the last message
        wxDateTime          lastTimestamp_;
        /** Seconds to wait before the message apear.
            This affects only message observers (e.g. BFMessageDlg)
            that handle this value.
            If '0' no timer is set. */
        long                lLastTimerSec_;

        /// count observers for messages of type MsgBACKUP
        long                lBackupObservers_;

        /** if there are more then one message at one time
            they need to be stored for later processing
            please see BFSystem::Message() for more detailes */
        std::vector<BFMessageType>  vecWaiting_Type_;
        /// see 'vecWaiting_Type_' for more detailes
        std::vector<wxString>       vecWaiting_Message_;
        /// see 'vecWaiting_Type_' for more detailes
        std::vector<wxString>       vecWaiting_Location_;
        /// see 'vecWaiting_Type_' for more detailes
        std::vector<long>           vecWaiting_TimerSec_;
        /// see 'vecWaiting_Type_' for more detailes
        bool                        bWhileBroadcast_;


        /// ctor
        BFSystem();

        /// the instance
        static BFSystem sSystem_;

    public:
        /// dtor
        virtual ~BFSystem();

        ///
        BFMessageType GetLastType ();
        ///
        const wxDateTime& GetLastTimestamp ();
        ///
        const wxString& GetLastLocation ();
        ///
        const wxString& GetLastMessage ();
        ///
        const long GetLastTimerSec ();

        /** return true if the last message should be handled
            depending on its message-type and the specified
            'verbose' level */
        bool HandleLastMessage (BF_VerboseLevel verbose);

        /** increment 'lBackupObservers_' and return the new value */
        long IncrementBackupObservers ();
        /** decrement 'lBackupObservers_' and return the new value
            it won't decrement in a negativ value */
        long DecrementBackupObservers ();
        /** tell you how many observers for messages of the type MsgBACKUP are alaive */
        long GetBackupObservers ();

        ///
        static BFSystem& Instance ();
        ///
        static wxString GetVerboseString (BF_VerboseLevel verboseLevel);
        ///
        static wxString GetTypeString (BFMessageType type);
        /** return true if a message of type 'msgType' should be
            handled depending on the verbose level 'lvlVerbose' */
        static bool HandleThisMessage (BFMessageType msgType, BF_VerboseLevel lvlVerbose);

        /// create a message
        void Message (BFMessageType msgType,
                      const wxString& strMessage,
                      const wxString& strLocation = wxEmptyString,
                      long lTimerSec = 0);

        /// create a backup message to "log" a running backup process
        static void Backup (const wxString& strMessage);
        /// create a info message
        static void Info (const wxString& strMessage, const wxString& strLocation = wxEmptyString);
        /// create a log message
        static void Log (const wxString& strMessage, const wxString& strLocation = wxEmptyString);
        /// create a warning message
        static void Warning (const wxString& strMessage, const wxString& strLocation = wxEmptyString);
        /// create a fatal error message
        static void Fatal (const wxString& strMessage, const wxString& strLocation = wxEmptyString);
        /// create a errror message
        static void Error (const wxString& strMessage,
                           const wxString& strLocation = wxEmptyString,
                           long lTimerSec = 0);
        /// create a debug message
        static void Debug (const wxString& strMessage, const wxString& strLocation = wxEmptyString);
};


#endif // BFSYSTEM_H
