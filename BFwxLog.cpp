/**
 * Name:        BFwxLog.h
 * Purpose:     BFwxLog class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-15
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 2)
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
