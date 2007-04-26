#include "BFSystem.h"

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

BFMessageType BFSystem::GetLogLevel ()
{
    /// log MsgERROR, MsgFATAL, MsgLOG, MsgDEBUG in a file
    return MsgERROR;    /// MsgERROR and higher level
}

BFMessageType BFSystem::GetMsgLevel ()
{
    /// display MsgInfo, MsgERROR, MsgFATAL as a MessageBox
    return MsgFATAL;    /// MsgFATAL and lower level
}
