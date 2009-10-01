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
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>

class BFTimeCtrl;

///
class BFCronCtrl : public wxPanel
{
    private:
		///
		wxString			strCrontabline_;
        ///
        wxArrayString       arrCrontabline_;

		///
        wxRadioButton*		pRadioDaily_;
		///
		wxRadioButton*		pRadioWeekly_;
		///
		wxRadioButton*		pRadioIntervall_;

		///
		wxPanel*			pPanelDaily_;
		///
		wxPanel*			pPanelWeekly_;
		///
		wxPanel*			pPanelIntervall_;

		/// DAILY: clock
		BFTimeCtrl*			pDTimeCtrl_;

		/// WEEKLY: clock
		BFTimeCtrl*			pWTimeCtrl_;
		/// WEEKLY: weekdays
		wxComboBox*			pWComboDay_;

		/// INTERVALL: spin
		wxSpinCtrl*			pISpin_;
		/// INTERVALL: type
		wxComboBox*			pICombo_;

		/** INTERVALL: hours
		wxSpinCtrl*			pISpinHours_;
		/// INTERVALL: minutes
		wxSpinCtrl*			pISpinMinutes_;*/

        /** Read data from the crontab and display it in the dialog. */
        void GetData();
		///
		void GetData_Daily();
		///
		void GetData_Weekly();
		///
		void GetData_Intervall();

		///
		bool IsCrontablineDaily ();
		///
		bool IsCrontablineWeekly ();
		///
		bool IsCrontablineIntervall ();

    public:
        /** Read data from the dialog and create a crontab of it. */
        void SetData ();

		/// ctor
        BFCronCtrl (wxWindow* pParent,
                    const wxString& strCrontabline);

        /// virtual dtor
        virtual ~BFCronCtrl ();

        ///
        void OnRadio_D (wxCommandEvent& rEvent);
        ///
        void OnRadio_W (wxCommandEvent& rEvent);
        ///
        void OnRadio_I (wxCommandEvent& rEvent);

		/**
		void OnCombo_I (wxCommandEvent& rEvent);
		///
		void OnCombo_W (wxCommandEvent& rEvent);*/

        ///
        wxString GetCrontabline ();

    DECLARE_EVENT_TABLE()
};

#endif
