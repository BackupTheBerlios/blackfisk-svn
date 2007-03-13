/**
 * BFTaskDlg.h
 * definition of class BFTaskDlg
 * <comment>
 * 2006-06-05
 ***/

#ifndef BFTASKDLG_H
#define BFTASKDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include "BFTask.h"

#define BFTASKDLG_ID_BUTTONOK       1 + wxID_HIGHEST
#define BFTASKDLG_ID_BUTTONCANCEL   2 + wxID_HIGHEST

///
class BFTaskDlg : public wxDialog
{
    private:
        /// reference to the object holding the data
        BFTask&         rTask_;

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
        BFTaskDlg (wxWindow* pParent, BFTask& rTask);
        /// virtual destructor
        virtual ~BFTaskDlg ();

        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFTaskDlg

#endif    // BFTASKDLG_H
