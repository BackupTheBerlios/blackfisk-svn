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

BEGIN_EVENT_TABLE(BFTaskBaseDlg, wxDialog)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONOK,       BFTaskBaseDlg::OnButton_Ok)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONCANCEL,   BFTaskBaseDlg::OnButton_Cancel)
END_EVENT_TABLE()


BFTaskBaseDlg::BFTaskBaseDlg (wxWindow* pParent,
                              const wxString& strTitle,
                              BFTask& rTask)
         : wxDialog(pParent, -1, strTitle),
           rTask_(rTask),
           pNameCtrl_(NULL),
           pSourceCtrl_(NULL),
           pDestCtrl_(NULL)
{
    Init();
    ShowModal();
}

/*virtual*/ BFTaskBaseDlg::~BFTaskBaseDlg ()
{
}

void BFTaskBaseDlg::Init ()
{
    wxSizer* pTypeSizer = InitType();

    // name
    wxStaticText*   pNameStatic = new wxStaticText(this, -1, _("destination name:"));
                    pNameCtrl_  = new wxTextCtrl(this, -1);
                    SetRowSize(pNameStatic, pNameCtrl_);

    // source
    wxStaticText*   pSourceStatic   = new wxStaticText(this, -1, _("source:"));
                    pSourceCtrl_    = new wxTextCtrl(this, -1);
                    pSourceCtrl_->Disable();
                    SetRowSize(pSourceStatic, pSourceCtrl_);

    // destination
    wxStaticText*   pDestStatic = new wxStaticText(this, -1, _("destination path:"));
                    pDestCtrl_  = new wxDirPickerCtrl(this, -1, wxEmptyString, _("select a destination folder"));
                    SetRowSize(pDestStatic, pDestCtrl_);

    // verify
    wxStaticText*   pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
                    pVerifyCheck_   = new wxCheckBox(this, -1, wxEmptyString);
                    SetRowSize(pVerifyStatic, pVerifyCheck_);

    // exclude
    /*wxStaticText*   pExcludeStatic  = new wxStaticText(this, -1, _("files and directories to exclude from this task"));
                    pExcludeStatic->SetMinSize(wxSize(lWidth1+lWidth2, pExcludeStatic->GetSize().GetHeight()));
                    pExcludeList_   = new wxListBox(this, -1);
                    pExcludeList_->SetMinSize(wxSize(lWidth1+lWidth2, pExcludeList_->GetSize().GetHeight()));
*/

    wxSizer* pButtonSizer = InitButtons ();

    // sizer
    wxBoxSizer* pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSourceSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pDestSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pVerifySizer    = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer* pExcludeSizer   = new wxBoxSizer(wxVERTICAL);


    // arrange
    pNameSizer      ->Add(pNameStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer      ->Add(pNameCtrl_);
    pSourceSizer    ->Add(pSourceStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSourceSizer    ->Add(pSourceCtrl_);
    pDestSizer      ->Add(pDestStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pDestSizer      ->Add(pDestCtrl_);
    pVerifySizer    ->Add(pVerifyStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pVerifySizer    ->Add(pVerifyCheck_);
    //pExcludeSizer   ->Add(pExcludeStatic,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    //pExcludeSizer   ->Add(pExcludeList_);

    pTopSizer->Add(pTypeSizer);
    pTopSizer->Add(pSourceSizer,    wxSizerFlags(0).Border());
    pTopSizer->Add(pDestSizer,      wxSizerFlags(0).Border());
    pTopSizer->Add(pNameSizer,      wxSizerFlags(0).Border());
    pTopSizer->Add(pVerifySizer,    wxSizerFlags(0).Border());
    //pTopSizer->Add(pExcludeSizer,   wxSizerFlags(0).Border());
    pTopSizer->Add(pButtonSizer,    wxSizerFlags(0).Center());
    SetSizerAndFit(pTopSizer);

    GetData();
}

wxSizer* BFTaskBaseDlg::InitButtons ()
{
    wxButton* pButtonOk     = new wxButton(this, BFTASKDLG_ID_BUTTONOK, _("OK"));
    wxButton* pButtonCancel = new wxButton(this, BFTASKDLG_ID_BUTTONCANCEL, _("Cancel"));

    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);

    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());

    return pButtonSizer;
}

void BFTaskBaseDlg::SetRowSize (wxWindow* pCol1, wxWindow* pCol2)
{
    if (pCol1 == NULL || pCol2 == NULL)
        return;

    long lWidth1 = 100;   // width of the label column
    long lWidth2 = 250;   // width of the editable column

    pCol1->SetMinSize(wxSize(lWidth1, pCol1->GetSize().GetHeight()));
    pCol2->SetMinSize(wxSize(lWidth2, pCol2->GetSize().GetHeight()));
}

void BFTaskBaseDlg::OnButton_Ok (wxCommandEvent& rEvent)
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

void BFTaskBaseDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    // is it a unsaved/new created task?
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        delete &rTask_;

    // close the dialog
    Close();
}

void BFTaskBaseDlg::GetData ()
{
    pNameCtrl_      ->SetValue(rTask_.GetName());
    pSourceCtrl_    ->SetValue(rTask_.GetSource());
    pDestCtrl_      ->SetPath(rTask_.GetDestination());
    pVerifyCheck_   ->SetValue(rTask_.Verify());
    //pExcludeList_   ->Clear();
    //pExcludeList_   ->Set(rTask_.GetExclude());
}

bool BFTaskBaseDlg::IsPlausible ()
{
    if ( pNameCtrl_->IsEmpty() )
    {
        BFSystem::Info(_("Please insert a task name!"));
        return false;
    }

    if ( pDestCtrl_->GetPath().IsEmpty() )
    {
        BFSystem::Info(_("Please insert a destination!"));
        return false;
    }

    return true;
}

void BFTaskBaseDlg::SetData ()
{
    // name
    if (pNameCtrl_->IsModified())
    {
        rTask_.SetName(pNameCtrl_->GetValue());
        pNameCtrl_->DiscardEdits();
    }

    // source
    if (pSourceCtrl_->IsModified())
    {
        rTask_.SetSource(pSourceCtrl_->GetValue());
        pSourceCtrl_->DiscardEdits();
    }

    // destination
    if (pDestCtrl_->GetPath() != rTask_.GetDestination())
        rTask_.SetDestination(pDestCtrl_->GetPath());

    // verify
    if (pVerifyCheck_->GetValue() != rTask_.Verify())
        rTask_.SetVerify(pVerifyCheck_->GetValue());

    // exclude list
/*    wxArrayString arrExclude = pExcludeList_->GetStrings();

    if (arrExclude != rTask_.GetExclude())
        rTask_.SetExclude(arrExclude);
*/
    // add task if needed
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        BFRootTask::Instance().AppendTask(rTask_);
}


/*static*/ void BFTaskBaseDlg::Show (BFTask* pTask)
{
    if (pTask == NULL)
        return;

    switch (pTask->GetType())
    {
        case TaskFILECOPY:
            BFTaskFileCopyDlg(NULL, *pTask);
            break;

        case TaskINVALID:
        default:
            BFSystem::Fatal (_T("unhandled task type"), _T("BFTaskBaseDlg::Show()"));
            break;
/*#define TaskINVALID     0
#define TaskARCHIVE     1       // comprase backup-source to an archive
#define TaskDIRCOPY     2       // copy a complete directory
#define TaskFILECOPY    3       // copy a file*/
    };
}


BFTaskFileCopyDlg::BFTaskFileCopyDlg (wxWindow* pParent, BFTask& rTask)
                 : BFTaskBaseDlg(pParent, _("file copy"), rTask)
{
}

BFTaskFileCopyDlg::~BFTaskFileCopyDlg ()
{
}

wxSizer* BFTaskFileCopyDlg::InitType ()
{
    wxStaticBitmap* pTypeIcon = new wxStaticBitmap(this, -1, wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_fc.ico")),  wxBITMAP_TYPE_ICO));
    wxStaticText*   pTypeText = new wxStaticText(this, -1, rTask_.GetTypeDescription());

    wxBoxSizer* pTypeSizer = new wxBoxSizer(wxHORIZONTAL);

    pTypeSizer->Add(pTypeIcon, wxSizerFlags(0).Border());
    pTypeSizer->Add(pTypeText, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border());

    return pTypeSizer;
}
