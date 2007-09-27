/**
 * Name:        BFDirCtrl.cpp
 * Purpose:     BFDirCtrl class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-24
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

#include "BFDirCtrl.h"

#include <wx/dnd.h>

#include "BFBackupTree.h"
#include "BFMainFrame.h"
#include "BFDestinationDlg.h"
#include "BFSettings.h"
#include "BFIconTable.h"
#include "ctrlids.h"

BEGIN_EVENT_TABLE(BFDirCtrl, wxPanel)
  EVT_TOGGLEBUTTON (BFDIRCTRL_ID_FILEBUTTON,    BFDirCtrl::OnButton_DirCtrl)
  EVT_MENU    (BFDIRCTRL_ID_ADDDESTINATION,     BFDirCtrl::OnDestination)
  EVT_MENU    (BFDIRCTRL_ID_CREATEDESTINATION,  BFDirCtrl::OnDestination)
  EVT_MENU    (BFDIRCTRL_ID_BACKUP_FILECOPY,    BFDirCtrl::OnBackup)
  EVT_MENU    (BFDIRCTRL_ID_BACKUP_DIRCOPY,     BFDirCtrl::OnBackup)
  EVT_MENU    (BFDIRCTRL_ID_BACKUP_SYNCDIR,     BFDirCtrl::OnBackup)
  EVT_MENU    (BFDIRCTRL_ID_ARCHIVEDIR,         BFDirCtrl::OnBackup)
END_EVENT_TABLE()

//
BFDirCtrl::BFDirCtrl (wxWindow* pParent)
         : wxPanel(pParent, -1)
{
    // DirCtrl
    pDirCtrl_ = new wxGenericDirCtrl(this);
    if (BFSettings::Instance().GetWithFiles())
        pDirCtrl_->SetWindowStyle(pDirCtrl_->GetWindowStyle() & ~wxDIRCTRL_DIR_ONLY);
    else
        pDirCtrl_->SetWindowStyle(pDirCtrl_->GetWindowStyle() | wxDIRCTRL_DIR_ONLY);

    // button to show files in the DirCtrl
    pButtonFiles_ = new wxToggleButton(this, BFDIRCTRL_ID_FILEBUTTON, _("files"));
    pButtonFiles_->SetValue(BFSettings::Instance().GetWithFiles());

    // sizer
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

    // arrange
    pTopSizer->Add ( pButtonFiles_, wxSizerFlags(0).Center().Border(wxUP | wxDOWN, 5) );
    pTopSizer->Add ( pDirCtrl_, wxSizerFlags(1).Expand() );
    SetSizer(pTopSizer);

    // install handlers
    pDirCtrl_->GetTreeCtrl()->Connect
    (
        wxID_ANY,
        wxEVT_COMMAND_TREE_BEGIN_DRAG,
        wxTreeEventHandler (BFDirCtrl::OnBeginDrag),
        NULL,
        this
    );

    pDirCtrl_->GetTreeCtrl()->Connect
    (
        wxID_ANY,
        wxEVT_COMMAND_TREE_ITEM_MENU,
        wxTreeEventHandler (BFDirCtrl::OnItemMenu),
        NULL,
        this
    );
}


/*virtual*/ BFDirCtrl::~BFDirCtrl ()
{
}


void BFDirCtrl::OnButton_DirCtrl (wxCommandEvent& rEvent)
{
    long style = pDirCtrl_->GetWindowStyle();

    if (pButtonFiles_->GetValue())
        style = style & ~wxDIRCTRL_DIR_ONLY;
    else
        style = style | wxDIRCTRL_DIR_ONLY;

    pDirCtrl_->SetWindowStyle(style);
    wxString strPath(pDirCtrl_->GetPath());
    pDirCtrl_->CollapsePath(strPath);
    pDirCtrl_->ExpandPath(strPath);
}


void BFDirCtrl::OnBeginDrag (wxTreeEvent& event)
{
    wxFileDataObject    my_data;
    wxDropSource        dragSource  ( this );

    // selected the currently draging item
    pDirCtrl_->GetTreeCtrl()->SelectItem(event.GetItem());
    my_data.AddFile(pDirCtrl_->GetPath());
    dragSource.SetData(my_data);

    wxDragResult result = dragSource.DoDragDrop( TRUE );
}


