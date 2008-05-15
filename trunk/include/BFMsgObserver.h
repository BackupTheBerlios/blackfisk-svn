/**
 * Name:        BFMsgObserver.h
 * Purpose:     BFMsgObserver class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-29
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

#ifndef BFMSGOBSERVER_H
#define BFMSGOBSERVER_H

#include "ObserverPattern.h"
#include "BFMessageDlg.h"
#include "BFSystemBase.h"

/** this class handle messages that should be displayed
    in a message box or the user should be asked about it */
class BFMsgObserver : public Observer
{
    private:
        ///
        static BFMessageDlg_Type GetMsgStyle (BFMessageType type);

    protected:
        /// proteced members

    public:
        /// constructor
        BFMsgObserver ();

        /// virtual destructor
        virtual ~BFMsgObserver ();

        ///
        virtual void ValueChanged (Subject* pSender);
};    // class BFMsgObserver

#endif    // BFMSGOBSERVER_H
