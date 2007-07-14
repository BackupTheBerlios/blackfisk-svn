/**
 * BFSettingsDlg.h
 * definition of class BFSettingsDlg
 * <comment>
 * 2007-04-23
 ***/

#ifndef BFSETTINGSDLG_H
#define BFSETTINGSDLG_H

#include <wx/dialog.h>

class wxCheckBox;
class wxSpinCtrl;
class wxListbook;
class BFProjectSettingsCtrl;

#define BFSETTINGSDLG_ID_BUTTONOK        1 + wxID_HIGHEST
#define BFSETTINGSDLG_ID_BUTTONCANCEL    2 + wxID_HIGHEST

///
class BFSettingsDlg : public wxDialog
{
    private:
        ///
        wxCheckBox*             pCheckOpenLast_;
        ///
        wxCheckBox*             pCheckMacro_;
        ///
        wxCheckBox*             pCheckFiles_;
        ///
        wxSpinCtrl*             pSpinLogSize_;
        ///
        BFProjectSettingsCtrl*  pPrjCtrl_;

        /// read data from the settings data
        void GetData ();
        /// write data from the dialog to the settings data object
        void SetData ();

    public:
        /// constructor
        BFSettingsDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFSettingsDlg ();

        ///
        void OnClose (wxCloseEvent& event);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFSettingsDlg

#endif    // BFSETTINGSDLG_H
