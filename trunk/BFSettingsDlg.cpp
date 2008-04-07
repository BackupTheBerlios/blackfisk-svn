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
#include <wx/gbsizer.h>
#include <limits.h>

#include "BFSystem.h"
#include "BFProjectSettingsCtrl.h"
#include "BFSettings.h"
#include "BFApp.h"
#include "BFHelpCtrl.h"

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
    pBook->SetInternalBorder(15);



    // *** GENERAL-page ***
    pBook->AddPage(CreatePage_General(pBook),  _("General"));

    // *** VIEW-page ***
    pBook->AddPage(CreatePage_View(pBook),      _("View"));

    // *** LOG-page ***
    pBook->AddPage(CreatePage_Log(pBook),       _("Logging"));

    // *** PROJECT-page ***
    pBook->AddPage(CreatePage_Project(pBook),   _("Default Project"));

    // buttons
    wxButton* pButtonOk     = new wxButton(this, BFSETTINGSDLG_ID_BUTTONOK, "&OK");
    wxButton* pButtonCancel = new wxButton(this, BFSETTINGSDLG_ID_BUTTONCANCEL, _("&Cancel"));

    // sizer and arrange
    wxBoxSizer* pDlgSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    pDlgSizer->Add(pBook,                   wxSizerFlags(0).Expand().Border());
    pDlgSizer->Add(new wxStaticLine(this),  wxSizerFlags(0).Expand().Border().Center());
    pDlgSizer->Add(pButtonSizer,            wxSizerFlags(0).Center());
    SetSizerAndFit(pDlgSizer);
    GetData();
    ShowModal();
}

wxWindow* BFSettingsDlg::CreatePage_General (wxTreebook* pBook)
{
    wxString strTip;
    wxPanel* pPageGeneral = new wxPanel(pBook);
    wxStaticText* pLabelLanguage = new wxStaticText(pPageGeneral, wxID_ANY, _("Language: "));
    pComboLanguage_ = new wxComboBox(pPageGeneral,
                                     wxID_ANY,
                                     BFSettings::langNames_[0],
                                     wxDefaultPosition,
                                     wxDefaultSize,
                                     BFSettings::langCount_,
                                     BFSettings::langNames_,
                                     wxCB_READONLY);
    pCheckOpenLast_ = new wxCheckBox(pPageGeneral,
                                     wxID_ANY,
                                     _("open the last project on startup"));
    pCheckOpenLast_->SetToolTip(_("open the last opened project automaticly on startup"));
    wxBoxSizer* pPageGeneralSizerTop        = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pPageGeneralSizerLanguage   = new wxBoxSizer(wxHORIZONTAL);
    pPageGeneralSizerLanguage->Add(pLabelLanguage, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPageGeneralSizerLanguage->Add(pComboLanguage_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));
    pPageGeneralSizerTop->Add(pPageGeneralSizerLanguage, wxSizerFlags(0).Border(wxBOTTOM, 5));
    pPageGeneralSizerTop->Add(pCheckOpenLast_);
    pPageGeneral->SetSizer(pPageGeneralSizerTop);

    return pPageGeneral;
}

wxWindow* BFSettingsDlg::CreatePage_View (wxTreebook* pBook)
{
    wxString strTip;
    wxPanel* pPageView = new wxPanel(pBook);
    pCheckMacro_ = new wxCheckBox(pPageView,
                                  wxID_ANY,
                                  _("view backup tree with filled placeholders"));
    pCheckMacro_->SetToolTip(_("fill the placeholders in the backuptree by default"));
    pCheckFiles_ = new wxCheckBox(pPageView,
                                  wxID_ANY,
                                  _("view directory tree with files"));
    pCheckFiles_->SetToolTip(_("show files in the directory tree by default"));
    wxBoxSizer* pPageViewSizerTop = new wxBoxSizer(wxVERTICAL);
    pPageViewSizerTop->Add(pCheckMacro_, wxSizerFlags(0).Border(wxBOTTOM, 5));
    pPageViewSizerTop->Add(pCheckFiles_);
    pPageView->SetSizer(pPageViewSizerTop);

    return pPageView;
}

