/**
 * Name:        BFTaskDlg.cpp
 * Purpose:     BFTaskDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-06-05
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFTaskDlg.h"

#include <wx/statbmp.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/gbsizer.h>

#include "blackfisk.h"
#include "BFCore.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFRootTask.h"
#include "BFIconTable.h"
#include "BFDestinationCtrl.h"
#include "BFPlaceholderButton.h"
#include "BFHelpCtrl.h"
#include "BFExcludeCtrl.h"
#include "ids.h"

#define BFTASKDLG_ID_BUTTONOK       1 + BF_TASKDLG_ID_HIGHEST
#define BFTASKDLG_ID_BUTTONCANCEL   2 + BF_TASKDLG_ID_HIGHEST
#define BFTASKDGL_ID_CBTYPE         3 + BF_TASKDLG_ID_HIGHEST
#define BF_TASKDLG_CBVERIFY         4 + BF_TASKDLG_ID_HIGHEST

BEGIN_EVENT_TABLE(BFTaskDlg, wxDialog)
  EVT_CLOSE   (BFTaskDlg::OnClose)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONOK,       BFTaskDlg::OnButton_Ok)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONCANCEL,   BFTaskDlg::OnButton_Cancel)
  EVT_COMBOBOX(BFTASKDGL_ID_CBTYPE,         BFTaskDlg::OnCombo_TypeSelect)
  EVT_CHECKBOX(BF_TASKDLG_CBVERIFY,         BFTaskDlg::OnCheckBox_Verify)
END_EVENT_TABLE()

/*static*/ const long BFTaskDlg::lWidth1_ = 95;
/*static*/ const long BFTaskDlg::lWidth2_ = 280;

BFTaskDlg::BFTaskDlg (wxWindow* pParent,
                      BFTask* pTask)
         : wxDialog (pParent,
                     wxID_ANY,
                     pTask->GetTypeDescription()),
           pTask_(pTask),
           pTypeCtrl_(NULL),
           pNameCtrl_(NULL),
           pSourceCtrl_(NULL),
           pDestCtrl_(NULL),
           pVerifyCheck_(NULL),
           pVerifyContentCheck_(NULL),
           pExcludeCtrl_(NULL)
{
    // help ctrl
    pHelpCtrl_ = new BFHelpCtrl(this);

    // arrange and create controls
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    pSizer->Add (CreateBook(),      wxSizerFlags(0).Center());
    pSizer->Add (pHelpCtrl_,        wxSizerFlags(0).Center().Border().Expand());
    pSizer->Add (CreateButtons(),   wxSizerFlags(0).Center().Border(wxALL, 10));
    SetSizerAndFit(pSizer);

    // init the dialog with data
    GetData();

    // display the dialog
    wxDialog::Show();
    // disable the mainframe to simulate a modal dialog
    BFMainFrame::Instance()->Disable();

    /* if the dialog is shown modal it occures
       in a race condition somewhere in the wx-code
       so we need to simulate a modal dialog */

    Fit();
}

/*virtual*/ BFTaskDlg::~BFTaskDlg ()
{
}

wxBookCtrlBase* BFTaskDlg::CreateBook ()
{
    wxNotebook* pBook = new wxNotebook(this, wxID_ANY);

    pBook->AddPage(CreateBookPageA(pBook), _("General"), true);
    pBook->AddPage(CreateBookPageB(pBook), _("Advanced"), false);

    return pBook;
}

