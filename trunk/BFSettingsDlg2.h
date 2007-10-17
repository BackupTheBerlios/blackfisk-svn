/**
 * Name:        BFSettingsDlg2.h
 * Purpose:     BFSettingsDlg2 class definition
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

#ifndef BFSETTINGSDLG2_H
#define BFSETTINGSDLG2_H

#include <wx/dialog.h>

class wxCheckBox;
class wxSpinCtrl;
class wxListbook;
class wxComboBox;

#define BFSETTINGSDLG_ID_BUTTONOK        1 + wxID_HIGHEST
#define BFSETTINGSDLG_ID_BUTTONCANCEL    2 + wxID_HIGHEST

///
class BFSettingsDlg2 : public wxDialog
{
    private:

    public:
        /// constructor
        BFSettingsDlg2 (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFSettingsDlg2 ();

        ///
        void OnClose (wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};    // class BFSettingsDlg2

#endif    // BFSETTINGSDLG_H
