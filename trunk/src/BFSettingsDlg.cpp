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
    pDlgSizer->Add(pBook,                   wxSizerFlags(0).Border(wxRIGHT, 5));
    pDlgSizer->Add(new wxStaticLine(this),  wxSizerFlags(0).Expand().Border().Center());
    pDlgSizer->Add(pButtonSizer,            wxSizerFlags(0).Center());
    SetSizerAndFit(pDlgSizer);
    GetData();
    ShowModal();
}

void BFSettingsDlg::AddHead (wxSizer* pSizer,
                             wxWindow* pParent,
                             const wxString& strHeadline)
{
    if (pSizer == NULL)
        return;

    wxStaticText* pHead = new wxStaticText(pParent, wxID_ANY, strHeadline);

    wxFont font = pHead->GetFont();
    font.SetPointSize(font.GetPointSize() + 4);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    font.SetUnderlined(true);
    pHead->SetFont(font);

    pSizer->Add(pHead, wxSizerFlags(0).Center().Border(wxBOTTOM, 15));
}


wxWindow* BFSettingsDlg::CreatePage_General (wxTreebook* pBook)
{
    // page panel
    wxPanel* pPage = new wxPanel(pBook);

    // help ctrl
    BFHelpCtrl* pHelpCtrl = new BFHelpCtrl(pPage);

    // language
    wxStaticText* pLabelLanguage = new wxStaticText(pPage, wxID_ANY, _("Language: "));
    pComboLanguage_ = new wxComboBox(pPage,
                                     wxID_ANY,
                                     BFSettings::langNames_[0],
                                     wxDefaultPosition,
                                     wxDefaultSize,
                                     BFSettings::langCount_,
                                     BFSettings::langNames_,
                                     wxCB_READONLY);
    pHelpCtrl->Connect(pLabelLanguage, pComboLanguage_, _("The language for the user interface."));

    // open last project on startup
    pCheckOpenLast_ = new wxCheckBox(pPage, wxID_ANY, _("open the last project on startup"));
    pHelpCtrl->Connect(pCheckOpenLast_, _("Open the last opened project automaticly on startup."));

    // arrange
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* pGBSizer = new wxGridBagSizer(5);

    pGBSizer->Add(pLabelLanguage,     wxGBPosition(0, 0), wxGBSpan(),     wxALIGN_CENTER_VERTICAL);
    pGBSizer->Add(pComboLanguage_,    wxGBPosition(0, 1), wxGBSpan(),     wxALIGN_CENTER_VERTICAL);
    pGBSizer->Add(pCheckOpenLast_,    wxGBPosition(1, 0), wxGBSpan(1, 2), wxALIGN_CENTER_VERTICAL);

    AddHead(pSizer, pPage, _("General"));
    pSizer->Add(pGBSizer,   wxSizerFlags(1).Expand());
    pSizer->Add(pHelpCtrl,  wxSizerFlags(0).Expand().Bottom());

    pPage->SetSizer(pSizer);

    return pPage;
}

wxWindow* BFSettingsDlg::CreatePage_View (wxTreebook* pBook)
{
    // page panel
    wxPanel* pPage = new wxPanel(pBook);

    // help ctrl
    BFHelpCtrl* pHelpCtrl = new BFHelpCtrl(pPage);

    // switch backup tree and directory tree
    pCheckSwitchTrees_ = new wxCheckBox(pPage, wxID_ANY, _("switch Backup Tree && Directory Tree"));
    pHelpCtrl->Connect(pCheckSwitchTrees_, _("Change the order in the splitted main window " \
                                             "for the Backup Tree and the Directory Tree.\n" \
                                             "If not checked the backup tree is the first control " \
                                             "and the directory tree the second one.\n" \
                                             "You need to restart blackfisk to see an effect."));

    // display placeholders in backup tree
    pCheckMacro_ = new wxCheckBox(pPage, wxID_ANY, _("display filled placeholders (by default)"));
    pHelpCtrl->Connect(pCheckMacro_,
                       _("If checked the placeholders in the backuptree will be filled.\ne.g. \"<date>\" becomes \"2000-12-24\""));

    // display files in dir directory tree
    pCheckFiles_ = new wxCheckBox(pPage, wxID_ANY, _("display with files (by default)"));
    pHelpCtrl->Connect(pCheckFiles_, _("If checked the directory tree is displayed with files in it."));

    // display hidden files and dirs in directory tree
    pCheckHiddenFiles_ = new wxCheckBox(pPage, wxID_ANY, _("display with hidden files && directories (by default)"));
    pHelpCtrl->Connect(pCheckHiddenFiles_, _("If checked hidden files and directories are displayed in the directory tree."));

    // arrange
    wxBoxSizer*         pSizer              = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer*     pGBSizer            = new wxGridBagSizer(5);
    wxStaticBoxSizer*   pSBSizer_General    = new wxStaticBoxSizer(wxVERTICAL, pPage, _("General"));
    wxStaticBoxSizer*   pSBSizer_Backup     = new wxStaticBoxSizer(wxVERTICAL, pPage, _("Backup Tree"));
    wxStaticBoxSizer*   pSBSizer_Dir        = new wxStaticBoxSizer(wxVERTICAL, pPage, _("Directory Tree"));

    pSBSizer_General->Add(pCheckSwitchTrees_,   wxSizerFlags(0).Border());
    pSBSizer_Backup ->Add(pCheckMacro_,         wxSizerFlags(0).Border());
    pSBSizer_Dir    ->Add(pCheckFiles_,         wxSizerFlags(0).Border());
    pSBSizer_Dir    ->Add(pCheckHiddenFiles_,   wxSizerFlags(0).Border());

    pGBSizer->Add(pSBSizer_General,     wxGBPosition(0, 0), wxGBSpan(1, 2), wxEXPAND);
    pGBSizer->Add(pSBSizer_Backup,      wxGBPosition(1, 0), wxGBSpan(1, 2), wxEXPAND);
    pGBSizer->Add(pSBSizer_Dir,         wxGBPosition(2, 0), wxGBSpan(1, 2), wxEXPAND);
    //pGBSizer->Add(pCheckMacro_,         wxGBPosition(1, 0), wxGBSpan(1, 2));
    //pGBSizer->Add(pCheckFiles_,         wxGBPosition(2, 0), wxGBSpan(1, 2));
    //pGBSizer->Add(pCheckHiddenFiles_,   wxGBPosition(3, 0), wxGBSpan(1, 2));
    //pGBSizer->Add(pCheckSwitchTrees_,   wxGBPosition(3, 0), wxGBSpan(1, 2));

    AddHead(pSizer, pPage, _("View"));
    pSizer->Add(pGBSizer,   wxSizerFlags(1).Expand());
    pSizer->Add(pHelpCtrl,  wxSizerFlags(0).Expand().Bottom());

    pPage->SetSizer(pSizer);

    return pPage;
}

