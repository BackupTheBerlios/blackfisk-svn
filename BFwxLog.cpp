/**
 * Name:        BFwxLog.h
 * Purpose:     BFwxLog class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-15
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

#include "BFwxLog.h"

#include "BFSystem.h"

//
BFwxLog::BFwxLog ()
{
}


//
/*virtual*/ BFwxLog::~BFwxLog ()
{
}


/*virtual*/ void BFwxLog::DoLog(wxLogLevel level,
                                const wxChar *msg,
                                time_t timestamp)
{
    switch (level)
    {
        case wxLOG_FatalError:
            BFSystem::Fatal(msg);
            break;

        case wxLOG_Error:
            BFSystem::Error(msg);
            break;

        case wxLOG_Warning:
            BFSystem::Warning(msg);
            break;

        case wxLOG_Message:
        case wxLOG_Status:
        case wxLOG_Info:
            BFSystem::Info(msg);
            break;

        default:
            BFSystem::Debug(msg);
            break;
    };
}


/*virtual* void BFwxLog::DoLogString(const wxChar *szString, time_t t)
{
    wxString msg(szString);

    msg.Prepend(_("FOREIGN UNKNOWN: "));

    BFSystem::Instance().Message(MsgUNKNOWN, msg, NULL);
}*/