wxWindow* BFSettingsDlg::CreatePage_Log (wxTreebook* pBook)
{
    wxString strTip;

    // page panel
    wxPanel* pPageLog           = new wxPanel(pBook);

    // log size
    wxStaticText* pLabelLogSize = new wxStaticText(pPageLog, wxID_ANY, _("size of application log file (KB): "));
    pSpinLogSize_               = new wxSpinCtrl(pPageLog,
                                                 wxID_ANY,
                                                 wxEmptyString,
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 0,
                                                 0,
                                                 INT_MAX);
    strTip = _("max size of the log file bf.log\nif bf.log is to big it will be moved to bf.log.bak\nand a new bf.log will be created");
    pLabelLogSize->SetToolTip(strTip);
    pSpinLogSize_->SetToolTip(strTip);

    // log verbose
    wxStaticText* pLabelVerboseLog = new wxStaticText(pPageLog, wxID_ANY, _("verbose level for the application log: "));
    wxString arrVerbose[] = { _("backup"), _("information"), _("warning"), _("error"), _("fatal error"), _("debug") };
    pComboVerboseLog_ = new wxComboBox(pPageLog,
                                       wxID_ANY,
                                       arrVerbose[1],
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       6,
                                       arrVerbose,
                                       wxCB_READONLY);
    strTip = _("how detailed are the informations\nwritten to the application log-file");
    pLabelVerboseLog->SetToolTip(strTip);
    pComboVerboseLog_->SetToolTip(strTip);

    // arrange
    wxGridBagSizer* pSizerTop = new wxGridBagSizer(5);
    pSizerTop->Add(pLabelLogSize,       wxGBPosition(0, 0), wxGBSpan(), wxALIGN_CENTER_VERTICAL);
    pSizerTop->Add(pSpinLogSize_,       wxGBPosition(0, 1), wxGBSpan());
    pSizerTop->Add(pLabelVerboseLog,    wxGBPosition(1, 0), wxGBSpan(), wxALIGN_CENTER_VERTICAL);
    pSizerTop->Add(pComboVerboseLog_,   wxGBPosition(1, 1), wxGBSpan());
    pPageLog->SetSizer(pSizerTop);

    return pPageLog;
}

wxWindow* BFSettingsDlg::CreatePage_Project (wxTreebook* pBook)
{
    wxString strTip;
    wxPanel* pPageProject = new wxPanel(pBook);
    wxBoxSizer* pPageProjectSizerTop = new wxBoxSizer(wxVERTICAL);
    wxStaticText* pLabelProject = new wxStaticText(pPageProject, wxID_ANY, _("Default Settings for new projects\nAttention: This doesn't affect the existing projects!"));
    BFHelpCtrl* pHelpCtrl = new BFHelpCtrl(pPageProject);
    pPrjCtrl_ = new BFProjectSettingsCtrl(pPageProject, pHelpCtrl);
    pPageProjectSizerTop->Add(pLabelProject,                    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPageProjectSizerTop->Add(new wxStaticLine(pPageProject),   wxSizerFlags(0).Expand().Center().Border());
    pPageProjectSizerTop->Add(pPrjCtrl_,                        wxSizerFlags(0).Border(wxRIGHT, 15));
    pPageProjectSizerTop->Add(pHelpCtrl,                        wxSizerFlags(0).Expand());
    pPageProject->SetSizer(pPageProjectSizerTop);

    return pPageProject;
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

    int iLangSel = 0;
    for (int i = 0; i < BFSettings::langCount_; ++i)
    {
        if (BFSettings::langIds_[i] == rS.GetLanguage())
        {
            iLangSel = i;
            break;
        }
    }
    pComboLanguage_->SetSelection(iLangSel);
    pCheckOpenLast_->SetValue(rS.GetOpenLastProject());
    pCheckMacro_->SetValue(rS.GetFillBlackfiskPlaceholders());
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

    rS.SetLanguage(BFSettings::langIds_[pComboLanguage_->GetSelection()]);
    rS.SetOpenLastProject(pCheckOpenLast_->GetValue());
    rS.SetFillBlackfiskPlaceholders(pCheckMacro_->GetValue());
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

