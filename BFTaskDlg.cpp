/**
 * implementation of class BFTaskDlg
 * 2006-06-05
 ***/

#include "blackfisk.h"
#include "BFTaskDlg.h"
#include "BFCore.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFRootTask.h"
#include <wx/statbmp.h>
#include <wx/panel.h>

BEGIN_EVENT_TABLE(BFTaskDlg, wxDialog)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONOK,       BFTaskDlg::OnButton_Ok)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONCANCEL,   BFTaskDlg::OnButton_Cancel)
END_EVENT_TABLE()


BFTaskDlg::BFTaskDlg (wxWindow* pParent, BFTask& rTask)
         : wxDialog(pParent, -1, wxString(_("OctopusBackup Task"))),
           rTask_(rTask),
           pNameCtrl_(NULL),
           pSourceCtrl_(NULL),
           pDestCtrl_(NULL)
{
    const long lWidth1 = 100;   // width of the label column
    const long lWidth2 = 250;   // width of the editable column

    // type
    wxStaticBitmap* pTypeIcon = new wxStaticBitmap(this, -1, wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_dc.ico")),  wxBITMAP_TYPE_ICO));
    wxStaticText*   pTypeText = new wxStaticText(this, -1, rTask.GetTypeDescription());

    // name
    wxStaticText*   pNameStatic = new wxStaticText(this, -1, _("destination name:"));
                    pNameStatic->SetMinSize(wxSize(lWidth1, pNameStatic->GetSize().GetHeight()));
                    pNameCtrl_  = new wxTextCtrl(this, -1);
                    pNameCtrl_->SetMinSize(wxSize(lWidth2, pNameCtrl_->GetSize().GetHeight()));

    // source
    wxStaticText*   pSourceStatic   = new wxStaticText(this, -1, _("source:"));
                    pSourceStatic->SetMinSize(wxSize(lWidth1, pSourceStatic->GetSize().GetHeight()));
                    pSourceCtrl_    = new wxTextCtrl(this, -1);
                    pSourceCtrl_->SetMinSize(wxSize(lWidth2, pSourceCtrl_->GetSize().GetHeight()));
                    pSourceCtrl_->Disable();

    // destination
    wxStaticText*   pDestStatic = new wxStaticText(this, -1, _("destination path:"));
                    pDestStatic->SetMinSize(wxSize(lWidth1, pDestStatic->GetSize().GetHeight()));
                    pDestCtrl_  = new wxTextCtrl(this, -1);
                    pDestCtrl_->SetMinSize(wxSize(lWidth2, pDestCtrl_->GetSize().GetHeight()));
                    pDestCtrl_->Disable();

    // verify
    wxStaticText*   pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
                    pVerifyStatic->SetMinSize(wxSize(lWidth1, pVerifyStatic->GetSize().GetHeight()));
                    pVerifyCheck_   = new wxCheckBox(this, -1, wxEmptyString);
                    pVerifyCheck_->SetMinSize(wxSize(lWidth2, pVerifyCheck_->GetSize().GetHeight()));


    // exclude
    wxStaticText*   pExcludeStatic  = new wxStaticText(this, -1, _("files and directories to exclude from this task"));
                    pExcludeStatic->SetMinSize(wxSize(lWidth1+lWidth2, pExcludeStatic->GetSize().GetHeight()));
                    pExcludeList_   = new wxListBox(this, -1);
                    pExcludeList_->SetMinSize(wxSize(lWidth1+lWidth2, pExcludeList_->GetSize().GetHeight()));

    // buttons
    //wxPanel*        pButtonCtrl     = new wxPanel(this, -1);
    //pButtonCtrl->SetBackgroundColour(*wxRED);
    wxButton*       pButtonOk       = new wxButton(this, BFTASKDLG_ID_BUTTONOK, _("OK"));
    wxButton*       pButtonCancel   = new wxButton(this, BFTASKDLG_ID_BUTTONCANCEL, _("Cancel"));

    // sizer
    wxBoxSizer* pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTypeSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSourceSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pDestSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pVerifySizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pExcludeSizer   = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);

    // arrange
    pTypeSizer      ->Add(pTypeIcon,        wxSizerFlags(0).Border());
    pTypeSizer      ->Add(pTypeText,        wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border());
    pNameSizer      ->Add(pNameStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer      ->Add(pNameCtrl_);
    pSourceSizer    ->Add(pSourceStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSourceSizer    ->Add(pSourceCtrl_);
    pDestSizer      ->Add(pDestStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pDestSizer      ->Add(pDestCtrl_);
    pVerifySizer    ->Add(pVerifyStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pVerifySizer    ->Add(pVerifyCheck_);
    pExcludeSizer   ->Add(pExcludeStatic,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pExcludeSizer   ->Add(pExcludeList_);
    pButtonSizer    ->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer    ->Add(pButtonCancel,    wxSizerFlags(0).Border());

    pTopSizer->Add(pTypeSizer);
    pTopSizer->Add(pSourceSizer,    wxSizerFlags(0).Border());
    pTopSizer->Add(pDestSizer,      wxSizerFlags(0).Border());
    pTopSizer->Add(pNameSizer,      wxSizerFlags(0).Border());
    pTopSizer->Add(pVerifySizer,    wxSizerFlags(0).Border());
    pTopSizer->Add(pExcludeSizer,   wxSizerFlags(0).Border());
    pTopSizer->Add(pButtonSizer,    wxSizerFlags(0).Center());
    SetSizerAndFit(pTopSizer);

    GetData();

    ShowModal();
}

/*virtual*/ BFTaskDlg::~BFTaskDlg ()
{
}

void BFTaskDlg::OnButton_Ok (wxCommandEvent& rEvent)
{
    // check if the data in the dialog is plausible
    if ( !(IsPlausible()) )
        return;

    // set the data from dialog to the data object
    // and append the data object to the rootObject if needed
    SetData();

    // close the dialog
    Close();
}

void BFTaskDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    // is it a unsaved/new created task?
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        delete &rTask_;

    // close the dialog
    Close();
}

void BFTaskDlg::GetData ()
{
    pNameCtrl_      ->SetValue(rTask_.GetName());
    pSourceCtrl_    ->SetValue(rTask_.GetSource());
    pDestCtrl_      ->SetValue(rTask_.GetDestination());
    pVerifyCheck_   ->SetValue(rTask_.Verify());
    pExcludeList_   ->Clear();
    pExcludeList_   ->Set(rTask_.GetExclude());
}

bool BFTaskDlg::IsPlausible ()
{
    if ( pNameCtrl_->IsEmpty() )
    {
        BFSystem::Info(_("Please insert a task name!"));
        return false;
    }

    if ( pDestCtrl_->IsEmpty() )
    {
        BFSystem::Info(_("Please insert a destination!"));
        return false;
    }

    return true;
}

void BFTaskDlg::SetData ()
{
    if (pNameCtrl_->IsModified())
    {
        rTask_.SetName(pNameCtrl_->GetValue());
        pNameCtrl_->DiscardEdits();
    }

    if (pSourceCtrl_->IsModified())
    {
        rTask_.SetSource(pSourceCtrl_->GetValue());
        pSourceCtrl_->DiscardEdits();
    }

    if (pDestCtrl_->IsModified())
    {
        rTask_.SetDestination(pDestCtrl_->GetValue());
        pDestCtrl_->DiscardEdits();
    }

    if (pVerifyCheck_->GetValue() != rTask_.Verify())
        rTask_.SetVerify(pVerifyCheck_->GetValue());

    wxArrayString arrExclude = pExcludeList_->GetStrings();

    if (arrExclude != rTask_.GetExclude())
        rTask_.SetExclude(arrExclude);

    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        BFRootTask::Instance().AppendTask(rTask_);
}

