#include "BFSystem.h"

BFSystem BFSystem::sSystem_;

/*static*/ BFSystem& BFSystem::Instance ()
{
    return sSystem_;
}

BFSystem::BFSystem()
{
    //ctor
}

BFSystem::~BFSystem()
{
    //dtor
}


void BFSystem::Message (BFMessageType msgType,
                                   const wxChar* strMessage,
                                   const wxChar* strLocation)
{
    _lastTimestamp   .SetToCurrent();
    _lastType        = msgType;
    _strLastLocation = strLocation;
    _strLastMessage  = strMessage;
}

/*static*/ void BFSystem::Info (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgINFO, strMessage, strLocation);
}

/*static*/ void BFSystem::Log (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgLOG, strMessage, strLocation);
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
    return _lastType;
}

/*static*/ const wxChar* BFSystem::GetTypeString (BFMessageType type)
{
    wxString str;

    switch (type)
    {
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

    return str.c_str();
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
    return _lastTimestamp;
}

const wxString& BFSystem::GetLastLocation ()
{
    return _strLastLocation;
}

const wxString& BFSystem::GetLastMessage ()
{
    return _strLastMessage;
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