void BFDirCtrl::OnItemMenu (wxTreeEvent& event)
{
    wxMenuItem* pItem;

    // remember the item
    lastItemId_ = event.GetItem();

    // select the current item
    pDirCtrl_->GetTreeCtrl()->SelectItem(event.GetItem());

    // create a menu object
    wxMenu menu;
    wxMenu menu_backup;

    // ** backup menu **
    menu.AppendSubMenu(&menu_backup, _("backup"));
    menu.AppendSeparator();

    // get selected item in the dir control
    wxDirItemData* pDirItem = dynamic_cast<wxDirItemData*>(pDirCtrl_->GetTreeCtrl()->GetItemData(event.GetItem()));

    // check if it is a dir
    if ( pDirItem != NULL && pDirItem->m_isDir )
    {
        // ** add destination **
        menu.Append(BFDIRCTRL_ID_ADDDESTINATION, _("add as destination directory"));

        // ** create destination **
        menu.Append(BFDIRCTRL_ID_CREATEDESTINATION, _("create here a destination directory"));

        // ** backup dir copy **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_BACKUP_DIRCOPY, _("copy directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu_backup.Append(pItem);

        // ** backup sync dir **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_BACKUP_SYNCDIR, _("synchronize directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu_backup.Append(pItem);

        // ** backup archive dir **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_ARCHIVEDIR, _("archive/compress directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu_backup.Append(pItem);
    }
    else
    {
        // ** backup file copy **
        menu_backup.Append(BFDIRCTRL_ID_BACKUP_FILECOPY, _("file copy"));
    }

    // popup the menu
    if (menu.GetMenuItemCount() > 0)
        pDirCtrl_->PopupMenu(&menu, event.GetPoint());
}

void BFDirCtrl::OnDestination (wxCommandEvent& event)
{
    // get selected item in the dir control
    wxDirItemData* pDirItem = dynamic_cast<wxDirItemData*>(pDirCtrl_->GetTreeCtrl()->GetItemData(lastItemId_));

    if (pDirItem == NULL)
        return;

    // check if it is a dir
    if ( !(pDirItem->m_isDir) )
    {
        if (event.GetId() == BFDIRCTRL_ID_ADDDESTINATION)
        {
            BFSystem::Fatal(_T("It is not possible to add a file as a destination!\nYou can just add a directory as a backup destination."), _T("BFDirCtrl::OnDestination"));
            return;
        }

        if (event.GetId() == BFDIRCTRL_ID_CREATEDESTINATION)
        {
            BFSystem::Fatal(_T("It is not possible to create a destination here!\nIt is just a file."), _T("BFDirCtrl::OnDestination"));
            return;
        }

    }

    // get the backup tree object
    BFBackupTree* pBackupTree = BFMainFrame::Instance()->BackupTree();

    if (pBackupTree == NULL)
    {
        BFSystem::Fatal(_("no backup tree available (pBackupTree == NULL)"), _T("BFDirCtrl::OnDestination"));
        return;
    }

    switch (event.GetId())
    {
        case BFDIRCTRL_ID_ADDDESTINATION:
            // set and select the new added destination
            pBackupTree->SelectItem(pBackupTree->AddDestination(pDirItem->m_path));
            break;

        case BFDIRCTRL_ID_CREATEDESTINATION:
            // open the dialog
            new BFDestinationDlg(BFMainFrame::Instance(), pDirItem->m_path);
            break;

        default:
            BFSystem::Fatal(_T("unknown event"), _T("BFDirCtrl::OnDestination"));
            break;
    }
}

void BFDirCtrl::OnBackup (wxCommandEvent& event)
{
    wxDirItemData* pDirItem = dynamic_cast<wxDirItemData*>(pDirCtrl_->GetTreeCtrl()->GetItemData(lastItemId_));

    if (pDirItem == NULL)
        return;

    switch (event.GetId())
    {
        case BFDIRCTRL_ID_BACKUP_FILECOPY:
        case BFDIRCTRL_ID_BACKUP_DIRCOPY:
        case BFDIRCTRL_ID_BACKUP_SYNCDIR:
        case BFDIRCTRL_ID_ARCHIVEDIR:
            BFMainFrame::Instance()->BackupTree()->SetDropedFilename (pDirItem->m_path);
            BFMainFrame::Instance()->BackupTree()->OnCreateBackup (event);
            break;

        default:
            BFSystem::Fatal(_T("no matching menu ID"), _T("BFDirCtrl::OnBackup"));
            break;
    };
}
