/**
 * Name:        BFProcessMsgDlg.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-28
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


#ifndef BFPROCESSMSGDLG_H
#define BFPROCESSMSGDLG_H

#include <wx/dialog.h>
#include <wx/stattext.h>
#include "ObserverPattern.h"

class BFProcessMsgSubject;

/** just a little window that display messages while any process (check, load, etc)
    the window observs BFProcessMsgSubject for new messages
    the window need to be destroyed explicite (e.g. instance on the stack) */
class BFProcessMsgDlg : public Observer, public wxDialog
{
    private:
        ///
        wxStaticText*   pMsg_;

    public:
        ///
        BFProcessMsgDlg (wxWindow* pParent,
                         BFProcessMsgSubject* pSubject,
                         int iLines,
                         int iRows);

        /// virtual dtor
        virtual ~BFProcessMsgDlg ();

        ///
		virtual void ValueChanged (Subject* pSender);
};

#endif    // BFPROCESSMSGDLG_H
