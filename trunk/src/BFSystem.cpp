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
        : lBackupObservers_(0),
          bWhileBroadcast_(false)
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
                        const wxString& strMessage,
                        const wxString& strLocation /*= wxEmptyString*/)
{
    // remember the message for later use because another one is currently proceeded
    if (bWhileBroadcast_)
    {
        vecWaiting_Type_.push_back(msgType);
        vecWaiting_Message_.push_back(strMessage);
        vecWaiting_Location_.push_back(strLocation);
        return;
    }

    // set current message
    lastTimestamp_   .SetToCurrent();
    lastType_        = msgType;
    strLastLocation_ = strLocation;
    strLastMessage_  = strMessage;

    /* mark beginning of broadcasting the observers
       all new messages (while broadcasting) will be remmembered */
    bWhileBroadcast_ = true;

    //
    broadcastObservers();

    // check for remembered messages and proceed them
    while (vecWaiting_Type_.size() > 0)
    {
        // set current message
        lastTimestamp_   .SetToCurrent();
        lastType_        = vecWaiting_Type_[0];
        strLastLocation_ = vecWaiting_Location_[0];
        strLastMessage_  = vecWaiting_Message_[0];

        // erase the unremembered messages
        vecWaiting_Type_.erase(vecWaiting_Type_.begin());
        vecWaiting_Message_.erase(vecWaiting_Message_.begin());
        vecWaiting_Location_.erase(vecWaiting_Location_.begin());

        //
        broadcastObservers();
    }

    // mark the end of broadcasting
    bWhileBroadcast_ = false;
}

/*static*/ void BFSystem::Backup (const wxString& strMessage)
{
    Instance().Message(MsgBACKUP, strMessage);
}

/*static*/ void BFSystem::Info (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgINFO, strMessage, strLocation);
}

/*static*/ void BFSystem::Log (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgLOG, strMessage, strLocation);
}

/*static*/ void BFSystem::Warning (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgWARNING, strMessage, strLocation);
}

/*static*/ void BFSystem::Fatal (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgFATAL, strMessage, strLocation);
}

/*static*/ void BFSystem::Error (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgERROR, strMessage, strLocation);
}

/*static*/ void BFSystem::Debug (const wxString& strMessage, const wxString& strLocation /*= wxEmptyString*/)
{
    Instance().Message(MsgDEBUG, strMessage, strLocation);
}

BFMessageType BFSystem::GetLastType ()
{
    return lastType_;
}

/*static*/ wxString BFSystem::GetVerboseString (BF_VerboseLevel verboseLevel)
{
    switch (verboseLevel)
    {
        case BFVERBOSE_ERROR:
            return _("Error");

        case BFVERBOSE_FATAL:
            return _("Fatal Error");

        case BFVERBOSE_WARNING:
            return _("Warning");

        case BFVERBOSE_INFO:
            return _("Information");
    };

    return _("unknown verbose level");
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
            str = "Unknown";
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
