/**
 * Name:        BFLog.cpp
 * Purpose:     BFLog class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-21
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


#include "BFLog.h"
#include "blackfisk.h"
#include "BFSettings.h"

BFLog::BFLog ()
     : BFLogBase(BF_LOGFILE_NAME, BFSettings::Instance().GetMaxLogFileSize()*1024)
{
}

/*virtual*/ BFLog::~BFLog ()
{
}

void BFLog::Do(BFMessageType type,
               const wxDateTime& timestamp,
               const wxChar* strMessage,
               const wxChar* strLocation)
{
    // check arguments
    if (strMessage == NULL)
        return;

    // result string written to the file
    wxString strLog;

    // create timestamp
    strLog << timestamp.Format(_T("%Y-%m-%d %H:%M:%S"));

    // message type identifier
    if (type != MsgLOG)
        strLog << _T(" ") << wxString(BFSystem::GetTypeString(type)).MakeUpper() << _T(" ");

    strLog << _T(": ");

    // log message
    if (wxStrlen(strLocation) == 0)
        strLog << strMessage << _T('\n');
    else
        strLog << strMessage << _T(" in ") << strLocation << _T('\n');

    // write to the file
    fileLog_.Write(strLog);
}


/*virtual*/ void BFLog::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    // check log-level
    if (pSys->GetLastType() < pSys->GetLogLevel())
    {
        // check for message of type MsgBACKUP
        if (pSys->GetLastType() != MsgBACKUP)
        {
            return;
        }
        else
        {
            // handle MsgBACKUP just if there is no other observer for it
            if (pSys->GetBackupObservers() > 0)
                return;
        }
    }


    // log to file
    Do
    (
        pSys->GetLastType(),
        pSys->GetLastTimestamp(),
        pSys->GetLastMessage().c_str(),
        pSys->GetLastLocation().c_str()
    );
}
