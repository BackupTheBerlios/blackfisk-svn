/**
 * Name:        BFSystem.cpp
 * Purpose:     BFSystem class implementation
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

#include "BFSettings.h"

BFSystem BFSystem::sSystem_;

/*static*/ BFSystem& BFSystem::Instance ()
{
    return sSystem_;
}

BFSystem::BFSystem()
        : lBackupObservers_(0)
{
    //ctor
}

BFSystem::~BFSystem()
{
    //dtor
}

long BFSystem::IncrementBackupObservers ()
{
    return ++lBackupObservers_;
}

long BFSystem::DecrementBackupObservers ()
{
    if (lBackupObservers_ > 0)
        --lBackupObservers_;

    return lBackupObservers_;
}

long BFSystem::GetBackupObservers ()
{
    return lBackupObservers_;
}

void BFSystem::Message (BFMessageType msgType,
                                   const wxChar* strMessage,
                                   const wxChar* strLocation)
{
    lastTimestamp_   .SetToCurrent();
    lastType_        = msgType;
    strLastLocation_ = strLocation;
    strLastMessage_  = strMessage;

    broadcastObservers();
}

/*static*/ void BFSystem::Backup (const wxChar* strMessage)
{
    Instance().Message(MsgBACKUP, strMessage, NULL);
}

/*static*/ void BFSystem::Info (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgINFO, strMessage, strLocation);
}

/*static*/ void BFSystem::Log (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgLOG, strMessage, strLocation);
}

/*static*/ void BFSystem::Warning (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgWARNING, strMessage, strLocation);
}

/*static*/ void BFSystem::Fatal (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgFATAL, strMessage, strLocation);
}

/*static*/ void BFSystem::Error (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgERROR, strMessage, strLocation);
}

/*static*/ void BFSystem::Debug (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgDEBUG, strMessage, strLocation);
}

BFMessageType BFSystem::GetLastType ()
{
    return lastType_;
}

/*static*/ wxString BFSystem::GetTypeString (BFMessageType type)
{
    wxString str;

    switch (type)
    {
        case MsgBACKUP:
            str = _("Backup");
            break;

        case MsgINFO:
            str = _("Info");
            break;

        case MsgWARNING:
            str = _("Warning");
            break;

        case MsgERROR:
            str = _("Error");
            break;

        case MsgFATAL:
            str = _("Fatal Error");
            break;

        case MsgLOG:
            str = _("Log");
            break;

        case MsgDEBUG:
            str = _("Debug");
            break;

        case MsgUNKNOWN:
        default:
            str = _("Unknown");
            break;
    };

    return str;
}


/*static*/ long BFSystem::GetMsgStyle (BFMessageType type)
{
    switch (type)
    {
        case MsgINFO:
            return wxICON_INFORMATION;

        case MsgERROR:
            return wxICON_ERROR;
            break;

        case MsgFATAL:
            return wxICON_EXCLAMATION;
            break;

        case MsgLOG:
            return wxICON_INFORMATION;
            break;

        case MsgDEBUG:
            return wxICON_INFORMATION;
            break;

        default:
            return wxICON_STOP;
            break;
    };

    return wxICON_STOP;
}

const wxDateTime& BFSystem::GetLastTimestamp ()
{
    return lastTimestamp_;
}

const wxString& BFSystem::GetLastLocation ()
{
    return strLastLocation_;
}

const wxString& BFSystem::GetLastMessage ()
{
    return strLastMessage_;
}

bool BFSystem::HandleLastMessage (BF_VerboseLevel verbose)
{
    return HandleThisMessage(GetLastType(), verbose);
}

/*static*/ bool BFSystem::HandleThisMessage (BFMessageType msgType, BF_VerboseLevel lvlVerbose)
{
    // check verboseLevel and messageType
    switch (lvlVerbose)
    {
        case BFVERBOSE_FATAL:
            if (msgType != MsgFATAL)
                return false;
            break;

        case BFVERBOSE_ERROR:
            if (msgType != MsgFATAL
             && msgType != MsgERROR)
                return false;
            break;

        case BFVERBOSE_WARNING:
            if (msgType != MsgFATAL
             && msgType != MsgERROR
             && msgType != MsgWARNING)
                return false;
            break;

        case BFVERBOSE_INFO:
            if (msgType != MsgFATAL
             && msgType != MsgERROR
             && msgType != MsgWARNING
             && msgType != MsgBACKUP)
                return false;
            break;
    };

    return true;
}
