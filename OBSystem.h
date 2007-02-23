/**
 * OBSystem.h
 * definition of class OBSystem
 * implements message handling
 * for error-, debug-, info-messages, etc
 * 2007-02-21
 ***/

#ifndef OBSYSTEM_H
#define OBSYSTEM_H

#include <wx/wx.h>
#include <wx/datetime.h>

#include "ObserverPattern.h"


enum OBMessageType { MsgINFO, MsgERROR, MsgFATAL, MsgLOG, MsgDEBUG };

/// manage handling of messages for errors and other messages
class OBSystem : public Subject
{
    private:
        /// type of the last message
        OBMessageType       _lastType;
        /// location of the last message
        wxString            _strLastLocation;
        /// the last message itself
        wxString            _strLastMessage;
        /// timestamp of the last message
        wxDateTime          _lastTimestamp;

        /// ctor
        OBSystem();

        /// the instance
        static OBSystem sSystem_;

    public:
        /// dtor
        virtual ~OBSystem();

        ///
        OBMessageType GetLastType ();
        ///
        const wxDateTime& GetLastTimestamp ();
        ///
        const wxString& GetLastLocation ();
        ///
        const wxString& GetLastMessage ();

        ///
        OBMessageType GetLogLevel ();
        ///
        OBMessageType GetMsgLevel ();

        ///
        static OBSystem& Instance ();
        ///
        static const wxChar* GetTypeString (OBMessageType type);
        ///
        static long GetMsgStyle (OBMessageType type);

        /// create a message
        void Message (OBMessageType msgType, const wxChar* strMessage, const wxChar* strLocation);

        /// create a info message
        static void Info (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a log message
        static void Log (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a fatal error message
        static void Fatal (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a errror message
        static void Error (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a debug message
        static void Debug (const wxChar* strMessage, const wxChar* strLocation = NULL);
};


#endif // OBSYSTEM_H
