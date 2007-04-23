/**
 * BFProjectSettingsDlg.h
 * definition of class BFProjectSettingsDlg
 * <comment>
 * 2007-04-20
 ***/

#ifndef BFPROJECTSETTINGSDLG_H
#define BFPROJECTSETTINGSDLG_H

#include <wx/dialog.h>
#include <wx/textctrl.h>

#define BFPRJSETDLG_ID_BUTTONOK        1 + wxID_HIGHEST
#define BFPRJSETDLG_ID_BUTTONCANCEL    2 + wxID_HIGHEST

class BFProjectSettingsCtrl;

///
class BFProjectSettingsDlg : public wxDialog
{
    private:
        /// project name
        wxTextCtrl*             pNameCtrl_;
        /// other project settings
        BFProjectSettingsCtrl*  pCtrl_;

        /// read data from the data object (current rootTask)
        void GetData ();
        /// write data from the dialog to the data object (current rootTask)
        void SetData ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFProjectSettingsDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFProjectSettingsDlg ();

        ///
        void OnClose (wxCloseEvent& event);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFProjectSettingsDlg

#endif    // BFPROJECTSETTINGSDLG_H
