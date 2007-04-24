/**
 * implementation of class BFSettingsDlg
 * 2007-04-23
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

    // view-page
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

    // log-page
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
    pPageLog->SetSizer(pPageLogSizerTop);
    pBook->AddPage(pPageLog, _("Logging"));

    // project-page
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

    pCheckMacro_->SetValue(rS.GetReplaceMacros());
    pCheckFiles_->SetValue(rS.GetWithFiles());
    pSpinLogSize_->SetValue(rS.GetMaxLogFileSize());
    pPrjCtrl_->GetData(rS.GetDefaultProjectSettings());
}

void BFSettingsDlg::SetData ()
{
    BFSettings& rS = BFSettings::Instance();

    rS.SetReplaceMacros(pCheckMacro_->GetValue());
    rS.SetWithFiles(pCheckFiles_->GetValue());
    rS.SetMaxLogFileSize(pSpinLogSize_->GetValue());
    pPrjCtrl_->SetData(rS.GetDefaultProjectSettings());
}

