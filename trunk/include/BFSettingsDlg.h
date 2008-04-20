/**
 * Name:        BFSettingsDlg.h
 * Purpose:     BFSettingsDlg class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-23
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFSETTINGSDLG_H
#define BFSETTINGSDLG_H

#include <wx/dialog.h>

class wxCheckBox;
class wxSpinCtrl;
class wxListbook;
class wxComboBox;
class wxTreebook;
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
        wxCheckBox*             pCheckSwitchTrees_;
        ///
        wxCheckBox*             pCheckHiddenFiles_;
        ///
        wxSpinCtrl*             pSpinLogSize_;
        ///
        BFProjectSettingsCtrl*  pPrjCtrl_;
        ///
        wxComboBox*             pComboVerboseLog_;
        ///
        wxComboBox*             pComboLanguage_;

        /// read data from the settings data
        void GetData ();
        /// write data from the dialog to the settings data object
        void SetData ();

        ///
        wxWindow* CreatePage_General (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_View (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_Log (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_Project (wxTreebook* pBook);

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
