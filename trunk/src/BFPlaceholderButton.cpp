/**
 * Name:        BFPlaceholderButton.cpp
 * Purpose:     BFPlaceholderButton class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-18
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

#include "BFPlaceholderButton.h"

#include "ids.h"
#include "BFSystem.h"
#include "BFTaskBase.h"

#define BFPLACEHOLDERBUTTON_ID_PRESSED              1 + BF_PLACEHOLDERBUTTON_ID_HIGHEST
#define BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_DATE     2 + BF_PLACEHOLDERBUTTON_ID_HIGHEST
#define BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_TIME     3 + BF_PLACEHOLDERBUTTON_ID_HIGHEST

BEGIN_EVENT_TABLE(BFPlaceholderButton, wxButton)
    EVT_BUTTON  (BFPLACEHOLDERBUTTON_ID_PRESSED,            BFPlaceholderButton::OnPressed)
    EVT_MENU    (BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_DATE,   BFPlaceholderButton::OnPlaceholder)
    EVT_MENU    (BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_TIME,   BFPlaceholderButton::OnPlaceholder)
END_EVENT_TABLE()

//
BFPlaceholderButton::BFPlaceholderButton (wxWindow* pParent, wxTextCtrl& rTextCtrl)
             : wxButton (pParent,
                         BFPLACEHOLDERBUTTON_ID_PRESSED,
                         _("placeholder"),
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxNO_BORDER | wxBU_EXACTFIT),
               rTextCtrl_(rTextCtrl)
{
}


//
/*virtual*/ BFPlaceholderButton::~BFPlaceholderButton ()
{
}

void BFPlaceholderButton::OnPressed(wxCommandEvent& rEvent)
{
    wxMenu menu;
    menu.Append(BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_DATE, BFTASK_PLACEHOLDER_DATE);
    menu.Append(BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_TIME, BFTASK_PLACEHOLDER_TIME);
    PopupMenu(&menu);
}

void BFPlaceholderButton::OnPlaceholder(wxCommandEvent& rEvent)
{
    switch (rEvent.GetId())
    {
        case BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_DATE:
            rTextCtrl_.WriteText(BFTASK_PLACEHOLDER_DATE);
            break;

        case BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_TIME:
            rTextCtrl_.WriteText(BFTASK_PLACEHOLDER_TIME);
            break;
    }

    rTextCtrl_.SetFocus();
}
