#include "BFTestDlg.h"

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
#include "BFHelpCtrl.h"

BEGIN_EVENT_TABLE(BFTestDlg, wxDialog)
  EVT_CLOSE     (BFTestDlg::OnClose)
END_EVENT_TABLE()

//
BFTestDlg::BFTestDlg (wxWindow* pParent)
             : wxDialog(pParent, wxID_ANY, wxString(_("Global Settings")))
{
    // the book
    wxTreebook* pBook = new wxTreebook(this,
                                       wxID_ANY,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxNB_LEFT);
    pBook->SetInternalBorder(15);

    // *** PROJECT-page ***
    wxPanel* pPage = new wxPanel(pBook);
    wxBoxSizer* pPageSizerTop       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pSubSizer           = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pLabel    = new wxStaticText(pPage, wxID_ANY, _("L A B E L 1"));
    wxTextCtrl* pTextCtrl   = new wxTextCtrl(pPage, wxID_ANY);
    wxPanel* pPanel         = new wxPanel(pPage, wxID_ANY);
    wxStaticText* pLabel2   = new wxStaticText(pPanel, wxID_ANY, _("L A B E L 2"));
    wxStaticBoxSizer* pSubSizer2    = new wxStaticBoxSizer(wxHORIZONTAL, pPanel);
    pSubSizer->Add(pLabel,      wxSizerFlags(0).Expand());
    pSubSizer->Add(pTextCtrl,   wxSizerFlags(0).Expand());
    pSubSizer2->Add(pLabel2,    wxSizerFlags(0).Expand());
    pPanel->SetSizer(pSubSizer2);
    pPageSizerTop->Add(pSubSizer,       wxSizerFlags(0).Expand().Center().Border());
    pPageSizerTop->Add(pPanel,          wxSizerFlags(0).Expand());
    //pPageProjectSizerTop->Add(pPrjCtrl_,                        wxSizerFlags(0).Expand());//.Border(wxRIGHT, 50));
    pPage->SetSizer(pPageSizerTop);
    pBook->AddPage(pPage, _("T E S T"));

    // sizer and arrange
    wxBoxSizer* pDlgSizer       = new wxBoxSizer(wxVERTICAL);
    pDlgSizer->Add(pBook,                   wxSizerFlags(0).Expand().Border());
    SetSizerAndFit(pDlgSizer);
    ShowModal();
}


//
/*virtual*/ BFTestDlg::~BFTestDlg ()
{
}


void BFTestDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}
