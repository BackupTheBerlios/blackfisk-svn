/**
 * OBMessageDlg.h
 * definition of class OBMessageDlg
 * <comment>
 * 2007-02-23
 ***/

#ifndef OBMESSAGEDLG_H
#define OBMESSAGEDLG_H

class OBMainFrame;

#include <wx/msgdlg.h>

#include "ObserverPattern.h"

///
class OBMessageDlg : public wxMessageDialog, public Observer
{
    private:
        ///
        void SetMessage (const wxChar* msg);
        ///
        void SetCaption (const wxChar* cap);
        ///
        void SetStyle (long style);

        /// private ctor
        OBMessageDlg ();

    protected:
        /// proteced members

    public:
        /// constructor
        OBMessageDlg (OBMainFrame* pParent);

        /// virtual destructor
        virtual ~OBMessageDlg ();

        virtual void ValueChanged (Subject* pSender);
};    // class OBMessageDlg

#endif    // OBMESSAGEDLG_H
