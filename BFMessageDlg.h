/**
 * BFMessageDlg.h
 * definition of class BFMessageDlg
 * <comment>
 * 2007-02-23
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

    protected:
        /// proteced members

    public:
        /// constructor
        BFMessageDlg (BFMainFrame* pParent);

        /// virtual destructor
        virtual ~BFMessageDlg ();

        virtual void ValueChanged (Subject* pSender);
};    // class BFMessageDlg

#endif    // BFMESSAGEDLG_H
