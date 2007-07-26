/**
 * Name:        BFMacroButton.cpp
 * Purpose:     BFMacroButton class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-18
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

#include "BFMacroButton.h"

#include "ctrlids.h"
#include "BFSystem.h"
#include "BFTaskBase.h"

BEGIN_EVENT_TABLE(BFMacroButton, wxButton)
    EVT_BUTTON  (BFMACROBUTTON_ID_PRESSED,      BFMacroButton::OnPressed)
    EVT_MENU    (BFMACROBUTTON_ID_MACRO_DATE,   BFMacroButton::OnMacro)
END_EVENT_TABLE()

//
BFMacroButton::BFMacroButton (wxWindow* pParent, wxTextCtrl& rTextCtrl)
             : wxButton (pParent,
                         BFMACROBUTTON_ID_PRESSED,
                         _("macro"),
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxNO_BORDER | wxBU_EXACTFIT),
               rTextCtrl_(rTextCtrl)
{
}


//
/*virtual*/ BFMacroButton::~BFMacroButton ()
{
}

void BFMacroButton::OnPressed(wxCommandEvent& rEvent)
{
    wxMenu menu;
    menu.Append(BFMACROBUTTON_ID_MACRO_DATE, BFTASK_MACRO_DATE);
    PopupMenu(&menu);
}

void BFMacroButton::OnMacro(wxCommandEvent& rEvent)
{
    rTextCtrl_.WriteText(BFTASK_MACRO_DATE);
    rTextCtrl_.SetFocus();
}
