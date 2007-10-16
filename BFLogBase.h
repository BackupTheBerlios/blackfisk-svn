/**
 * Name:        BFLogBase.h
 * Purpose:     BFLogBase class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-21
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

#ifndef BFLOGBASE_H
#define BFLOGBASE_H

#include <wx/file.h>
#include "ObserverPattern.h"


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

        /// return the maximum size in bytes
        long GetMaxSize ();
        ///
        void SetMaxSize (long lMaxSizeInBytes);

        /// the ctor set its 'subject' by itself
        BFLogBase (const wxChar* strLogFileName, long lMaxSize = 0);

    public:
        /// virtual destructor
        virtual ~BFLogBase ();

};    // class BFLogBase

#endif    // BFLOGBASE_H
