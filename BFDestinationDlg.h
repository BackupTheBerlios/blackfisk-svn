/**
 * BFDestinationDlg.h
 * definition of class BFDestinationDlg
 * <comment>
 * 2007-03-28
 ***/

#ifndef BFDESTINATIONDLG_H
#define BFDESTINATIONDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>

#define BFDESTINATIONDLG_ID_BUTTONOK        1 + wxID_HIGHEST
#define BFDESTINATIONDLG_ID_BUTTONCANCEL    2 + wxID_HIGHEST

class BFDestinationCtrl;

///
class BFDestinationDlg : public wxDialog
{
    private:
        ///
        BFDestinationCtrl*  pDestCtrl_;

        ///
        BFDestinationDlg ();

    public:
        /// constructor
        BFDestinationDlg (wxWindow* pParent, const wxChar* strPath = wxEmptyString);

        /// virtual destructor
        virtual ~BFDestinationDlg ();

        ///
        void OnClose (wxCloseEvent& event);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFDestinationDlg

#endif    // BFDESTINATIONDLG_H
