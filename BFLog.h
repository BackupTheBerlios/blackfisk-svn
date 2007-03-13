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
#define BF_LOGFILE_BAKNAME      _T("bf.log.bak")
#define BF_LOGFILE_MAXSIZE      150000


/// it is just log
class BFLog : public Observer
{
    private:
        /// direct interface to the log file
        wxFile      fileLog_;

        /** take care that size of the logfile is not to big */
        void CareSize ();

    public:
        /// the ctor set its 'subject' by itself
        BFLog ();

        /// virtual destructor
        virtual ~BFLog ();

        ///
        virtual void ValueChanged (Subject* pSender);

        /// log a message
        void Do(BFMessageType type,
                const wxDateTime& timestamp,
                const wxChar* strMessage,
                const wxChar* strLocation);


};    // class BFLog

#endif    // BFLOG_H
