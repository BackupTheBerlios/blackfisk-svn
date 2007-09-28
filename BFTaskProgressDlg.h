/**
 * Name:        BFTaskProgressDlg.cpp
 * Purpose:     BFTaskProgressDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-09
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFTASKPROGRESSDLG_H
#define BFTASKPROGRESSDLG_H

#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/button.h>

#include "ObserverPattern.h"

// forwarde declarations
class BFRootTask;
class Progress;
class ProgressTotal;
class ProgressWithMessage;
class BFProgressTaskCtrl;
class BFProgressTotalCtrl;

///
class BFTaskProgressDlg : public wxDialog
{
    private:
        ///
        wxListBox*                  pListBox_;
        ///
        BFProgressTotalCtrl*        pCtrlTotalProgress_;
        ///
        BFProgressTaskCtrl*         pCtrlTaskProgress_;

        ///
        BFTaskProgressDlg();

        ///
        void Init ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFTaskProgressDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFTaskProgressDlg ();

        ///
        void SetCurrentTaskName (const wxChar* name);

        ///
        void OnClose (wxCloseEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFTaskProgressDlg

#endif    // BFTASKPROGRESSDLG_H
