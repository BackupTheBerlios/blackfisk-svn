/**
 * OBLog.h
 * definition of class OBLog
 * <comment>
 * 2007-02-21
 ***/

#ifndef OBLOG_H
#define OBLOG_H

#include <wx/file.h>

#include "ObserverPattern.h"
#include "OBSystem.h"

#define OB_DEFAULT_OVERWRITE       true
#define OB_LOGFILE_NAME         _T("ob.log")
#define OB_LOGFILE_BAKNAME      _T("ob.log.bak")
#define OB_LOGFILE_MAXSIZE      150000


/// it is just log
class OBLog : public Observer
{
    private:
        /// direct interface to the log file
        wxFile      fileLog_;

        /** take care that size of the logfile is not to big */
        void CareSize ();

    public:
        /// the ctor set its 'subject' by itself
        OBLog ();

        /// virtual destructor
        virtual ~OBLog ();

        ///
        virtual void ValueChanged (Subject* pSender);

        /// log a message
        void Do(OBMessageType type,
                const wxDateTime& timestamp,
                const wxChar* strMessage,
                const wxChar* strLocation);


};    // class OBLog

#endif    // OBLOG_H
