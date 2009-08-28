/**
 * Name:        BFCronCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-06-30
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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


#ifndef BFCRONCTRL_H
#define BFCRONCTRL_H

#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>

class BFTimeCtrl;

///
class BFCronCtrl : public wxPanel
{
    private:
        ///
        wxArrayString       arrCrontabline_;
        /// daily, weekly, intervall
        wxComboBox*         pComboType_;
		///
		BFTimeCtrl*			pTimeCtrl_;

        /*
        wxComboBox*         pComboIntervall_;
        
        wxComboBox*         pComboFixed_;
        ///
        wxRadioButton*      pRadioIntervall_;
        ///
        wxRadioButton*      pRadioFixed_;

        ///
        void CheckRadios ();*/
        ///
        //void FillCombos ();

        ///
        void GetData();
        ///
        void SetData ();

    public:
        /// ctor
        BFCronCtrl (wxWindow* pParent,
                    const wxArrayString& arrCrontabline);

        /// virtual dtor
        virtual ~BFCronCtrl ();

        ///
        bool static IsCrontablineUsable (const wxArrayString& arr);

        //        void OnRadioButton (wxCommandEvent& rEvent);
        ///
        void OnCombo (wxCommandEvent& rEvent);

        ///
        wxString GetCrontabline ();

    DECLARE_EVENT_TABLE()
};

#endif
