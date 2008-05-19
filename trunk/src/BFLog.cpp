/**
 * Name:        BFLog.cpp
 * Purpose:     BFLog class implementation
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


#include "BFLog.h"
#include "blackfisk.h"
#include "BFSettings.h"

BFLog::BFLog ()
     : BFLogBase(BF_LOGFILE_NAME, BFSettings::Instance().GetMaxLogFileSize()*1024)
{
    fileLog_.Write("\n");
}

/*virtual*/ BFLog::~BFLog ()
{
}

void BFLog::Do(BFMessageType type,
               const wxDateTime& timestamp,
               const wxString& strMessage,
               const wxString& strLocation)
{
    // result string written to the file
    wxString strLog;

    // create timestamp
    strLog << timestamp.Format("%Y-%m-%d %H:%M:%S");

    // message type identifier
    if (type != MsgLOG)
        strLog << " " << wxString(BFSystem::GetTypeString(type)).MakeUpper() << " ";

    strLog << ": ";

    // log message
    if (strLocation.IsEmpty())
        strLog << strMessage << '\n';
    else
        strLog << strMessage << " in " << strLocation << '\n';

    // write to the file
    fileLog_.Write(strLog);
}


/*virtual*/ void BFLog::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    // check log-level
    if (pSys->GetLastType() < BFSettings::Instance().GetVerboseLevelLog())
        if (pSys->GetLastType() != MsgLOG)
            return;

    // log to file
    Do
    (
        pSys->GetLastType(),
        pSys->GetLastTimestamp(),
        pSys->GetLastMessage().c_str(),
        pSys->GetLastLocation().c_str()
    );
}
