/**
 * Name:        BFwxLog.h
 * Purpose:     BFwxLog class declaration
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-15
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***/


#ifndef BFWXLOG_H
#define BFWXLOG_H

#include <wx/log.h>

///
class BFwxLog : public wxLog
{
    private:
        /// private members

    protected:
        ///
        virtual void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp);
        ///
        //virtual void DoLogString(const wxChar *szString, time_t t);

    public:
        /// constructor
        BFwxLog ();

        /// virtual destructor
        virtual ~BFwxLog ();

};    // class BFwxLog

#endif    // BFWXLOG_H
