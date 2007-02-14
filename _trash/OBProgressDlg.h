/**
 * OBProgressDlg.h
 * definition of class OBProgressDlg
 * <comment>
 * 2006-04-17
 ***/

#ifndef OBPROGRESSDLG_H
#define OBPROGRESSDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include "ObserverPattern.h"

class Progress;
class wxStaticText;
class wxGauge;

///
class OBProgressDlg : public wxDialog, public Observer
{
    private:
        ///
        OBProgressDlg ();

        ///
        wxStaticText*       pStaticText_;
        ///
        wxGauge*            pProgressBar_;
        ///
        wxStaticText*       pProgressText_;

        ///
        void SetBarValue (Progress* pP);

    protected:
        /// proteced members

    public:
        /// constructor
        OBProgressDlg (Progress* pProgress, wxWindow* pParent, const wxChar* title, const wxChar* message);

        /// virtual destructor
        virtual ~OBProgressDlg ();

        /// if the 'subject' do a broadcast
        virtual void ValueChanged (Subject* pSender);

};    // class OBProgressDlg

#endif    // OBPROGRESSDLG_H
