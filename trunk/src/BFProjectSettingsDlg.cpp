/**
 * Name:        BFProjectSettingsDlg.cpp
 * Purpose:     BFProjectSettingsDlg class implementation
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

#include "BFProjectSettingsDlg.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>

#include "BFProjectSettingsCtrl.h"
#include "BFProject.h"
#include "BFHelpCtrl.h"
#include "BFBitmapButton.h"
#include "BFIconTable.h"
#include "BFApp.h"
#include "BFSettings.h"

BEGIN_EVENT_TABLE(BFProjectSettingsDlg, wxDialog)
  EVT_CLOSE     (                               BFProjectSettingsDlg::OnClose)
  EVT_BUTTON    (BFPRJSETDLG_ID_BUTTONOK,       BFProjectSettingsDlg::OnButton_Ok)
  EVT_BUTTON    (BFPRJSETDLG_ID_BUTTONCANCEL,   BFProjectSettingsDlg::OnButton_Cancel)
END_EVENT_TABLE()


//
BFProjectSettingsDlg::BFProjectSettingsDlg (wxWindow* pParent)
                    : wxDialog(pParent,
                               wxID_ANY,
                               _("Project settings"))
{
    wxString strTip;

    // help ctrl
    pHelp_ = new BFHelpCtrl(this);

    // project-settings ctrl
    pCtrl_ = new BFProjectSettingsCtrl(this, pHelp_);

    // name
    wxStaticText* pNameLabel = new wxStaticText(this, wxID_ANY, _("Name:"));
    //pNameLabel->SetMinSize(wxSize(pCtrl_->GetLabelWidth(), pNameLabel->GetSize().GetHeight()));
    pNameCtrl_ = new wxTextCtrl(this, wxID_ANY);
    strTip = _("name of the backup project");
    pNameLabel->SetHelpText(strTip);
    pNameCtrl_->SetHelpText(strTip);

    // button
    BFBitmapButton* pButtonOk     = new BFBitmapButton(this,
                                                       BFPRJSETDLG_ID_BUTTONOK,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::ok),
                                                       "OK");
    BFBitmapButton* pButtonCancel = new BFBitmapButton(this,
                                                       BFPRJSETDLG_ID_BUTTONCANCEL,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::cancel),
                                                       _("Cancel"));

    // sizer
    wxBoxSizer*         pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer*         pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer*         pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pNameSizer->Add(pNameLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT));
    pNameSizer->Add(pNameCtrl_, wxSizerFlags(1));
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    pTopSizer->Add(pNameSizer,              wxSizerFlags(0).Border(wxLEFT | wxUP | wxRIGHT).Expand());
    pTopSizer->Add(new wxStaticLine(this),  wxSizerFlags(0).Border().Expand());
    pTopSizer->Add(pCtrl_,                  wxSizerFlags(0).Border());
    pTopSizer->Add(pHelp_,                  wxSizerFlags(0).Border().Expand());
    pTopSizer->Add(pButtonSizer,            wxSizerFlags(0).Center());

    // motion events
    pHelp_->Connect(pNameLabel);
    pHelp_->Connect(pNameCtrl_);

    // arrange and show
    SetSizerAndFit(pTopSizer);
    GetData();
    CenterOnParent();
    ShowModal();
}


//
/*virtual*/ BFProjectSettingsDlg::~BFProjectSettingsDlg ()
{
}


void BFProjectSettingsDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}

void BFProjectSettingsDlg::OnButton_Ok (wxCommandEvent& rEvent)
{
    SetData();

	if ( BFProject::Instance().IsModified() && BFSettings::Instance().GetAutosaveProjects() )
	{
		BFSystem::Log(_("Autosave current project..."));
		wxGetApp().SaveCurrentProject();
	}

    Close();
}

void BFProjectSettingsDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}

void BFProjectSettingsDlg::GetData ()
{
    BFProject& rPrj = BFProject::Instance();

    pNameCtrl_->SetValue(rPrj.GetName());
    pCtrl_->GetData(rPrj.GetSettings());
}

void BFProjectSettingsDlg::SetData ()
{
    BFProject& rPrj = BFProject::Instance();
    bool bBroadcast = false;

    if (rPrj.GetName() != pNameCtrl_->GetValue())
    {
        rPrj.SetName(pNameCtrl_->GetValue());
        BFProject::Instance().SetModified();
        bBroadcast = true;
    }

    pCtrl_->SetData(rPrj.GetSettings());

    if (bBroadcast)
        rPrj.broadcastObservers();
}