wxWindow* BFSettingsDlg::CreatePage_Log (wxTreebook* pBook)
{
    // page panel
    wxPanel* pPage = new wxPanel(pBook);

    // help ctrl
    BFHelpCtrl* pHelpCtrl = new BFHelpCtrl(pPage);

    // log size
    wxStaticText* pLabelLogSize = new wxStaticText(pPage, wxID_ANY, _("size (in KB) of application logfile: "));
    pSpinLogSize_               = new wxSpinCtrl(pPage,
                                                 wxID_ANY,
                                                 wxEmptyString,
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 0,
                                                 0,
                                                 INT_MAX);
    pHelpCtrl->Connect(pLabelLogSize, pSpinLogSize_,
                       _("The maximum size of the log file bf.log. If the " \
                         "maximum size is reached the bf.log will be moved " \
                         "to bf.log.bak and a new and empty bf.log will be created."));

    // log verbose
    wxStaticText* pLabelVerboseLog = new wxStaticText(pPage, wxID_ANY, _("verbose level for the application log: "));
    wxString arrVerbose[] = { _("backup"), _("information"), _("warning"), _("error"), _("fatal error"), _("debug") };
    pComboVerboseLog_ = new wxComboBox(pPage,
                                       wxID_ANY,
                                       arrVerbose[1],
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       6,
                                       arrVerbose,
                                       wxCB_READONLY);
    pHelpCtrl->Connect(pComboVerboseLog_, pLabelVerboseLog,
                       _("How detailed are the informations written to the application-logfile."));

    // arrange
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* pGBSizer = new wxGridBagSizer(5);

    pGBSizer->Add(pLabelLogSize,       wxGBPosition(0, 0), wxGBSpan(), wxALIGN_CENTER_VERTICAL);
    pGBSizer->Add(pSpinLogSize_,       wxGBPosition(0, 1), wxGBSpan(), wxRIGHT, 14);
    pGBSizer->Add(pLabelVerboseLog,    wxGBPosition(1, 0), wxGBSpan(), wxALIGN_CENTER_VERTICAL);
    pGBSizer->Add(pComboVerboseLog_,   wxGBPosition(1, 1), wxGBSpan());

    AddHead(pSizer, pPage, _("Logging"));
    pSizer->Add(pGBSizer,   wxSizerFlags(1).Expand());
    pSizer->Add(pHelpCtrl,  wxSizerFlags(0).Expand().Bottom());

    pPage->SetSizer(pSizer);

    return pPage;
}

wxWindow* BFSettingsDlg::CreatePage_Project (wxTreebook* pBook)
{
    // page panel
    wxPanel* pPage = new wxPanel(pBook);

    // help ctrl
    BFHelpCtrl* pHelpCtrl = new BFHelpCtrl(pPage);

    // label
    wxStaticText* pLabelProject = new wxStaticText
                                  (
                                        pPage,
                                        wxID_ANY,
                                        _("Default Settings for new projects\nAttention: This doesn't affect the existing projects!")
                                  );

    // project settings control
    pPrjCtrl_ = new BFProjectSettingsCtrl(pPage, pHelpCtrl);

    // arrange
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pBSizer = new wxBoxSizer(wxVERTICAL);

    pBSizer->Add(pLabelProject,                 wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBSizer->Add(new wxStaticLine(pPage),       wxSizerFlags(0).Expand().Center().Border());
    pBSizer->Add(pPrjCtrl_,                     wxSizerFlags(0).Border(wxRIGHT, 15));

    AddHead(pSizer, pPage, _("Default Project"));
    pSizer->Add(pBSizer,    wxSizerFlags(1).Expand());
    pSizer->Add(pHelpCtrl,  wxSizerFlags(0).Expand().Bottom());

    pPage->SetSizer(pSizer);

    return pPage;
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
    pCheckHiddenFiles_->SetValue(rS.GetShowHiddenFiles());
    pCheckSwitchTrees_->SetValue(rS.GetSwitchMainCtrls());
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
    rS.SetShowHiddenFiles(pCheckHiddenFiles_->GetValue());
    rS.SetSwitchMainCtrls(pCheckSwitchTrees_->GetValue());
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

