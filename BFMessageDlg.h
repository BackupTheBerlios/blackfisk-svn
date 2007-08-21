/**
 * Name:        BFMessageDlg.h
 * Purpose:     BFMessageDlg class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-23
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

#ifndef BFMESSAGEDLG_H
#define BFMESSAGEDLG_H

class BFMainFrame;

#include <wx/msgdlg.h>

#include "ObserverPattern.h"

///
class BFMessageDlg : public wxMessageDialog, public Observer
{
    private:
        ///
        void SetMessage (const wxChar* msg);
        ///
        void SetCaption (const wxChar* cap);
        ///
        void SetStyle (long style);

        /// private ctor
        BFMessageDlg ();

        /// standard behavior
        void BehaviorDefault ();
        /// behavior while a backup runs
        void BehaviorWhileBackup ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFMessageDlg (BFMainFrame* pParent);

        /// virtual destructor
        virtual ~BFMessageDlg ();

        ///
        virtual void ValueChanged (Subject* pSender);
};    // class BFMessageDlg

#endif    // BFMESSAGEDLG_H
