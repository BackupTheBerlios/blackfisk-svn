/**
 * Name:        BFTaskProgressDlg.cpp
 * Purpose:     BFTaskProgressDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-09
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/control.h>
#include <wx/panel.h>

#include "ObserverPattern.h"

// forwarde declarations
class BFRootTask;
class Progress;
class ProgressTotal;
class ProgressWithMessage;

///
class BFProgressCtrlBase : public wxPanel, Observer
{
    private:
        ///
        BFProgressCtrlBase();

    protected:
        ///
        wxGauge*        pBar_;
        ///
        wxStaticText*   pTextA_;
        /// by default it is used for the progress-value in %
        wxStaticText*   pTextB_;

        /// ctor
        BFProgressCtrlBase (wxWindow* pParent, Progress* pProgress);
        /** should be called by ctor
            the derived classes should decide how the
            three controls should be arranged */
        virtual void InitControls () = 0;

        ///
        virtual void UpdateBar (Progress* pP);
        ///
        virtual void UpdateText (Progress* pP);

    public:
        /// dtor
        virtual ~BFProgressCtrlBase ();

        ///
		virtual void ValueChanged (Subject* pSender);

		///
		void SetTextA (const wxChar* text);

};  // class BFProgressCtrlBase


/// displayes the Total-Progress of complete backup (BFRootTask)
class BFProgressTotalCtrl : public BFProgressCtrlBase
{
    private:
        ///
        BFProgressTotalCtrl ();

    protected:
        ///
        virtual void InitControls ();

    public:
        ///
        BFProgressTotalCtrl (wxWindow* pParent, Progress* pProgress);
        ///
        virtual ~BFProgressTotalCtrl ();
};  // class BFProgressTotalCtrl


/// displays the Progress of current BFTask
class BFProgressTaskCtrl : public BFProgressCtrlBase
{
    private:
        ///
        BFProgressTaskCtrl ();

    protected:
        ///
        virtual void InitControls ();

        ///
        virtual void UpdateText (Progress* pP);

    public:
        ///
        BFProgressTaskCtrl (wxWindow* pParent, Progress* pProgress);
        ///
        virtual ~BFProgressTaskCtrl ();
};  // class BFProgressTaskCtrl


///
class BFTaskProgressDlg : public wxDialog
{
    private:
        ///
        wxListBox*                  pListBox_;
        ///
        BFProgressTotalCtrl*        pCtrlTotalProgress_;
        ///
        ProgressTotal*              pProgressTotal_;
        ///
        BFProgressTaskCtrl*         pCtrlTaskProgress_;
        ///
        ProgressWithMessage*        pProgressTask_;

        ///
        BFTaskProgressDlg();

        ///
        void Init (BFRootTask& rRootTask);

    protected:
        /// proteced members

    public:
        /// constructor
        BFTaskProgressDlg (wxWindow* pParent, BFRootTask& rRootTask);

        /// virtual destructor
        virtual ~BFTaskProgressDlg ();

        ///
        Progress* GetProgressTotal ();
        ///
        ProgressWithMessage* GetProgressTask ();

        ///
        void SetCurrentTaskName (const wxChar* name);
};    // class BFTaskProgressDlg

#endif    // BFTASKPROGRESSDLG_H
