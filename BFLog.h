/**
 * Name:        BFLog.h
 * Purpose:     BFLog class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-21
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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

#ifndef BFLOG_H
#define BFLOG_H

#include "BFLogBase.h"
#include "BFSystem.h"

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
