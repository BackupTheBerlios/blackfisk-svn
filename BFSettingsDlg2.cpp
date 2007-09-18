/**
 * Name:        BFSettingsDlg2.cpp
 * Purpose:     BFSettingsDlg2 class implementation
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

#include "BFSettingsDlg2.h"

#include <wx/listbook.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/statline.h>
#include <wx/treebook.h>
#include <limits.h>

#include "BFSystem.h"
#include "BFProjectSettingsCtrl2.h"
#include "BFSettings.h"
#include "BFApp.h"
#include "BFHelpCtrl.h"

BEGIN_EVENT_TABLE(BFSettingsDlg2, wxDialog)
  EVT_CLOSE     (                               BFSettingsDlg2::OnClose)
END_EVENT_TABLE()

//
BFSettingsDlg2::BFSettingsDlg2 (wxWindow* pParent)
             : wxDialog(pParent, wxID_ANY, wxString(_("Global Settings")))
{
    wxString strTip;

    // the book
    wxTreebook* pBook = new wxTreebook(this,
                                       wxID_ANY,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxNB_LEFT);

    // *** PROJECT-page ***
    wxPanel* pPageProject = new wxPanel(pBook);
    wxBoxSizer* pPageProjectSizerTop = new wxBoxSizer(wxVERTICAL);
    pPrjCtrl_ = new BFProjectSettingsCtrl2(pPageProject);
    pPageProjectSizerTop->Add(pPrjCtrl_, wxSizerFlags(0).Expand());
    pPageProject->SetSizer(pPageProjectSizerTop);
    pBook->AddPage(pPageProject, _("Default Project"));

    // sizer and arrange
    wxBoxSizer* pDlgSizer = new wxBoxSizer(wxVERTICAL);
    pDlgSizer->Add(pBook, wxSizerFlags(0).Expand().Border());
    SetSizerAndFit(pDlgSizer);
    ShowModal();
}


//
/*virtual*/ BFSettingsDlg2::~BFSettingsDlg2 ()
{
}


void BFSettingsDlg2::OnClose (wxCloseEvent& event)
{
    Destroy();
}
