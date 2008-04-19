/**
 * Name:        BFwxLog.h
 * Purpose:     BFwxLog class declaration
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-15
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    public:
        /// constructor
        BFwxLog ();

        /// virtual destructor
        virtual ~BFwxLog ();

};    // class BFwxLog

#endif    // BFWXLOG_H
