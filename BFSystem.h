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


enum BFMessageType
{
    MsgBACKUP,
    MsgINFO,
    MsgWARNING,
    MsgERROR,
    MsgFATAL,
    MsgLOG,
    MsgDEBUG
};

/// manage handling of messages for errors and other messages
class BFSystem : public Subject
{
    private:
        /// type of the last message
        BFMessageType       lastType_;
        /// location of the last message
        wxString            strLastLocation_;
        /// the last message itself
        wxString            strLastMessage_;
        /// timestamp of the last message
        wxDateTime          lastTimestamp_;
        /// count observers for messages of type MsgBACKUP
        long                lBackupObservers_;

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

        /** increment 'lBackupObservers_' and return the new value */
        long IncrementBackupObservers ();
        /** decrement 'lBackupObservers_' and return the new value
            it won't decrement in a negativ value */
        long DecrementBackupObservers ();
        /** tell you how many observers for messages of the type MsgBACKUP are alaive */
        long GetBackupObservers ();


        ///
        BFMessageType GetLogLevel ();
        ///
        BFMessageType GetMsgLevel ();

        ///
        static BFSystem& Instance ();
        ///
        static wxString GetTypeString (BFMessageType type);
        ///
        static long GetMsgStyle (BFMessageType type);

        /// create a message
        void Message (BFMessageType msgType, const wxChar* strMessage, const wxChar* strLocation);

        /// create a backup message to "log" a running backup process
        static void Backup (const wxChar* strMessage);
        /// create a info message
        static void Info (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a log message
        static void Log (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a warning message
        static void Warning (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a fatal error message
        static void Fatal (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a errror message
        static void Error (const wxChar* strMessage, const wxChar* strLocation = NULL);
        /// create a debug message
        static void Debug (const wxChar* strMessage, const wxChar* strLocation = NULL);
};


#endif // BFSYSTEM_H
