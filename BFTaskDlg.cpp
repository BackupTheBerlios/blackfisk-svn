/**
 * implementation of class BFTaskDlg
 * 2006-06-05
 ***/

#include "BFTaskDlg.h"

#include <wx/statbmp.h>
#include <wx/panel.h>

#include "blackfisk.h"
#include "BFCore.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFRootTask.h"
#include "BFIconTable.h"
#include "BFDestinationCtrl.h"

BEGIN_EVENT_TABLE(BFTaskBaseDlg, wxDialog)
  EVT_CLOSE   (BFTaskBaseDlg::OnClose)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONOK,       BFTaskBaseDlg::OnButton_Ok)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONCANCEL,   BFTaskBaseDlg::OnButton_Cancel)
END_EVENT_TABLE()

/*static*/ const long BFTaskBaseDlg::lWidth1_ = 95;
/*static*/ const long BFTaskBaseDlg::lWidth2_ = 280;

BFTaskBaseDlg::BFTaskBaseDlg (wxWindow* pParent,
                              const wxString& strTitle,
                              BFTask& rTask)
         : wxDialog(pParent, wxID_ANY, strTitle),
           rTask_(rTask),
           pNameCtrl_(NULL),
           pSourceCtrl_(NULL),
           pDestCtrl_(NULL),
           pVerifyCheck_(NULL)
{
    pNameCtrl_      = new wxTextCtrl(this, -1);
    pSourceCtrl_    = new wxTextCtrl(this, -1);
    pDestCtrl_      = new BFDestinationCtrl(this);
    pVerifyCheck_   = new wxCheckBox(this, -1, wxEmptyString);
}

/*virtual*/ BFTaskBaseDlg::~BFTaskBaseDlg ()
{
}

wxSizer* BFTaskBaseDlg::InitType ()
{
    wxStaticBitmap* pTypeIcon = new wxStaticBitmap
                                    (
                                        this,
                                        -1,
                                        BFIconTable::Instance()->GetIcon(rTask_.GetTypeIconId())
                                    );
    wxStaticText*   pTypeText = new wxStaticText(this, -1, rTask_.GetTypeDescription());

    wxBoxSizer* pTypeSizer = new wxBoxSizer(wxHORIZONTAL);

    pTypeSizer->Add(pTypeIcon, wxSizerFlags(0).Border());
    pTypeSizer->Add(pTypeText, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border());

    return pTypeSizer;
}

wxSizer* BFTaskBaseDlg::InitButtons ()
{
    wxButton* pButtonOk     = new wxButton(this, BFTASKDLG_ID_BUTTONOK, _("&OK"));
    wxButton* pButtonCancel = new wxButton(this, BFTASKDLG_ID_BUTTONCANCEL, _("&Cancel"));

    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);

    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());

    return pButtonSizer;
}

void BFTaskBaseDlg::OnClose(wxCloseEvent& rEvent)
{
    // is it a unsaved/new created task?
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        delete &rTask_;

    Destroy();
}

/*static*/ void BFTaskBaseDlg::SetRowSize (wxWindow* pCol1, wxWindow* pCol2)
{
    if (pCol1 == NULL || pCol2 == NULL)
        return;

    pCol1->SetMinSize(wxSize(lWidth1_, pCol1->GetSize().GetHeight()));
    pCol2->SetMinSize(wxSize(lWidth2_, pCol2->GetSize().GetHeight()));
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
    else
        BFRootTask::Instance().broadcastObservers();
}

void BFTaskBaseDlg::Arrange(wxSizer* pBody, wxSizer* pBodyExtra /*= NULL*/)
{
    // main sizer
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);

    // type
    pMainSizer->Add (InitType(),  wxSizerFlags(0).Border());

    // body
    if (pBody != NULL)
        pMainSizer->Add (pBody, wxSizerFlags(0));

    // body extra
    if (pBodyExtra != NULL)
        pMainSizer->Add (pBodyExtra, wxSizerFlags(0));

    // buttons
    pMainSizer->Add(InitButtons(), wxSizerFlags(0).Border().Center());

    SetSizerAndFit(pMainSizer);
}

