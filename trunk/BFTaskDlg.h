/**
 * Name:        BFTaskDlg.h
 * Purpose:     BFTaskDlg class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-06-05
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

#ifndef BFTASKDLG_H
#define BFTASKDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/bmpcbox.h>
#include <vector>
#include "BFTask.h"

#define BFTASKDLG_ID_BUTTONOK       1 + wxID_HIGHEST
#define BFTASKDLG_ID_BUTTONCANCEL   2 + wxID_HIGHEST
#define BFTASKDGL_ID_CBTYPE         3 + wxID_HIGHEST

class wxBookCtrlBase;
class wxSizer;
class BFDestinationCtrl;
class BFHelpCtrl;
class BFExcludeCtrl;

/// task dialog
class BFTaskDlg : public wxDialog
{
    private:
        /** reference to the object holding the data */
        BFTask*             pTask_;

        /// help control
        BFHelpCtrl*         pHelpCtrl_;
        /// type control
        wxBitmapComboBox*   pTypeCtrl_;
        /// task name
        wxTextCtrl*         pNameCtrl_;
        /// task source
        wxTextCtrl*         pSourceCtrl_;
        /// task destination
        BFDestinationCtrl*  pDestCtrl_;
        /// verify general
        wxCheckBox*         pVerifyCheck_;
        /// verify inclusive content
        wxCheckBox*         pVerifyContentCheck_;
        ///
        BFExcludeCtrl*      pExcludeCtrl_;

        /** init the type control */
        void InitTypeCtrl ();
        /** get the data from the data layer ('rTask')
            in the dialog controls */
        void GetData ();
        /** set/store the data from the controls to
            the data layer ('rTask') if they are modified */
        void SetData ();
        /** check if the data in the dialog is plausible */
        bool IsPlausible ();

        ///
        wxBookCtrlBase* CreateBook ();
        ///
        wxWindow* CreateBookPageA (wxWindow* pParent);
        ///
        wxWindow* CreateBookPageB (wxWindow* pParent);
        /** init the buttons in the dialog*/
        virtual wxSizer* CreateButtons ();

    public:
        /// width of the label column
        static const long lWidth1_;
        /// width of the editable column
        static const long lWidth2_;

        /// constructor
        BFTaskDlg (wxWindow* pParent, BFTask* pTask);
        /// virtual destructor
        virtual ~BFTaskDlg ();

        /** set the size of the specified dialog row */
        static void SetRowSize (wxWindow* pCol1, wxWindow* pCol2);

        ///
        void OnClose (wxCloseEvent& rEvent);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);
        ///
        void OnCombo_TypeSelect (wxCommandEvent& rEvent);
        ///
        void OnCheckBox_Verify (wxCommandEvent& rEvent);

        /** this static methode display the given task
            with a dialog specified by type of the task */
        static void Show (BFTask* pTask);

    DECLARE_EVENT_TABLE();
};

#endif    // BFTASKDLG_H
