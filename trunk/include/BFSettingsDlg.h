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
class wxStaticText;
class BFProjectSettingsCtrl;

///
class BFSettingsDlg : public wxDialog
{
    private:
        // --- GENERALE PAGE ---
        ///
        wxCheckBox*             pCheckOpenLast_;
        ///
        wxComboBox*             pComboLanguage_;
        ///
        wxCheckBox*             pCheckNewVersion_;
        ///
        wxSpinCtrl*             pSpinDaysNewVersion_;
        ///
        wxStaticText*           pLabelDays1_;
        ///
        wxStaticText*           pLabelDays2_;

        // --- VIEW PAGE ---
        ///
        wxCheckBox*             pCheckMacro_;
        ///
        wxCheckBox*             pCheckFiles_;
        ///
        wxCheckBox*             pCheckSwitchTrees_;
        ///
        wxCheckBox*             pCheckHiddenFiles_;

        // --- LOG PAGE ---
        ///
        wxSpinCtrl*             pSpinLogSize_;
        ///
        wxComboBox*             pComboVerboseLog_;

        // --- DEFAULT PROJECT PAGE ---
        ///
        BFProjectSettingsCtrl*  pPrjCtrl_;

        /// read data from the settings data
        void GetData ();
        /// write data from the dialog to the settings data object
        void SetData ();

        ///
        void AddHead (wxSizer* pSizer, wxWindow* pParent, const wxString& strHeadline);
        ///
        wxWindow* CreatePage_General (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_View (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_Log (wxTreebook* pBook);
        ///
        wxWindow* CreatePage_Project (wxTreebook* pBook);

        /** enable or disable 'pSpinDaysNewVersion_' related
            on the state of 'pCheckNewVersion_' */
        void On_NewVersion ();

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

        ///
        void OnCheck_NewVersion (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFSettingsDlg

#endif    // BFSETTINGSDLG_H
