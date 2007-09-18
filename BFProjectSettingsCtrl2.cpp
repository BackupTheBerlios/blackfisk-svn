/**
 * Name:        BFProjectSettingsCtrl2.cpp
 * Purpose:     BFProjectSettingsCtrl2 class implementation
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

#include "BFProjectSettingsCtrl2.h"

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

BEGIN_EVENT_TABLE(BFProjectSettingsCtrl2, wxPanel)

END_EVENT_TABLE()


BFProjectSettingsCtrl2::BFProjectSettingsCtrl2 (wxWindow* pParent)
                     : wxPanel (pParent, wxID_ANY)
{
    // StopLevel
    wxPanel*        pStopCtrl = new wxPanel(this, wxID_ANY);
    wxStaticText*   pLabelA = new wxStaticText(pStopCtrl, wxID_ANY, _("ABCD"));

    // arrange
    wxStaticBoxSizer* pStopStaticSizer  = new wxStaticBoxSizer(wxVERTICAL, pStopCtrl);
    pStopStaticSizer->Add(pLabelA, wxSizerFlags(0).Expand());
    pStopCtrl->SetSizer(pStopStaticSizer);

    // sizer and arrange
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    pTopSizer->Add(pStopCtrl, wxSizerFlags(0).Expand());
    SetSizer(pTopSizer);
}


//
/*virtual*/ BFProjectSettingsCtrl2::~BFProjectSettingsCtrl2 ()
{
}
