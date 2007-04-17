/**
 * BFLog.h
 * definition of class BFLog
 * <comment>
 * 2007-02-21
 ***/

#ifndef BFLOG_H
#define BFLOG_H

#include <wx/file.h>

#include "ObserverPattern.h"
#include "BFSystem.h"

#define BF_DEFAULT_OVERWRITE       true
#define BF_LOGFILE_NAME         _T("bf.log")
#define BF_LOGFILE_BAKSUFFIX    _T(".bak")
#define BF_LOGFILE_MAXSIZE      150000


/// base class for a log-file class
class BFLogBase : public Observer
{
    private:
        /** max size of a file in bytes
            it is handled in CareSize()
            set it to '0' if the size does not matter */
        long        lMaxSize_;

    protected:
        /// the name of the logfile
        wxString    strLogFileName_;

        /// direct interface to the log file
        wxFile      fileLog_;

        /** take care that size of the logfile is not to big */
        void CareSize ();

        /// log a message
        void Do(BFMessageType type,
                const wxDateTime& timestamp,
                const wxChar* strMessage,
                const wxChar* strLocation);

        ///
        long GetMaxSize ();
        ///
        void SetMaxSize (long lSize);

        /// the ctor set its 'subject' by itself
        BFLogBase (const wxChar* strLogFileName, long lMaxSize = 0);

    public:
        /// virtual destructor
        virtual ~BFLogBase ();
};    // class BFLogBase


/// it is just log
class BFLog : public BFLogBase
{
    public:
        ///
        BFLog (const wxChar* strLogFileName);

        /// virtual destructor
        virtual ~BFLog ();

        ///
        virtual void ValueChanged (Subject* pSender);
};    // class BFLog

#endif    // BFLOG_H
