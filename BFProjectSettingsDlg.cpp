/**
 * implementation of class BFProjectSettingsDlg
 * 2007-04-20
 ***/

#include "BFProjectSettingsDlg.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include "BFProjectSettingsCtrl.h"
#include "BFRootTask.h"


BEGIN_EVENT_TABLE(BFProjectSettingsDlg, wxDialog)
  EVT_CLOSE     (                               BFProjectSettingsDlg::OnClose)
  EVT_BUTTON    (BFPRJSETDLG_ID_BUTTONOK,       BFProjectSettingsDlg::OnButton_Ok)
  EVT_BUTTON    (BFPRJSETDLG_ID_BUTTONCANCEL,   BFProjectSettingsDlg::OnButton_Cancel)
END_EVENT_TABLE()


//
BFProjectSettingsDlg::BFProjectSettingsDlg (wxWindow* pParent)
                    : wxDialog(pParent, wxID_ANY, wxString(_("Project settings")))
{
    // controls
    pCtrl_ = new BFProjectSettingsCtrl(this);

    // name
    wxStaticText* pNameLabel = new wxStaticText(this, wxID_ANY, _("Name:"));
    pNameLabel->SetMinSize(wxSize(pCtrl_->GetLabelWidth(), pNameLabel->GetSize().GetHeight()));
    pNameCtrl_ = new wxTextCtrl(this, wxID_ANY);

    // button
    wxButton* pButtonOk     = new wxButton(this, BFPRJSETDLG_ID_BUTTONOK, _("&OK"));
    wxButton* pButtonCancel = new wxButton(this, BFPRJSETDLG_ID_BUTTONCANCEL, _("&Cancel"));

    // sizer
    wxBoxSizer* pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pNameSizer->Add(pNameLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer->Add(pNameCtrl_, wxSizerFlags(1));
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    pTopSizer->Add(pNameSizer,      wxSizerFlags(0).Border(wxLEFT | wxUP | wxRIGHT).Expand());
    pTopSizer->Add(pCtrl_,          wxSizerFlags(0).Border());
    pTopSizer->Add(pButtonSizer,    wxSizerFlags(0).Center());

    // arrange and show
    SetSizerAndFit(pTopSizer);
    GetData();
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
    Close();
}

void BFProjectSettingsDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}

void BFProjectSettingsDlg::GetData ()
{
    BFRootTask& rPrj = BFRootTask::Instance();

    pNameCtrl_->SetValue(rPrj.GetName());
    pCtrl_->GetData(rPrj.GetSettings());
}

void BFProjectSettingsDlg::SetData ()
{
    BFRootTask& rPrj = BFRootTask::Instance();
    bool bBroadcast = false;

    if (rPrj.GetName() != pNameCtrl_->GetValue())
    {
        rPrj.SetName(pNameCtrl_->GetValue());
        bBroadcast = true;
    }

    pCtrl_->SetData(rPrj.GetSettings());

    if (bBroadcast)
        rPrj.broadcastObservers();
}
