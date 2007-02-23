#include "OBSystem.h"

OBSystem OBSystem::sSystem_;

/*static*/ OBSystem& OBSystem::Instance ()
{
    return sSystem_;
}

OBSystem::OBSystem()
{
    //ctor
}

OBSystem::~OBSystem()
{
    //dtor
}


void OBSystem::Message (OBMessageType msgType,
                                   const wxChar* strMessage,
                                   const wxChar* strLocation)
{
    _lastTimestamp   .SetToCurrent();
    _lastType        = msgType;
    _strLastLocation = strLocation;
    _strLastMessage  = strMessage;
}

/*static*/ void OBSystem::Info (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgINFO, strMessage, strLocation);
}

/*static*/ void OBSystem::Log (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgLOG, strMessage, strLocation);
}

/*static*/ void OBSystem::Fatal (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgFATAL, strMessage, strLocation);
}

/*static*/ void OBSystem::Error (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgERROR, strMessage, strLocation);
}

/*static*/ void OBSystem::Debug (const wxChar* strMessage, const wxChar* strLocation/* = NULL*/)
{
    Instance().Message(MsgDEBUG, strMessage, strLocation);
}

OBMessageType OBSystem::GetLastType ()
{
    return _lastType;
}

/*static*/ const wxChar* OBSystem::GetTypeString (OBMessageType type)
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


/*static*/ long OBSystem::GetMsgStyle (OBMessageType type)
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

const wxDateTime& OBSystem::GetLastTimestamp ()
{
    return _lastTimestamp;
}

const wxString& OBSystem::GetLastLocation ()
{
    return _strLastLocation;
}

const wxString& OBSystem::GetLastMessage ()
{
    return _strLastMessage;
}

OBMessageType OBSystem::GetLogLevel ()
{
    /// log MsgERROR, MsgFATAL, MsgLOG, MsgDEBUG in a file
    return MsgERROR;    /// MsgERROR and higher level
}

OBMessageType OBSystem::GetMsgLevel ()
{
    /// display MsgInfo, MsgERROR, MsgFATAL as a MessageBox
    return MsgFATAL;    /// MsgFATAL and lower level
}
