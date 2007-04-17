/**
 * implementation of class BFDestinationDlg
 * 2007-03-28
 ***/

#include "BFDestinationDlg.h"

#include "BFDestinationCtrl.h"
#include "BFSystem.h"
#include "BFIconTable.h"
#include "BFMainFrame.h"
#include "BFBackupTree.h"

BEGIN_EVENT_TABLE(BFDestinationDlg, wxDialog)
  EVT_CLOSE   (BFDestinationDlg::OnClose)
  EVT_BUTTON  (BFDESTINATIONDLG_ID_BUTTONOK,        BFDestinationDlg::OnButton_Ok)
  EVT_BUTTON  (BFDESTINATIONDLG_ID_BUTTONCANCEL,    BFDestinationDlg::OnButton_Cancel)
END_EVENT_TABLE()

//
BFDestinationDlg::BFDestinationDlg (wxWindow* pParent, const wxChar* strPath /*= wxEmptyString*/)
                : wxDialog(pParent, wxID_ANY, wxString(_("create a destination"))),
                  pDestCtrl_(NULL)
{
    // dialog icon
    SetIcon(BFIconTable::Instance()->GetIcon(BFIconTable::folder));

    // destination control
    pDestCtrl_ = new BFDestinationCtrl(this, strPath);

    // buttons
    wxButton* pButtonOk     = new wxButton(this, BFDESTINATIONDLG_ID_BUTTONOK, _("&OK"));
    wxButton* pButtonCancel = new wxButton(this, BFDESTINATIONDLG_ID_BUTTONCANCEL, _("&Cancel"));

    // sizer
    wxBoxSizer* pSizer          = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);

    // arrange/layout
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    pSizer      ->Add(pDestCtrl_,       wxSizerFlags(0).Center().Border());
    pSizer      ->Add(pButtonSizer,     wxSizerFlags(0).Center());

    SetSizerAndFit(pSizer);
    ShowModal();
}


//
/*virtual*/ BFDestinationDlg::~BFDestinationDlg ()
{
}

void BFDestinationDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}

void BFDestinationDlg::OnButton_Ok (wxCommandEvent& rEvent)
{
    // get the backup tree
    BFBackupTree* pBackupTree = BFMainFrame::Instance()->BackupTree();

    // add and select the destination dir
    pBackupTree->SelectItem(pBackupTree->AddDestination(pDestCtrl_->GetPath()));

    Close();
}

void BFDestinationDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}