wxWindow* BFTaskDlg::CreateBookPageA (wxWindow* pParent)
{
    wxPanel* pPanel = new wxPanel(pParent, wxID_ANY);

    // type
    wxStaticText* pTypeStatic = new wxStaticText(pPanel, -1, _("type:"));
    pTypeCtrl_ = new wxBitmapComboBox(pPanel,
                                      BFTASKDGL_ID_CBTYPE,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      0,
                                      NULL,
                                      wxCB_READONLY);
    pHelpCtrl_->Connect(pTypeStatic, pTypeCtrl_, _("This is the type of the backup task."));

    // name
    wxPanel*        pNamePanel      = new wxPanel(pPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                    pNameCtrl_      = new wxTextCtrl(pNamePanel, -1);
    wxButton*       pPlaceholderButton    = new BFPlaceholderButton(pNamePanel, *pNameCtrl_);
    wxStaticText*   pNameStatic     = new wxStaticText(pPanel, -1, _("destination name:"));
    pNameStatic ->SetMinSize(wxSize(BFTaskDlg::lWidth1_, pNameStatic->GetSize().GetHeight()));
    pNamePanel  ->SetMinSize(wxSize(BFTaskDlg::lWidth2_, pNamePanel->GetSize().GetHeight()+3));
    wxBoxSizer* pNameSubSizer   = new wxBoxSizer(wxHORIZONTAL);
    pNameSubSizer->Add(pNameCtrl_,          wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));
    pNameSubSizer->Add(pPlaceholderButton,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNamePanel->SetSizer(pNameSubSizer);
    pHelpCtrl_->Connect(pNameStatic, pNameCtrl_, _("The name of the Task. It is the same as the destination."));

    // source
    pSourceCtrl_ = new wxTextCtrl(pPanel, -1);
    wxStaticText* pSourceStatic   = new wxStaticText(pPanel, -1, _("source:"));
    pSourceCtrl_->Disable();
    SetRowSize(pSourceStatic, pSourceCtrl_);
    pHelpCtrl_->Connect(pSourceCtrl_, pSourceStatic, _("This is the directory or file that should be backuped."));

    // destination
    wxStaticText* pDestStatic = new wxStaticText(pPanel, -1, _("destination path:"));
    pDestCtrl_ = new BFDestinationCtrl(pPanel, wxEmptyString, false);
    pDestCtrl_->ConnectHelpText(pHelpCtrl_, _("This is the path where the backup is stored."));

    // sizer and arrange
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
    wxGridSizer* pBodySizer     = new wxFlexGridSizer(2);
    pBodySizer->SetVGap(5);
    pBodySizer->Add(pTypeStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pTypeCtrl_,     wxSizerFlags(0).Expand());
    pBodySizer->Add(pNameStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pNamePanel);
    pBodySizer->Add(pSourceStatic,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pSourceCtrl_);
    pBodySizer->Add(pDestStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pDestCtrl_);
    pSizer->Add(pBodySizer, wxSizerFlags(0).Border());
    pPanel->SetSizer(pSizer);

    return pPanel;
}

wxWindow* BFTaskDlg::CreateBookPageB (wxWindow* pParent)
{
    wxPanel* pPanel = new wxPanel(pParent, wxID_ANY);

    // verify
    pVerifyCheck_ = new wxCheckBox(pPanel, BF_TASKDLG_CBVERIFY, wxEmptyString);
    wxStaticText* pVerifyStatic   = new wxStaticText(pPanel, -1, _("verify:"));
    SetRowSize(pVerifyStatic, pVerifyCheck_);
    pHelpCtrl_->Connect(pVerifyStatic, pVerifyCheck_,
                        _("If checked the backup is verified.\nThe source and the destination "\
                          "are compared by size, modification time and attributes.\n" \
                          "Of course it cost some time!"));

    // verify content
    pVerifyContentCheck_ = new wxCheckBox(pPanel, -1, wxEmptyString);
    wxStaticText* pVerifyContentStatic = new wxStaticText(pPanel, -1, _("verify content:"));
    SetRowSize(pVerifyContentStatic, pVerifyContentCheck_);
    pHelpCtrl_->Connect(pVerifyContentStatic, pVerifyContentCheck_,
                        _("If cheched the backup is verified including a byte-by-byte comparison.\n" \
                          "This will cost a lot of time!"));

    // exclude
    wxStaticText* pExcludeStatic = NULL;
    if (pTask_->GetType() != TaskFILECOPY)
    {
        pExcludeStatic = new wxStaticText(pPanel, -1, _("Exclude this directories:\nTo delete a entry double-click on it."));
        pExcludeCtrl_ = new BFExcludeCtrl(pPanel, pTask_);
        pHelpCtrl_->Connect(pExcludeStatic, pExcludeCtrl_,
                            _("Here you can exclude directories from the backup. You can use wildcard-masks, too."));
    }

    // sizer and arrange
    wxBoxSizer*     pSizer      = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* pBodySizer  = new wxGridBagSizer(5);
    pBodySizer->Add(pVerifyStatic,          wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL);
    pBodySizer->Add(pVerifyCheck_,          wxGBPosition(0, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL);
    pBodySizer->Add(pVerifyContentStatic,   wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL);
    pBodySizer->Add(pVerifyContentCheck_,   wxGBPosition(1, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL);
    if (pExcludeCtrl_)
    {
        pBodySizer->Add(pExcludeStatic,         wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL);
        pBodySizer->Add(pExcludeCtrl_,          wxGBPosition(3, 0), wxGBSpan(2, 2), wxALIGN_CENTER | wxEXPAND);
    }
    pSizer->Add(pBodySizer, wxSizerFlags(0).Border());

    pPanel->SetSizer(pSizer);

    return pPanel;
}

wxSizer* BFTaskDlg::CreateButtons ()
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);

    pSizer->Add(new wxButton(this, BFTASKDLG_ID_BUTTONOK,     "&OK"),       wxSizerFlags(0));
    pSizer->AddSpacer(20);
    pSizer->Add(new wxButton(this, BFTASKDLG_ID_BUTTONCANCEL, _("&Cancel")),   wxSizerFlags(0));

    return pSizer;
}

void BFTaskDlg::OnClose(wxCloseEvent& rEvent)
{
    // is it a unsaved/new created task?
    if ( BFRootTask::Instance().HasTask(pTask_->GetOID()) == false )
        delete pTask_;

    BFMainFrame::Instance()->Enable();
    Destroy();
}

/*static*/ void BFTaskDlg::SetRowSize (wxWindow* pCol1, wxWindow* pCol2)
{
    if (pCol1 == NULL || pCol2 == NULL)
        return;

    pCol1->SetMinSize(wxSize(lWidth1_, pCol1->GetSize().GetHeight()));
    pCol2->SetMinSize(wxSize(lWidth2_, pCol2->GetSize().GetHeight()));
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
    Close();
}

void BFTaskDlg::OnCombo_TypeSelect (wxCommandEvent& rEvent)
{
    BFTypeVector vecTaskTypes;

    pTask_->GetAvailableTypes(vecTaskTypes);
    BFTaskType tt = vecTaskTypes[pTypeCtrl_->GetSelection()];

    if (tt != pTask_->GetType())
    {
        pTask_->SetTaskType(tt);
        BFRootTask::Instance().SetModified();
    }

    InitTypeCtrl();
}

void BFTaskDlg::OnCheckBox_Verify (wxCommandEvent& rEvent)
{
    if (pVerifyCheck_->GetValue())
    {
        pVerifyContentCheck_->Enable(true);
    }
    else
    {
        pVerifyContentCheck_->SetValue(false);
        pVerifyContentCheck_->Enable(false);
    }
}

void BFTaskDlg::InitTypeCtrl ()
{
    BFTypeVector vecTaskTypes;

    pTypeCtrl_->Clear();
    pTask_->GetAvailableTypes(vecTaskTypes);

    for (BFTypeVector::iterator it = vecTaskTypes.begin();
         it != vecTaskTypes.end();
         ++it)
    {
        BFTaskType type = (*it);
        pTypeCtrl_->Append(BFTask::GetTypeDescription(type),
                           BFIconTable::Instance()->GetBitmap(BFTask::GetTypeIconId(type)));
    }

    pTypeCtrl_->Select(0);
}

void BFTaskDlg::GetData ()
{
    InitTypeCtrl();
    pNameCtrl_      ->SetValue(pTask_->GetName());
    pSourceCtrl_    ->SetValue(pTask_->GetSource());
    pDestCtrl_      ->SetPath(pTask_->GetDestination());
    pVerifyCheck_   ->SetValue(pTask_->Verify());
    pVerifyContentCheck_->SetValue(pTask_->VerifyContent());
    if (pExcludeCtrl_)
        pExcludeCtrl_   ->GetData(pTask_->GetExclude());

    wxCommandEvent event;
    OnCheckBox_Verify(event);
}

bool BFTaskDlg::IsPlausible ()
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

void BFTaskDlg::SetData ()
{
    // type
    // is set by OnCombo_TypeSelect()

    // name
    if (pNameCtrl_->IsModified())
    {
        pTask_->SetName(pNameCtrl_->GetValue());
        BFRootTask::Instance().SetModified();
        pNameCtrl_->DiscardEdits();
    }

    // source
    if (pSourceCtrl_->IsModified())
    {
        pTask_->SetSource(pSourceCtrl_->GetValue());
        BFRootTask::Instance().SetModified();
        pSourceCtrl_->DiscardEdits();
    }

    // destination
    if (pDestCtrl_->GetPath() != pTask_->GetDestination())
    {
        pTask_->SetDestination(pDestCtrl_->GetPath());
        BFRootTask::Instance().SetModified();
    }

    // verify
    if (pVerifyCheck_->GetValue() != pTask_->Verify())
    {
        pTask_->SetVerify(pVerifyCheck_->GetValue());
        BFRootTask::Instance().SetModified();
    }

    // verify content
    if (pVerifyContentCheck_->GetValue() != pTask_->VerifyContent())
    {
        pTask_->SetVerifyContent(pVerifyContentCheck_->GetValue());
        BFRootTask::Instance().SetModified();
    }

    // exclude list
    if (pExcludeCtrl_)
    {
        wxArrayString arrExclude;
        pExcludeCtrl_->SetData(arrExclude);
        if (arrExclude != pTask_->GetExclude())
        {
            pTask_->SetExclude(arrExclude);
            BFRootTask::Instance().SetModified();
        }
    }

    // add task if needed
    if ( BFRootTask::Instance().HasTask(pTask_->GetOID()) == false )
    {
        BFRootTask::Instance().AppendTask(*pTask_);
        BFRootTask::Instance().broadcastObservers();
    }
    else
    {
        if ( BFRootTask::Instance().IsModified() )
            BFRootTask::Instance().broadcastObservers();
    }
}


/*static*/ void BFTaskDlg::Show (BFTask* pTask)
{
    if (pTask == NULL)
        return;

    new BFTaskDlg(BFMainFrame::Instance(), pTask);
}
