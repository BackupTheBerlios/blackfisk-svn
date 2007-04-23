/**
 * BFProjectSettingsCtrl.h
 * definition of class BFProjectSettingsCtrl
 * <comment>
 * 2007-04-20
 ***/

#ifndef BFPROJECTSETTINGSCTRL_H
#define BFPROJECTSETTINGSCTRL_H

#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/filepicker.h>
#include <wx/radiobut.h>
#include "BFProjectSettings.h"

///
class BFProjectSettingsCtrl : public wxPanel
{
    private:
        ///
        wxComboBox*         pComboVerbose_;
        ///
        BF_StopLevel        iStopLevelOnFatal_;
        ///
        BF_StopLevel        iStopLevelOnError_;
        ///
        BF_StopLevel        iStopLevelOnWarning_;
        ///
        wxDirPickerCtrl*    pPickerBackupLog_;

        ///
        wxRadioButton*      pRadioWarningPrj_;
        ///
        wxRadioButton*      pRadioWarningTsk_;
        ///
        wxRadioButton*      pRadioWarningAsk_;
        ///
        wxRadioButton*      pRadioWarningIgn_;
        ///
        wxRadioButton*      pRadioErrorPrj_;
        ///
        wxRadioButton*      pRadioErrorTsk_;
        ///
        wxRadioButton*      pRadioErrorAsk_;
        ///
        wxRadioButton*      pRadioErrorIgn_;
        ///
        wxRadioButton*      pRadioFatalPrj_;
        ///
        wxRadioButton*      pRadioFatalTsk_;
        ///
        wxRadioButton*      pRadioFatalAsk_;
        ///
        wxRadioButton*      pRadioFatalIgn_;

        ///
        int                 iLabelWidth_;

        ///
        wxSizer* CreateStopLevelCtrl ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFProjectSettingsCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFProjectSettingsCtrl ();

        ///
        void OnRadio (wxCommandEvent& event);

        ///
        int GetLabelWidth ();

        ///
        void GetData (BFProjectSettings& rSettings);
        ///
        void SetData (BFProjectSettings& rSettings);
    DECLARE_EVENT_TABLE();
};    // class BFProjectSettingsCtrl

#endif    // BFPROJECTSETTINGSCTRL_H
