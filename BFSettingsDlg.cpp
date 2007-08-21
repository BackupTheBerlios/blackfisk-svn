/**
 * Name:        BFSettingsDlg.cpp
 * Purpose:     BFSettingsDlg class implementation
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

#include "BFSettingsDlg.h"

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
#include "BFProjectSettingsCtrl.h"
#include "BFSettings.h"
#include "BFApp.h"

BEGIN_EVENT_TABLE(BFSettingsDlg, wxDialog)
  EVT_CLOSE     (                               BFSettingsDlg::OnClose)
  EVT_BUTTON    (BFSETTINGSDLG_ID_BUTTONOK,     BFSettingsDlg::OnButton_Ok)
  EVT_BUTTON    (BFSETTINGSDLG_ID_BUTTONCANCEL, BFSettingsDlg::OnButton_Cancel)
END_EVENT_TABLE()

//
BFSettingsDlg::BFSettingsDlg (wxWindow* pParent)
             : wxDialog(pParent, wxID_ANY, wxString(_("Global Settings")))
{
    // the book
    wxTreebook* pBook = new wxTreebook(this,
                                       wxID_ANY,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxNB_LEFT);
    pBook->SetMinSize(wxSize(440, 220));
    pBook->SetInternalBorder(15);



    // *** GENERAL-page ***
    wxPanel* pPageGeneral = new wxPanel(pBook);
    pCheckOpenLast_ = new wxCheckBox(pPageGeneral,
                                     wxID_ANY,
                                     _("open the last opened project on startup"));
    wxBoxSizer* pPageGeneralSizerTop = new wxBoxSizer(wxVERTICAL);
    pPageGeneralSizerTop->Add(pCheckOpenLast_);
    wxBoxSizer* pPageGeneralSizerVerbose = new wxBoxSizer(wxHORIZONTAL);

    pPageGeneral->SetSizer(pPageGeneralSizerTop);
    pBook->AddPage(pPageGeneral, _("General"));



    // *** VIEW-page ***
    wxPanel* pPageView = new wxPanel(pBook);
    pCheckMacro_ = new wxCheckBox(pPageView,
                                  wxID_ANY,
                                  _("view backup tree with activated macros"));
    pCheckFiles_ = new wxCheckBox(pPageView,
                                  wxID_ANY,
                                  _("view directory tree with files"));
    wxBoxSizer* pPageViewSizerTop = new wxBoxSizer(wxVERTICAL);
    pPageViewSizerTop->Add(pCheckMacro_);
    pPageViewSizerTop->Add(pCheckFiles_);
    pPageView->SetSizer(pPageViewSizerTop);
    pBook->AddPage(pPageView, _("View"));



    // *** LOG-page ***
    wxPanel* pPageLog           = new wxPanel(pBook);
    wxStaticText* pLabelLogSize = new wxStaticText(pPageLog, wxID_ANY, _("size of application log file (KB): "));
    pSpinLogSize_               = new wxSpinCtrl(pPageLog,
                                                 wxID_ANY,
                                                 wxEmptyString,
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 0,
                                                 0,
                                                 INT_MAX);
    wxBoxSizer* pPageLogSizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pPageLogSizerLog = new wxBoxSizer(wxHORIZONTAL);
    pPageLogSizerLog->Add(pLabelLogSize, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPageLogSizerLog->Add(pSpinLogSize_);
    pPageLogSizerTop->Add(pPageLogSizerLog);

    wxStaticText* pLabelVerboseLog = new wxStaticText(pPageLog, wxID_ANY, _("Verbose-Level for the application log: "));
    wxString arrVerbose[] = { _("backup"), _("information"), _("warning"), _("error"), _("fatal error"), _("debug") };
    pComboVerboseLog_ = new wxComboBox(pPageLog,
                                       wxID_ANY,
                                       arrVerbose[1],
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       6,
                                       arrVerbose,
                                       wxCB_READONLY);
    wxBoxSizer* pPageLogSizerVerbose = new wxBoxSizer(wxHORIZONTAL);
    pPageLogSizerVerbose->Add(pLabelVerboseLog, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPageLogSizerVerbose->Add(pComboVerboseLog_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));
    pPageLogSizerTop->Add(pPageLogSizerVerbose);
    pPageLog->SetSizer(pPageLogSizerTop);
    pBook->AddPage(pPageLog, _("Logging"));

    // *** PROJECT-page ***
    wxPanel* pPageProject = new wxPanel(pBook);
    wxBoxSizer* pPageProjectSizerTop = new wxBoxSizer(wxVERTICAL);
    wxStaticText* pLabelProject = new wxStaticText(pPageProject, wxID_ANY, _("Default Settings for new projects\nAttention: This doesn't affect the existing projects!"));
    pPrjCtrl_ = new BFProjectSettingsCtrl(pPageProject);
    pPageProjectSizerTop->Add(pLabelProject,                    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPageProjectSizerTop->Add(new wxStaticLine(pPageProject),   wxSizerFlags(0).Expand().Center().Border());
    pPageProjectSizerTop->Add(pPrjCtrl_);
    pPageProject->SetSizer(pPageProjectSizerTop);
    pBook->AddPage(pPageProject, _("Default Project"));

    // buttons
    wxButton* pButtonOk     = new wxButton(this, BFSETTINGSDLG_ID_BUTTONOK, _("&OK"));
    wxButton* pButtonCancel = new wxButton(this, BFSETTINGSDLG_ID_BUTTONCANCEL, _("&Cancel"));

    // sizer and arrange
    wxBoxSizer* pDlgSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    pDlgSizer->Add(pBook,                   wxSizerFlags(3).Border().Expand());
    pDlgSizer->Add(new wxStaticLine(this),  wxSizerFlags(0).Expand().Center().Border());
    pDlgSizer->Add(pButtonSizer,            wxSizerFlags(0).Center());
    SetSizerAndFit(pDlgSizer);
    GetData();
    ShowModal();
}


//
/*virtual*/ BFSettingsDlg::~BFSettingsDlg ()
{
}


void BFSettingsDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}


void BFSettingsDlg::OnButton_Ok (wxCommandEvent& rEvent)
{
    SetData();
    BFApp::SaveSettings();
    Close();
}

void BFSettingsDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}

void BFSettingsDlg::GetData ()
{
    BFSettings& rS = BFSettings::Instance();

    pCheckOpenLast_->SetValue(rS.GetOpenLastProject());
    pCheckMacro_->SetValue(rS.GetReplaceMacros());
    pCheckFiles_->SetValue(rS.GetWithFiles());
    pSpinLogSize_->SetValue(rS.GetMaxLogFileSize());
    pPrjCtrl_->GetData(rS.GetDefaultProjectSettings());

    switch (rS.GetVerboseLevelLog())
    {
        case MsgBACKUP:
            pComboVerboseLog_->SetSelection(0);
            break;

        case MsgWARNING:
            pComboVerboseLog_->SetSelection(2);
            break;

        case MsgERROR:
            pComboVerboseLog_->SetSelection(3);
            break;

        case MsgFATAL:
            pComboVerboseLog_->SetSelection(4);
            break;

        case MsgDEBUG:
            pComboVerboseLog_->SetSelection(5);
            break;

        case MsgUNKNOWN:
        case MsgLOG:
        case MsgINFO:
        default:
            pComboVerboseLog_->SetSelection(1);
            break;
    };
}

void BFSettingsDlg::SetData ()
{
    BFSettings& rS = BFSettings::Instance();

    rS.SetOpenLastProject(pCheckOpenLast_->GetValue());
    rS.SetReplaceMacros(pCheckMacro_->GetValue());
    rS.SetWithFiles(pCheckFiles_->GetValue());
    rS.SetMaxLogFileSize(pSpinLogSize_->GetValue());
    pPrjCtrl_->SetData(rS.GetDefaultProjectSettings());

    switch (pComboVerboseLog_->GetSelection())
    {
        case 0:
            rS.SetVerboseLevelLog(MsgBACKUP);
            break;

        case 2:
            rS.SetVerboseLevelLog(MsgWARNING);
            break;

        case 3:
            rS.SetVerboseLevelLog(MsgERROR);
            break;

        case 4:
            rS.SetVerboseLevelLog(MsgFATAL);
            break;

        case 5:
            rS.SetVerboseLevelLog(MsgDEBUG);
            break;

        case 1:
        default:
            rS.SetVerboseLevelLog(MsgINFO);
            break;
    };
}

