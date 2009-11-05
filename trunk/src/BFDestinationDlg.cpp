/**
 * Name:        BFDestinationDlg.cpp
 * Purpose:     BFDestinationDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-28
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

#include "BFDestinationDlg.h"

#include "BFDestinationCtrl.h"
#include "BFSystem.h"
#include "BFIconTable.h"
#include "BFBitmapButton.h"
#include "BFMainFrame.h"
#include "BFBackupTree.h"
#include "BFBackup.h"
#include "BFCore.h"

BEGIN_EVENT_TABLE(BFDestinationDlg, wxDialog)
  EVT_CLOSE   (BFDestinationDlg::OnClose)
  EVT_BUTTON  (BFDESTINATIONDLG_ID_BUTTONOK,        BFDestinationDlg::OnButton_Ok)
  EVT_BUTTON  (BFDESTINATIONDLG_ID_BUTTONCANCEL,    BFDestinationDlg::OnButton_Cancel)
END_EVENT_TABLE()

//
BFDestinationDlg::BFDestinationDlg (wxWindow* pParent,
                                    const wxChar* strPath,
                                    BF_DestinationAction action)
                : wxDialog(pParent, wxID_ANY, wxEmptyString),
                  pDestCtrl_(NULL),
                  action_(action),
                  strInitialPath_(strPath)
{
    // caption
    if (action_ == add_destination)
        SetTitle(_("create a destination"));

    if (action_ == modify_destination)
        SetTitle(_("modify a destination"));

    // dialog icon
    SetIcon(BFIconTable::Instance()->GetIcon(BFIconTable::folder));

    // destination control
    pDestCtrl_ = new BFDestinationCtrl(this, strPath);

    // buttons
    BFBitmapButton* pButtonOk     = new BFBitmapButton(this,
                                                       BFDESTINATIONDLG_ID_BUTTONOK,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::ok),
                                                       "OK");
    BFBitmapButton* pButtonCancel = new BFBitmapButton(this,
                                                       BFDESTINATIONDLG_ID_BUTTONCANCEL,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::cancel),
                                                       _("Cancel"));

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

	// get the path
	wxString strPath = pDestCtrl_->GetPath();

	// valid path?
	if ( (!BFCore::IsPathValid(strPath)) )
	{
		BFSystem::Error ( wxString::Format ( _("The path %s is not valid!"), strPath ) );
		return;
	}

    // add and select the destination dir
    if (action_ == add_destination)
    {
        pBackupTree->SelectItem(pBackupTree->AddDestination(pDestCtrl_->GetPath()));
    }

    // modify the destination and rebuild the tree
    if (action_ == modify_destination)
    {
        BFBackup::Instance().ModifyDestination(strInitialPath_, pDestCtrl_->GetPath());
        pBackupTree->Init();
    }

    Close();
}

void BFDestinationDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}
