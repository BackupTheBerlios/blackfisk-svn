/**
 * OBTaskProgressDlg.h
 * definition of class OBTaskProgressDlg
 * <comment>
 * 2006-05-09
 ***/

#ifndef OBTASKPROGRESSDLG_H
#define OBTASKPROGRESSDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/control.h>
#include <wx/panel.h>
#include "ObserverPattern.h"

// forwarde declarations
class OBRootTask;
class Progress;
class ProgressTotal;
class ProgressWithMessage;

///
class OBProgressCtrlBase : public wxPanel, Observer
{
    private:
        ///
        OBProgressCtrlBase();

    protected:
        ///
        wxGauge*        pBar_;
        ///
        wxStaticText*   pTextA_;
        /// by default it is used for the progress-value in %
        wxStaticText*   pTextB_;

        /// ctor
        OBProgressCtrlBase (wxWindow* pParent, Progress* pProgress);
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
        virtual ~OBProgressCtrlBase ();

        ///
		virtual void ValueChanged (Subject* pSender);

		///
		void SetTextA (const wxChar* text);

};  // class OBProgressCtrlBase


/// displayes the Total-Progress of complete backup (OBRootTask)
class OBProgressTotalCtrl : public OBProgressCtrlBase
{
    private:
        ///
        OBProgressTotalCtrl ();

    protected:
        ///
        virtual void InitControls ();

    public:
        ///
        OBProgressTotalCtrl (wxWindow* pParent, Progress* pProgress);
        ///
        virtual ~OBProgressTotalCtrl ();
};  // class OBProgressTotalCtrl


/// displays the Progress of current OBTask
class OBProgressTaskCtrl : public OBProgressCtrlBase
{
    private:
        ///
        OBProgressTaskCtrl ();

    protected:
        ///
        virtual void InitControls ();

        ///
        virtual void UpdateText (Progress* pP);

    public:
        ///
        OBProgressTaskCtrl (wxWindow* pParent, Progress* pProgress);
        ///
        virtual ~OBProgressTaskCtrl ();
};  // class OBProgressTaskCtrl


///
class OBTaskProgressDlg : public wxDialog
{
    private:
        ///
        wxListBox*                  pListBox_;
        ///
        OBProgressTotalCtrl*        pCtrlTotalProgress_;
        ///
        ProgressTotal*              pProgressTotal_;
        ///
        OBProgressTaskCtrl*         pCtrlTaskProgress_;
        ///
        ProgressWithMessage*        pProgressTask_;

        ///
        OBTaskProgressDlg();

        ///
        void Init (OBRootTask& rRootTask);

    protected:
        /// proteced members

    public:
        /// constructor
        OBTaskProgressDlg (wxWindow* pParent, OBRootTask& rRootTask);

        /// virtual destructor
        virtual ~OBTaskProgressDlg ();

        ///
        Progress* GetProgressTotal ();
        ///
        ProgressWithMessage* GetProgressTask ();

        ///
        void SetCurrentTaskName (const wxChar* name);
};    // class OBTaskProgressDlg

#endif    // OBTASKPROGRESSDLG_H