/*static*/ void BFTaskBaseDlg::Show (BFTask* pTask)
{
    if (pTask == NULL)
        return;

    switch (pTask->GetType())
    {
        case TaskFILECOPY:
            new BFTaskFileCopyDlg(BFMainFrame::Instance(), *pTask);
            break;

        case TaskDIRCOPY:
            new BFTaskDirectoryCopyDlg(BFMainFrame::Instance(), *pTask);
            break;

        case TaskSYNC:
            new BFTaskDirectoryCopyDlg(BFMainFrame::Instance(), *pTask);
            break;

        //case TaskARCHIVE:

        case TaskINVALID:
        default:
            BFSystem::Fatal (_T("unhandled task type"), _T("BFTaskBaseDlg::Show()"));
            break;
    };
}


BFTaskFileCopyDlg::BFTaskFileCopyDlg (wxWindow* pParent, BFTask& rTask)
                 : BFTaskBaseDlg(pParent, _("file copy"), rTask)
{
    // name
    wxStaticText* pNameStatic = new wxStaticText(this, -1, _("destination name:"));
    SetRowSize(pNameStatic, pNameCtrl_);

    // source
    wxStaticText* pSourceStatic   = new wxStaticText(this, -1, _("source:"));
    pSourceCtrl_->Disable();
    SetRowSize(pSourceStatic, pSourceCtrl_);

    // destination
    // is handled by the base-ctor and the ctrl itself
    //wxStaticText* pDestStatic = new wxStaticText(this, -1, _("destination path:"));
    //SetRowSize(pDestStatic, pDestCtrl_);

    // verify
    wxStaticText* pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
    SetRowSize(pVerifyStatic, pVerifyCheck_);

    // sizer
    wxBoxSizer* pBodySizer      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSourceSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pDestSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pVerifySizer    = new wxBoxSizer(wxHORIZONTAL);

    // arrange
    pNameSizer      ->Add(pNameStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer      ->Add(pNameCtrl_);
    pSourceSizer    ->Add(pSourceStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSourceSizer    ->Add(pSourceCtrl_);
    pDestSizer      ->Add(pDestCtrl_);
    pVerifySizer    ->Add(pVerifyStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pVerifySizer    ->Add(pVerifyCheck_);

    pBodySizer->Add(pSourceSizer,    wxSizerFlags(0).Border());
    pBodySizer->Add(pDestSizer,      wxSizerFlags(0).Border());
    pBodySizer->Add(pNameSizer,      wxSizerFlags(0).Border());
    pBodySizer->Add(pVerifySizer,    wxSizerFlags(0).Border());

    Arrange(pBodySizer);

    GetData();
    ShowModal();
}

BFTaskFileCopyDlg::~BFTaskFileCopyDlg ()
{
}


BFTaskDirectoryCopyDlg::BFTaskDirectoryCopyDlg (wxWindow* pParent, BFTask& rTask)
                      : BFTaskBaseDlg(pParent, _("directory copy"), rTask)
{
    // name
    wxStaticText* pNameStatic = new wxStaticText(this, -1, _("destination name:"));
    SetRowSize(pNameStatic, pNameCtrl_);

    // source
    wxStaticText* pSourceStatic   = new wxStaticText(this, -1, _("source:"));
    pSourceCtrl_->Disable();
    SetRowSize(pSourceStatic, pSourceCtrl_);

    // destination
    // is handled by the base-ctor and the ctrl itself
    //wxStaticText* pDestStatic = new wxStaticText(this, -1, _("destination path:"));
    //SetRowSize(pDestStatic, pDestCtrl_);

    // verify
    wxStaticText* pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
    SetRowSize(pVerifyStatic, pVerifyCheck_);

    /* exclude control
    wxStaticText* pExcludeStatic = new wxStaticText(this, -1, _("files and directories to exclude from this task"));
    pExcludeStatic->SetMinSize(wxSize(lWidth1_+lWidth2_, pExcludeStatic->GetSize().GetHeight()));
    pExcludeList_ = new wxListBox(this, -1);
    pExcludeList_->SetMinSize(wxSize(lWidth1_+lWidth2_, pExcludeList_->GetSize().GetHeight()));*/

    // sizer
    wxBoxSizer* pBodySizer      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pExtraSizer     = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSourceSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pDestSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pVerifySizer    = new wxBoxSizer(wxHORIZONTAL);

    // arrange
    pNameSizer      ->Add(pNameStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer      ->Add(pNameCtrl_);
    pSourceSizer    ->Add(pSourceStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSourceSizer    ->Add(pSourceCtrl_);
    pDestSizer      ->Add(pDestCtrl_);
    pVerifySizer    ->Add(pVerifyStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pVerifySizer    ->Add(pVerifyCheck_);
    /*pExtraSizer     ->Add(pExcludeStatic,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pExtraSizer     ->Add(pExcludeList_);*/

    pBodySizer->Add(pSourceSizer,   wxSizerFlags(0).Border());
    pBodySizer->Add(pDestSizer,     wxSizerFlags(0).Border());
    pBodySizer->Add(pNameSizer,     wxSizerFlags(0).Border());
    pBodySizer->Add(pVerifySizer,   wxSizerFlags(0).Border());
    //pBodySizer->Add(pExtraSizer,    wxSizerFlags(0).Border());

    Arrange(pBodySizer);

    GetData();
    ShowModal();
}

BFTaskDirectoryCopyDlg::~BFTaskDirectoryCopyDlg ()
{
}

/*
BFTaskSynchronizeDlg::BFTaskSynchronizeDlg (wxWindow* pParent, BFTask& rTask)
                      : BFTaskBaseDlg(pParent, _("directory copy"), rTask)
{
    // name
    wxStaticText* pNameStatic = new wxStaticText(this, -1, _("destination name:"));
    SetRowSize(pNameStatic, pNameCtrl_);

    // source
    wxStaticText* pSourceStatic   = new wxStaticText(this, -1, _("source:"));
    pSourceCtrl_->Disable();
    SetRowSize(pSourceStatic, pSourceCtrl_);

    // destination
    // is handled by the base-ctor and the ctrl itself
    //wxStaticText* pDestStatic = new wxStaticText(this, -1, _("destination path:"));
    //SetRowSize(pDestStatic, pDestCtrl_);

    // verify
    wxStaticText* pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
    SetRowSize(pVerifyStatic, pVerifyCheck_);

    /* exclude control
    wxStaticText* pExcludeStatic = new wxStaticText(this, -1, _("files and directories to exclude from this task"));
    pExcludeStatic->SetMinSize(wxSize(lWidth1_+lWidth2_, pExcludeStatic->GetSize().GetHeight()));
    pExcludeList_ = new wxListBox(this, -1);
    pExcludeList_->SetMinSize(wxSize(lWidth1_+lWidth2_, pExcludeList_->GetSize().GetHeight()));*/
/*
    // sizer
    wxBoxSizer* pBodySizer      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pExtraSizer     = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSourceSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pDestSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pVerifySizer    = new wxBoxSizer(wxHORIZONTAL);

    // arrange
    pNameSizer      ->Add(pNameStatic,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNameSizer      ->Add(pNameCtrl_);
    pSourceSizer    ->Add(pSourceStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSourceSizer    ->Add(pSourceCtrl_);
    pDestSizer      ->Add(pDestCtrl_);
    pVerifySizer    ->Add(pVerifyStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pVerifySizer    ->Add(pVerifyCheck_);
    /*pExtraSizer     ->Add(pExcludeStatic,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pExtraSizer     ->Add(pExcludeList_);*/
/*
    pBodySizer->Add(pSourceSizer,   wxSizerFlags(0).Border());
    pBodySizer->Add(pDestSizer,     wxSizerFlags(0).Border());
    pBodySizer->Add(pNameSizer,     wxSizerFlags(0).Border());
    pBodySizer->Add(pVerifySizer,   wxSizerFlags(0).Border());
    //pBodySizer->Add(pExtraSizer,    wxSizerFlags(0).Border());

    Arrange(pBodySizer);

    GetData();
    ShowModal();
}

BFTaskSynchronizeDlg::~BFTaskSynchronizeDlg ()
{
}
*/
