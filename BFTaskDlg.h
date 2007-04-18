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
#include <wx/filepicker.h>
#include "BFTask.h"

#define BFTASKDLG_ID_BUTTONOK       1 + wxID_HIGHEST
#define BFTASKDLG_ID_BUTTONCANCEL   2 + wxID_HIGHEST

class wxSizer;
class BFDestinationCtrl;

/// the base class of all task dialogs
class BFTaskBaseDlg : public wxDialog
{
    protected:
        /** reference to the object holding the data */
        BFTask&             rTask_;

        /// task name
        wxTextCtrl*         pNameCtrl_;
        /// task source
        wxTextCtrl*         pSourceCtrl_;
        /// task destination
        BFDestinationCtrl*  pDestCtrl_;
        /// verify
        wxCheckBox*         pVerifyCheck_;

        /** get the data from the data layer ('rTask')
            in the dialog controls */
        void GetData ();
        /** set/store the data from the controls to
            the data layer ('rTask') if they are modified */
        void SetData ();
        /** check if the data in the dialog is plausible */
        bool IsPlausible ();

        /// constructor
        BFTaskBaseDlg (wxWindow* pParent, const wxString& strTitle, BFTask& rTask);

        /** init the task type with icon and its description
            and return its wxSizer */
        virtual wxSizer* InitType ();
        /** init the buttons in the dialog*/
        virtual wxSizer* InitButtons ();
        ///
        void Arrange(wxSizer* pBody, wxSizer* pBodyExtra = NULL);

    public:
        /// width of the label column
        static const long lWidth1_;
        /// width of the editable column
        static const long lWidth2_;

        /// virtual destructor
        virtual ~BFTaskBaseDlg ();

        /** set the size of the specified dialog row */
        static void SetRowSize (wxWindow* pCol1, wxWindow* pCol2);

        ///
        void OnClose (wxCloseEvent& rEvent);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

        /** this static methode display the given task
            with a dialog specified by type of the task */
        static void Show (BFTask* pTask);

    DECLARE_EVENT_TABLE();
};


/// dialog for file copy tasks
class BFTaskFileCopyDlg : public BFTaskBaseDlg
{
    protected:

    public:
        /// constructor
        BFTaskFileCopyDlg (wxWindow* pParent, BFTask& rTask);
        /// virtual destructor
        virtual ~BFTaskFileCopyDlg ();

};


/// dialog for directory copy tasks
class BFTaskDirectoryCopyDlg : public BFTaskBaseDlg
{
    protected:
        /// exclude control
        wxListBox*          pExcludeList_;

    public:
        /// constructor
        BFTaskDirectoryCopyDlg (wxWindow* pParent, BFTask& rTask);
        /// virtual destructor
        virtual ~BFTaskDirectoryCopyDlg ();
};

/*
/// dialog for directory synchronization
class BFTaskSynchronizeDlg : public BFTaskBaseDlg
{
    protected:
        /// exclude control
        wxListBox*          pExcludeList_;

    public:
        /// ctor
        BFTaskSynchronizeDlg (wxWindow* pParent, BFTask& rTask);
        /// virtual dtor
        virtual ~BFTaskSynchronizeDlg ();
};
*/

#endif    // BFTASKDLG_H
