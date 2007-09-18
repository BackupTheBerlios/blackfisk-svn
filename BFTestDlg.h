#ifndef BFTESTDLG_H
#define BFTESTDLG_H

#include <wx/dialog.h>


///
class BFTestDlg : public wxDialog
{
    private:

    public:
        /// constructor
        BFTestDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFTestDlg ();

        ///
        void OnClose (wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};

#endif    // BFTESTDLG_H
