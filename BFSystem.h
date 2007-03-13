/**
 * BFSystem.h
 * definition of class BFSystem
 * implements message handling
 * for error-, debug-, info-messages, etc
 * 2007-02-21
 ***/

#ifndef BFSYSTEM_H
#define BFSYSTEM_H

#include <wx/wx.h>
#include <wx/datetime.h>

#include "ObserverPattern.h"


enum BFMessageType { MsgINFO, MsgERROR, MsgFATAL, MsgLOG, MsgDEBUG };

/// manage handling of messages for errors and other messages
class BFSystem : public Subject
{
    private:
        /// type of the last message
        BFMessageType       _lastType;
        /// location of the last message
        wxString            _strLastLocation;
        /// the last message itself
        wxString            _strLastMessage;
        /// timestamp of the last message
        wxDateTime          _lastTimestamp;

        /// ctor
        BFSystem();

        /// the instance
        static BFSystem sSystem_;

    public:
        /// dtor
        virtual ~BFSystem();

        ///
        BFMessageType GetLastType ();
        ///
        const wxDateTime& GetLastTimestamp ();
        ///
        const wxString& GetLastLocation ();
        ///
        const wxString& GetLastMessage ();

        ///
        BFMessageType GetLogLevel ();
        ///
        BFMessageType GetMsgLevel ();

        ///
        static BFSystem& Instance ();
        ///
        static const wxChar* GetTypeString (BFMessageType type);
        ///
        static long GetMsgStyle (BFMessageType type);

        /// create a message
        void Message (BFMessageType msgType, const wxChar* strMessage, const wxChar* strLocation);

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


#endif // BFSYSTEM_H
