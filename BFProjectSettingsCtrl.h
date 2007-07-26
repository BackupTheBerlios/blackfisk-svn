/**
 * Name:        BFProjectSettingsCtrl.h
 * Purpose:     BFProjectSettingsCtrl class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
