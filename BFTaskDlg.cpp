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

#include "blackfisk.h"
#include "BFCore.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFRootTask.h"
#include "BFIconTable.h"
#include "BFDestinationCtrl.h"
#include "BFPlaceholderButton.h"

BEGIN_EVENT_TABLE(BFTaskDlg, wxDialog)
  EVT_CLOSE   (BFTaskDlg::OnClose)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONOK,       BFTaskDlg::OnButton_Ok)
  EVT_BUTTON  (BFTASKDLG_ID_BUTTONCANCEL,   BFTaskDlg::OnButton_Cancel)
  EVT_COMBOBOX(BFTASKDGL_ID_CBTYPE,         BFTaskDlg::OnCombo_TypeSelect)
END_EVENT_TABLE()

/*static*/ const long BFTaskDlg::lWidth1_ = 95;
/*static*/ const long BFTaskDlg::lWidth2_ = 280;

BFTaskDlg::BFTaskDlg (wxWindow* pParent,
                      BFTask& rTask)
         : wxDialog (pParent,
                     wxID_ANY,
                     rTask.GetTypeDescription()),
           rTask_(rTask),
           pNameCtrl_(NULL),
           pSourceCtrl_(NULL),
           pDestCtrl_(NULL),
           pVerifyCheck_(NULL),
           pExcludeCtrl_(NULL)
{
    // arrange and create controls
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add (CreateControls(),  wxSizerFlags(0).Border(wxALL, 10).Center());
    pMainSizer->Add (CreateButtons(),   wxSizerFlags(0).Border().Center());
    SetSizerAndFit(pMainSizer);

    // init the dialog with data
    GetData();

    // display the dialog
    ShowModal();
}

/*virtual*/ BFTaskDlg::~BFTaskDlg ()
{
}

wxSizer* BFTaskDlg::CreateControls ()
{
    // type
    wxStaticText* pTypeStatic = new wxStaticText(this, -1, _("type:"));
    pTypeCtrl_ = new wxBitmapComboBox(this,
                                      BFTASKDGL_ID_CBTYPE,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      0,
                                      NULL,
                                      wxCB_READONLY);

    // name
    wxPanel*        pNamePanel      = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
                    pNameCtrl_      = new wxTextCtrl(pNamePanel, -1);
    wxButton*       pPlaceholderButton    = new BFPlaceholderButton(pNamePanel, *pNameCtrl_);
    wxStaticText*   pNameStatic     = new wxStaticText(this, -1, _("destination name:"));
    pNameStatic ->SetMinSize(wxSize(BFTaskDlg::lWidth1_, pNameStatic->GetSize().GetHeight()));
    pNamePanel  ->SetMinSize(wxSize(BFTaskDlg::lWidth2_, pNamePanel->GetSize().GetHeight()+3));
    wxBoxSizer* pNameSubSizer   = new wxBoxSizer(wxHORIZONTAL);
    pNameSubSizer->Add(pNameCtrl_,          wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));
    pNameSubSizer->Add(pPlaceholderButton,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pNamePanel->SetSizer(pNameSubSizer);

    // source
    pSourceCtrl_ = new wxTextCtrl(this, -1);
    wxStaticText* pSourceStatic   = new wxStaticText(this, -1, _("source:"));
    pSourceCtrl_->Disable();
    SetRowSize(pSourceStatic, pSourceCtrl_);

    // destination
    wxStaticText* pDestStatic = new wxStaticText(this, -1, _("destination path:"));
    pDestCtrl_ = new BFDestinationCtrl(this, wxEmptyString, false);

    // verify
    pVerifyCheck_ = new wxCheckBox(this, -1, wxEmptyString);
    wxStaticText* pVerifyStatic   = new wxStaticText(this, -1, _("verify:"));
    SetRowSize(pVerifyStatic, pVerifyCheck_);

    // sizer and arrange
    wxGridSizer* pBodySizer     = new wxFlexGridSizer(2);
    pBodySizer->SetVGap(5);
    pBodySizer->Add(pTypeStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pTypeCtrl_,     wxSizerFlags(0).Expand());
/* DEBUG
    pBodySizer->AddStretchSpacer();
    wxPanel* pP = new wxPanel(this);
    wxTextCtrl* pT = new wxTextCtrl(pP, wxID_ANY, _T("xxx"));
    wxBoxSizer* pS = new wxBoxSizer(wxHORIZONTAL);
    pS->Add(pT);
    pP->SetSizer(pS);
    pBodySizer->Add(pP);
// DEBUG off */
    pBodySizer->Add(pNameStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pNamePanel);
    pBodySizer->Add(pSourceStatic,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pSourceCtrl_);
    pBodySizer->Add(pDestStatic,    wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pDestCtrl_);
    pBodySizer->Add(pVerifyStatic,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pBodySizer->Add(pVerifyCheck_,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));

    return pBodySizer;
}

