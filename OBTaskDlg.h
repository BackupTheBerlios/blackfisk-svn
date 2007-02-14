/**
 * OBTaskDlg.h
 * definition of class OBTaskDlg
 * <comment>
 * 2006-06-05
 ***/

#ifndef OBTASKDLG_H
#define OBTASKDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include "OBTask.h"

#define OBTASKDLG_ID_BUTTONOK       1 + wxID_HIGHEST
#define OBTASKDLG_ID_BUTTONCANCEL   2 + wxID_HIGHEST

///
class OBTaskDlg : public wxDialog
{
    private:
        /// reference to the object holding the data
        OBTask&         rTask_;

        /// task name
        wxTextCtrl*     pNameCtrl_;
        /// task source
        wxTextCtrl*     pSourceCtrl_;
        /// task destination
        wxTextCtrl*     pDestCtrl_;
        /// verify
        wxCheckBox*     pVerifyCheck_;
        /// exclude
        wxListBox*      pExcludeList_;

        /** get the data from the data layer ('rTask')
            in the dialog controls */
        void GetData ();
        /** set/store the data from the controls to
            the data layer ('rTask') if they are modified */
        void SetData ();
        /** check if the data in the dialog is plausible */
        bool IsPlausible ();

    protected:
        /// proteced members

    public:
        /// constructor
        OBTaskDlg (wxWindow* pParent, OBTask& rTask);
        /// virtual destructor
        virtual ~OBTaskDlg ();

        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class OBTaskDlg

#endif    // OBTASKDLG_H
