/**
 * BFLog.h
 * definition of class BFLog
 * <comment>
 * 2007-02-21
 ***/

#ifndef BFLOG_H
#define BFLOG_H

#include "BFLogBase.h"
#include "BFSystem.h"

#define BF_DEFAULT_OVERWRITE       true
#define BF_LOGFILE_NAME         _T("bf.log")
#define BF_LOGFILE_BAKSUFFIX    _T(".bak")
#define BF_LOGFILE_MAXSIZE      150000



/// it is just log
class BFLog : public BFLogBase
{
    private:
        /// log a message
        void Do(BFMessageType type,
                const wxDateTime& timestamp,
                const wxChar* strMessage,
                const wxChar* strLocation);

    public:
        ///
        BFLog ();

        /// virtual destructor
        virtual ~BFLog ();

        ///
        virtual void ValueChanged (Subject* pSender);
};    // class BFLog

#endif    // BFLOG_H