wxSizer* BFTaskDlg::CreateButtons ()
{
    wxButton* pButtonOk     = new wxButton(this, BFTASKDLG_ID_BUTTONOK, _("&OK"));
    wxButton* pButtonCancel = new wxButton(this, BFTASKDLG_ID_BUTTONCANCEL, _("&Cancel"));

    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);

    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());

    return pButtonSizer;
}

void BFTaskDlg::OnClose(wxCloseEvent& rEvent)
{
    // is it a unsaved/new created task?
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        delete &rTask_;

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

    rTask_.GetAvailableTypes(vecTaskTypes);
    BFTaskType tt = vecTaskTypes[pTypeCtrl_->GetSelection()];
    rTask_.SetTaskType(tt);

    InitTypeCtrl();
}

void BFTaskDlg::InitTypeCtrl ()
{
    BFTypeVector vecTaskTypes;

    pTypeCtrl_->Clear();
    rTask_.GetAvailableTypes(vecTaskTypes);

    for (BFTypeVector::iterator it = vecTaskTypes.begin();
         it != vecTaskTypes.end();
         it++)
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
    pNameCtrl_      ->SetValue(rTask_.GetName());
    pSourceCtrl_    ->SetValue(rTask_.GetSource());
    pDestCtrl_      ->SetPath(rTask_.GetDestination());
    pVerifyCheck_   ->SetValue(rTask_.Verify());
    //pExcludeList_   ->Clear();
    //pExcludeList_   ->Set(rTask_.GetExclude());
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
    // name
    if (pNameCtrl_->IsModified())
    {
        rTask_.SetName(pNameCtrl_->GetValue());
        BFRootTask::Instance().SetModified();
        pNameCtrl_->DiscardEdits();
    }

    // source
    if (pSourceCtrl_->IsModified())
    {
        rTask_.SetSource(pSourceCtrl_->GetValue());
        BFRootTask::Instance().SetModified();
        pSourceCtrl_->DiscardEdits();
    }

    // destination
    if (pDestCtrl_->GetPath() != rTask_.GetDestination())
    {
        rTask_.SetDestination(pDestCtrl_->GetPath());
        BFRootTask::Instance().SetModified();
    }

    // verify
    if (pVerifyCheck_->GetValue() != rTask_.Verify())
    {
        rTask_.SetVerify(pVerifyCheck_->GetValue());
        BFRootTask::Instance().SetModified();
    }

    // exclude list
/*    wxArrayString arrExclude = pExcludeList_->GetStrings();

    if (arrExclude != rTask_.GetExclude())
        rTask_.SetExclude(arrExclude);
*/
    // add task if needed
    if ( BFRootTask::Instance().HasTask(rTask_.GetOID()) == false )
        BFRootTask::Instance().AppendTask(rTask_);
    else
    {
        BFRootTask::Instance().broadcastObservers();
    }
}


/*static*/ void BFTaskDlg::Show (BFTask* pTask)
{
    if (pTask == NULL)
        return;

    new BFTaskDlg(BFMainFrame::Instance(), *pTask);
}
