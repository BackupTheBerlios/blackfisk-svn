/**
 * Name:        BFProjectSettingsDlg.h
 * Purpose:     BFProjectSettingsDlg class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
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
