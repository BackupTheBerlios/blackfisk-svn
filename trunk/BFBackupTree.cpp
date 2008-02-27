/**
 * Name:        BFBackupTree.cpp
 * Purpose:     BFBackupTree and BFBackupCtrl class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-25
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

#include "BFBackupTree.h"

#include <wx/icon.h>
#include <wx/dnd.h>
#include <wx/tokenzr.h>
#include <wx/image.h>
#include <wx/volume.h>

#include "blackfisk.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFTask.h"
#include "BFTaskBase.h"
#include "BFRootTask.h"
#include "BFRootTaskApp.h"
#include "BFTaskDlg.h"
#include "BFIconTable.h"
#include "BFDestinationDlg.h"
#include "BFSettings.h"
#include "ctrlids.h"

BEGIN_EVENT_TABLE(BFBackupTree, wxTreeCtrl)
    EVT_TREE_ITEM_ACTIVATED     (wxID_ANY,                          BFBackupTree::OnItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK   (wxID_ANY,                          BFBackupTree::OnItemMenu)
    EVT_MENU                    (BFBACKUPCTRL_ID_ADDDESTINATION,    BFBackupTree::OnAddDestination)
    EVT_MENU                    (BFBACKUPCTRL_ID_PROJECTSETTINGS,   BFBackupTree::OnProjectSettings)
    EVT_MENU                    (BFBACKUPCTRL_ID_CREATEDESTINATION, BFBackupTree::OnCreateDestination)
    EVT_MENU                    (BFBACKUPCTRL_ID_TASKSETTINGS,      BFBackupTree::OnTaskSettings)
    EVT_MENU                    (BFBACKUPCTRL_ID_DELETETASK,        BFBackupTree::OnDeleteTask)
    EVT_MENU                    (BFBACKUPCTRL_ID_COPY_DIR,          BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_COPY_FILE,         BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_SYNC_DIR,          BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_ARCHIVE_DIR,       BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_MODIFYDDESTINATION,BFBackupTree::OnModifyDestination)
    EVT_TREE_BEGIN_LABEL_EDIT   (wxID_ANY,                          BFBackupTree::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT     (wxID_ANY,                          BFBackupTree::OnEndLabelEdit)
    EVT_MENU                    (BF_BACKUPTREE_REBUILD,             BFBackupTree::OnRebuild)
    EVT_TREE_BEGIN_DRAG         (wxID_ANY,                          BFBackupTree::OnBeginDrag)
END_EVENT_TABLE()


//
BFBackupTree::BFBackupTree (wxWindow* pParent)
            : wxTreeCtrl(pParent,
                         -1,
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS),
              Observer(&(BFRootTask::Instance())),
              bFillBlackfiskPlaceholders_(BFSettings::Instance().GetFillBlackfiskPlaceholders()),
              oidCurrentDrag_(BFInvalidOID)
{
    SetImageList ( BFIconTable::Instance() );

    SetDropTarget   ( new BFBackupDropTarget(this) );

    Init();
}


//
/*virtual*/ BFBackupTree::~BFBackupTree ()
{
}

void BFBackupTree::OnRebuild (wxCommandEvent& rEvent)
{
    Init();
}

void BFBackupTree::Init ()
{
    lastItemId_ = wxTreeItemId();

    Freeze();

    // recreate the treeCtrl with all tasks
    BFRootTaskApp::Instance().InitThat(this);

    // expand all items in the treeCtlr
    ExpandAll();
    SelectItem(lastItemId_);

    SetToolTip(_("files and directories for the backup"));

    Thaw();
}

void BFBackupTree::SetDropedFilename (wxString strDropedFilename)
{
    strDropedFilename_ = strDropedFilename;
}

void BFBackupTree::OnItemActivated(wxTreeEvent& rEvent)
{
    // remember this itemId
    lastItemId_ = rEvent.GetItem();

    // root?
    if (lastItemId_ == GetRootItem())
    {
        BFMainFrame::Instance()->OpenProjectSettings();
    }
    else
    {
        ShowTaskSettings (lastItemId_);
    }
}

void BFBackupTree::ShowTaskSettings (wxTreeItemId id)
{
    // get the task object from the data layer
    BFTask* pTask = GetTaskByItem(id);

    if (pTask != NULL)
        BFTaskDlg::Show (pTask);
}


/*static*/ wxMenu& BFBackupTree::GenerateBackupMenu (wxMenu& menu, bool bOnDir)
{
    wxMenuItem      *pItem          = NULL;

    // ** backup menu **
    //menu.AppendSubMenu(pMenu_backup, _("backup"));

    // check if it is a dir
    if ( bOnDir )
    {
        // ** backup dir copy **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_BACKUP_DIRCOPY, _("copy directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu.Append(pItem);

        // ** backup sync dir **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_BACKUP_SYNCDIR, _("synchronize directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu.Append(pItem);

        // ** backup archive dir **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_ARCHIVEDIR, _("archive/compress directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu.Append(pItem);
    }
    else
    {
        // ** backup file copy **
        pItem = new wxMenuItem(&menu, BFDIRCTRL_ID_BACKUP_FILECOPY, _("copy file"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_filecopy));
        menu.Append(pItem);
    }

    return menu;
}


void BFBackupTree::OnItemMenu(wxTreeEvent& rEvent)
{
    wxPoint         point   (rEvent.GetPoint());
    wxMenu          menu, menu_backup;

    // remember the selected item
    lastItemId_ = rEvent.GetItem();

    if ( IsTask(lastItemId_) )
    {
        BFTask* pTask = GetTaskByItem(lastItemId_);

        if (pTask == NULL)
            return;

        // ** create destination directory **
        if (pTask->GetType() == TaskDIRCOPY)
            menu.Append(BFBACKUPCTRL_ID_CREATEDESTINATION, _("create destination directory"));

        // ** delete task **
        menu.Append(BFBACKUPCTRL_ID_DELETETASK, _("delete task"));

        // separator
        menu.AppendSeparator();

        // ** backup **
        if (pTask->GetType() == TaskFILECOPY)
            BFBackupTree::GenerateBackupMenu(menu_backup, false);
        else
            BFBackupTree::GenerateBackupMenu(menu_backup, true);

        menu.AppendSubMenu(&menu_backup, _("backup"));

        // separator
        menu.AppendSeparator();

        // ** task settings **
        menu.Append(BFBACKUPCTRL_ID_TASKSETTINGS, _("Task settings"));
    }
    else
    {
        // for non-existing (while backup created) directories
        menu.Append(BFBACKUPCTRL_ID_CREATEDESTINATION, _("create destination directory"));

        // right click on root item
        if (GetRootItem() == lastItemId_)
        {
            // for real existing directories
            menu.Append(BFBACKUPCTRL_ID_ADDDESTINATION, _("add destination directory"));
            menu.Append(BFBACKUPCTRL_ID_DELETETASK, _("delete task"));
            menu.AppendSeparator();
            menu.Append(BFBACKUPCTRL_ID_PROJECTSETTINGS, _("Project settings"));
        }
        else
        {
            menu.Append(BFBACKUPCTRL_ID_MODIFYDDESTINATION, _("modify destination"));
            menu.Append(BFBACKUPCTRL_ID_DELETETASK, _("delete task"));
            menu.AppendSeparator();
            BFBackupTree::GenerateBackupMenu(menu_backup, true);
            menu.AppendSubMenu(&menu_backup, _("backup"));
        }
    }

    if (menu.GetMenuItemCount() > 0)
        PopupMenu(&menu, point);
}

/*virtual*/ wxDragResult BFBackupTree::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    wxTreeItemId itemId(HitTest(wxPoint(x, y)));

    // clean all selections
    UnselectAll();

    // is an item selected?
    if ( itemId.IsOk() )
    {
        // get the imageId of the selected item
        int iImageId(GetItemImage(itemId));

        // identify the item
        if (iImageId == BFIconTable::folder
         || iImageId == BFIconTable::folder_open
         || iImageId == BFIconTable::folder_virtual
         || iImageId == BFIconTable::folder_virtual_open
         || iImageId == BFIconTable::volume_harddisk
         || iImageId == BFIconTable::volume_cdrom
         || iImageId == BFIconTable::volume_floppy
         || iImageId == BFIconTable::volume_removable)
        {
            // select it if it is not
            if (GetSelection() != itemId)
                SelectItem( itemId );
        }
    }

    return def;
}

bool BFBackupTree::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (filenames.Count() == 0)
        return false;

    wxMenu                  menu;
    wxString                str;

    if ( wxFile::Exists(filenames[0]) )
        // ** copy file **
        BFBackupTree::GenerateBackupMenu(menu, false);
    else
        // ** directory (existing and unexisting/created while backup) **
        BFBackupTree::GenerateBackupMenu(menu, true);

    // remember the filename for use in other methodes
    SetDropedFilename(filenames[0]);

    // remember the destination directory (by draging over it) if ther is one
    BFBackupTreeItemData*   pItemData   = NULL;
    wxTreeItemId            itemId      = HitTest(wxPoint(x, y));

    if ( itemId.IsOk() )
        pItemData = (BFBackupTreeItemData*)GetItemData(itemId);

    if ( pItemData
      && pItemData->GetOID() == BFInvalidOID)
        strCurrentDestination_ = pItemData->GetPath();

    // show the menu
    PopupMenu(&menu, wxPoint(x, y));

    return true;
}

bool BFBackupTree::OnDropTask (wxCoord x, wxCoord y)
{
    // is there a valid oid ?
    if (oidCurrentDrag_ == BFInvalidOID)
        return false;

    // get the oid-related BFTask
    BFTask* pTask = BFRootTask::Instance().GetTask(oidCurrentDrag_);

    if (pTask == NULL)
        return false;

    // get the destination directory where the task was droped
    BFBackupTreeItemData*   pItemData   = NULL;
    wxTreeItemId            itemId      = HitTest(wxPoint(x, y));

    if ( itemId.IsOk() )
        pItemData = (BFBackupTreeItemData*)GetItemData(itemId);

    // set the new destination
    if ( pItemData && pItemData->GetOID() == BFInvalidOID)
    {
        pTask->SetDestination(pItemData->GetPath());
        BFRootTask::Instance().SetModified();
        BFRootTask::Instance().broadcastObservers();
    }

    oidCurrentDrag_ = BFInvalidOID;

    return true;
}

void BFBackupTree::OnBeginDrag (wxTreeEvent& event)
{
    // get data behind the item
    BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(event.GetItem()));

    if (pItemData == NULL)
        return;

    // selected the currently draging item
    SelectItem(event.GetItem());

    // init
    wxFileDataObject    my_data;
    wxDropSource        dragSource  ( this );

    // drag a task or a directory?
    if (pItemData->GetOID() == BFInvalidOID)
    {
        my_data.AddFile(pItemData->GetPath());
    }
    else
    {
        // remember the currently draged task
        BFTask* pTask = BFRootTask::Instance().GetTask(pItemData->GetOID());

        if (pTask == NULL)
            return;

        oidCurrentDrag_ = pTask->GetOID();

        // just set dummy data
        my_data.AddFile(_T("<oid>"));
    }

    // start dragging
    dragSource.SetData(my_data);
    wxDragResult result = dragSource.DoDragDrop( TRUE );
}


void BFBackupTree::OnProjectSettings (wxCommandEvent& rEvent)
{
    BFMainFrame::Instance()->OpenProjectSettings();
}


void BFBackupTree::OnAddDestination (wxCommandEvent& rEvent)
{
    // create dir dialog
    wxDirDialog dlg(this, _T("choose a destination directory"));

    // let the user choose a dir
    if (dlg.ShowModal() != wxID_OK)
        return;

    // add the destination dir to the tree
    SelectItem(AddDestination (dlg.GetPath()));
}


void BFBackupTree::OnCreateDestination (wxCommandEvent& rEvent)
{
    wxString strPath;

    if (lastItemId_ != GetRootItem())
        strPath = GetPathByItem(lastItemId_);

    new BFDestinationDlg(BFMainFrame::Instance(),
                         strPath,
                         BFDestinationDlg::add_destination);
}


void BFBackupTree::OnModifyDestination (wxCommandEvent& rEvent)
{
    new BFDestinationDlg(BFMainFrame::Instance(),
                         GetPathByItem(lastItemId_),
                         BFDestinationDlg::modify_destination);
}


void BFBackupTree::OnTaskSettings (wxCommandEvent& rEvent)
{
    ShowTaskSettings(lastItemId_);
}


void BFBackupTree::OnDeleteTask (wxCommandEvent& rEvent)
{
    if ( IsTask(lastItemId_) )
    /* one task */
    {
        BFTask* pTask = GetTaskByItem(lastItemId_);

        if (pTask == NULL)
            return;

        if ( BFMainFrame::Instance()->QuestionYesNo(_("Do you realy want to delete this task?")) )
            BFRootTask::Instance().DeleteTask(pTask->GetOID());
    }
    else
    /* a couple of tasks */
    {
        // get the affected tree-item-ids
        VectorTreeItemId vecIds( GetTaskItems(lastItemId_, true) );

        // get the corrosponding BFTask
        BFTaskVector vecTasks;
        for (ItVectorTreeItemId it = vecIds.begin();
             it != vecIds.end();
             ++it)
        {
            vecTasks.push_back(GetTaskByItem(*it));
        }

        // create the question string
        wxString str = wxString::Format("Are you sure to delete this %d tasks?\n", vecTasks.size());
        for (BFTaskVectorIt it = vecTasks.begin();
             it != vecTasks.end();
             ++it)
        {
            str = str + "\n\t" + (*it)->GetName();
        }

        // ask the user
        if ( BFMainFrame::Instance()->QuestionYesNo(str) )
        {
            // delete the tasks
            BFRootTask::Instance().DeleteTasks(vecTasks);
        }
    }
}


wxTreeItemId BFBackupTree::AddDestination (wxString strPath)
{
    wxTreeItemId    idLast, idCurr;
    wxString        strCurr, strCurrFilled, strAdd;

    wxStringTokenizer tkz(strPath, wxFILE_SEP_PATH);
    idLast = GetRootItem();

    while ( tkz.HasMoreTokens() )
    {
        idCurr          = idLast;
        strCurr         = tkz.GetNextToken();
        strCurrFilled   = strCurr;
        if (bFillBlackfiskPlaceholders_)
            BFTask::FillBlackfiskPlaceholders(strCurrFilled);
        idLast  = FindItem(idCurr, strCurrFilled, false);

        // does the item exists
        if ( idLast.IsOk() )
        {
            idCurr = idLast;
            continue;
        }

        // the current path, stored behind the adding item
        strAdd = strPath.Left(strPath.Find(strCurr) + strCurr.Len());

        // ** it is a volume **
        if (strCurrFilled[1] == _T(':'))
        {
            // append the volume item
            idLast = AddVolume(idCurr, strCurr, strAdd);

            continue;
        }

        // ** it is a directory **
        idLast = AppendItem
                 (
                    idCurr,
                    strCurrFilled,
                    -1,
                    -1,
                    new BFBackupTreeItemData ( BFInvalidOID, strAdd)
                 );

        if ( wxDir::Exists(strAdd) )
        {
            SetItemImage(idLast, BFIconTable::folder, wxTreeItemIcon_Normal);
            SetItemImage(idLast, BFIconTable::folder_open, wxTreeItemIcon_Expanded);
        }
        else
        {
            SetItemImage(idLast, BFIconTable::folder_virtual, wxTreeItemIcon_Normal);
            SetItemImage(idLast, BFIconTable::folder_virtual_open, wxTreeItemIcon_Expanded);
        }
    }   // while(token)

    return idLast;
}


wxTreeItemId BFBackupTree::AddVolume(wxTreeItemId idParent, wxString strVol, wxString strAdd)
{
    wxTreeItemId    idReturn;
    bool            bJustAdded = false;

    // find the right position for the volume item
    if (HasChildren(GetRootItem()) == true)
    {
        wxTreeItemId        idPrev;
        wxTreeItemIdValue   idCookie;

        for (idPrev = GetFirstChild(GetRootItem(), idCookie);
             idPrev.IsOk();
             idPrev = GetNextSibling(idPrev))
        {
            // add the new volume before the current one
            if ( GetItemText(idPrev) > strVol )
            {
                idPrev = GetPrevSibling(idPrev);

                idReturn = InsertItem
                            (
                                idParent,
                                idPrev,
                                strVol,
                                -1,
                                -1,
                                new BFBackupTreeItemData ( BFInvalidOID, strAdd )
                            );

                bJustAdded = true;
                break;
            }
        }
    }

    // just append the volume if no position was found
    if (bJustAdded == false)
    {
        // append the volume item
        idReturn = AppendItem
                    (
                        idParent,
                        strVol,
                        -1,
                        -1,
                        new BFBackupTreeItemData ( BFInvalidOID, strAdd)
                    );
    }

    // set the volume icon
    wxFSVolume vol(strVol + wxFILE_SEP_PATH);

    switch(vol.GetKind())
    {
        case wxFS_VOL_FLOPPY:
            if ( strVol.StartsWith(_T("A:")) || strVol.StartsWith(_T("B:")) )
                SetItemImage(idReturn, BFIconTable::volume_floppy, wxTreeItemIcon_Normal);
            else
                SetItemImage(idReturn, BFIconTable::volume_removable, wxTreeItemIcon_Normal);
            break;

        case wxFS_VOL_DVDROM:
        case wxFS_VOL_CDROM:
            SetItemImage(idReturn, BFIconTable::volume_cdrom, wxTreeItemIcon_Normal);
            break;

        case wxFS_VOL_NETWORK:
        case wxFS_VOL_DISK:
        case wxFS_VOL_OTHER:
        default:
            SetItemImage(idReturn, BFIconTable::volume_harddisk, wxTreeItemIcon_Normal);
            break;
    };

    return idReturn;
}


wxTreeItemId BFBackupTree::AddTask (BFoid oid, BFTaskType type, const wxChar* strName, const wxChar* strDestination)
{
    wxString str(strName);
    wxString strFull;

    // create strFull
    strFull << strDestination;

    if (strFull.Last() != wxFILE_SEP_PATH)
        strFull << wxFILE_SEP_PATH;

    strFull << strName;

    if (bFillBlackfiskPlaceholders_)
        BFTask::FillBlackfiskPlaceholders(str);

    // add the destination items and the task item itself
    wxTreeItemId id = AppendItem
    (
        AddDestination(strDestination),
        str,
        BFTask::GetTypeIconId(type),
        -1,
        new BFBackupTreeItemData ( oid, strFull.c_str() )
    );

    return id;
}


void BFBackupTree::OnCreateBackup (wxCommandEvent& rEvent)
{
    BFTask*         pTask       = NULL;
    BFTaskType      type;
    BFArchiveFormat aformat     = CompressNOTUSED;
    wxArrayString   arrString;

    // created by "DnD" or "Backup a Backup" ?
    if (strDropedFilename_.IsEmpty())
    {
        BFBackupTreeItemData* pItemData = (BFBackupTreeItemData*)GetItemData(lastItemId_);

        if (pItemData == NULL)
            return;

        // a directory
        if (pItemData->GetOID() == BFInvalidOID)
        {
            strDropedFilename_ = pItemData->GetPath();
        }
        // a task
        else
        {
            BFTask* pTask = BFRootTask::Instance().GetTask(pItemData->GetOID());

            if (pTask == NULL)
                return;

            strDropedFilename_ = strDropedFilename_ + pTask->GetDestination() + wxFILE_SEP_PATH + pTask->GetName();
        }
    }

    switch (rEvent.GetId())
    {
        case BFBACKUPCTRL_ID_COPY_DIR:
            type = TaskDIRCOPY;
            break;

        case BFBACKUPCTRL_ID_COPY_FILE:
            type = TaskFILECOPY;
            break;

        case BFBACKUPCTRL_ID_SYNC_DIR:
            type = TaskSYNC;
            break;

        case BFBACKUPCTRL_ID_ARCHIVE_DIR:
            type = TaskARCHIVE;
            aformat = CompressZIP;
            break;

        default:
            BFSystem::Error((_T("copy ERROR")), _T("BFBackupTree::OnBackupCopy()"));
            return;
            break;
    };  // switch

    // create the new task
    pTask = new BFTask (type,
                        strDropedFilename_,
                        strCurrentDestination_,
                        strDropedFilename_.AfterLast(wxFILE_SEP_PATH),
                        true,   // verify files by default
                        false,  // don't verify file content by default
                        aformat,
                        arrString);

    if ( pTask->IsValid() )
    {
        BFRootTask::Instance().AppendTask(*pTask);
        BFRootTask::Instance().broadcastObservers();
    }
    else
    {
        BFTaskDlg::Show(pTask);
    }

    // the remembered filename and destination is not needed anymore
    strDropedFilename_      = wxEmptyString;
    strCurrentDestination_  = wxEmptyString;
}


void BFBackupTree::OnBeginLabelEdit (wxTreeEvent& rEvent)
{
    if (rEvent.GetItem() != GetRootItem())
        rEvent.Veto();
}


void BFBackupTree::OnEndLabelEdit (wxTreeEvent& rEvent)
{
    if (rEvent.GetItem() == GetRootItem())
        BFRootTask::Instance().SetName(rEvent.GetLabel());
}


/*virtual*/ void BFBackupTree::ValueChanged (Subject* pSender)
{
    if ( pSender != &(BFRootTask::Instance()) )
        return;

    this->AddPendingEvent(wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, BF_BACKUPTREE_REBUILD));
}


BFTask* BFBackupTree::GetTaskByItem (wxTreeItemId itemId)
{
    if ( !(itemId.IsOk()) )
        return NULL;

    // get the data object behind the item
    // because it holds the taskOID
    BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(itemId));

    if (pItemData == NULL)
        return NULL;

    // get the task object from the data layer
    return BFRootTask::Instance().GetTask(pItemData->GetOID());
}


const wxChar* BFBackupTree::GetPathByItem (wxTreeItemId itemId)
{
    // get the data behind the last selected (by right-click) item
    BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(lastItemId_));

    if (pItemData)
    {
        return pItemData->GetPath();
    }
    else
    {
        BFSystem::Fatal(_("not tree item found"), _("BFBackupTree::GetPathByItem"));
        return NULL;
    }
}


bool BFBackupTree::IsTask (wxTreeItemId itemId)
{
    BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(itemId));

    if (pItemData == NULL)
        return false;

    if (pItemData->GetOID() == BFInvalidOID)
        return false;

    return true;
}


wxTreeItemId BFBackupTree::FindItem (wxTreeItemId idStart, const wxChar* label, bool bGoDeep /*= true*/)
{
    if (label == NULL)
        return wxTreeItemId();

    if (GetItemText(idStart) == label)
        return idStart;

    wxTreeItemId idCurr, idRes;
    wxTreeItemIdValue idCookie;

    if (ItemHasChildren(idStart))
    {
        for (idCurr = GetFirstChild(idStart, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idStart, idCookie))
        {
            if (bGoDeep)
            {
                idRes = FindItem(idCurr, label, true);

                if (idRes.IsOk())
                    return idRes;
            }
            else
            {
                if (GetItemText(idCurr) == label)
                    return idCurr;
            }
        }
    }

    return wxTreeItemId();
}

VectorTreeItemId BFBackupTree::GetTaskItems (wxTreeItemId idParent, bool bGoDeep /*= true*/)
{
    VectorTreeItemId vec;

    wxTreeItemId idCurr;
    wxTreeItemIdValue idCookie;

    if (ItemHasChildren(idParent))
    {
        for (idCurr = GetFirstChild(idParent, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idParent, idCookie))
        {
            if (ItemHasChildren(idCurr) == bGoDeep)
            {
                VectorTreeItemId vecSub(GetTaskItems(idCurr, true));

                for (ItVectorTreeItemId it = vecSub.begin();
                     it != vecSub.end();
                     it++)
                {
                    vec.push_back(*it);
                }
            }
            else
            {
                if (IsTask(idCurr))
                    vec.push_back(idCurr);
            }
        }
    }

    return vec;
}

void BFBackupTree::SetFillBlackfiskPlaceholders(bool bValue)
{
    if (bFillBlackfiskPlaceholders_ == bValue)
        return;

    bFillBlackfiskPlaceholders_ = bValue;
    Init();
}

BFBackupTreeItemData::BFBackupTreeItemData (BFoid oid, const wxChar* strPath /*= NULL*/)
                : oid_(oid),
                  strPath_(strPath)
{
}

/*virtual*/ BFBackupTreeItemData::~BFBackupTreeItemData ()
{
}

BFoid BFBackupTreeItemData::GetOID ()
{
    return oid_;
}

const wxChar* BFBackupTreeItemData::GetPath ()
{
    return strPath_.c_str();
}



BFBackupTree::BFBackupDropTarget::BFBackupDropTarget (BFBackupTree* pBackupTree)
                                   : pBackupTree_(pBackupTree)
{
}

BFBackupTree::BFBackupDropTarget::~BFBackupDropTarget ()
{
}

bool BFBackupTree::BFBackupDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (pBackupTree_ == NULL)
        return false;

    if (filenames[0] == _T("<oid>"))
        return pBackupTree_->OnDropTask(x, y);
    else
        return pBackupTree_->OnDropFiles(x, y, filenames);
}

wxDragResult BFBackupTree::BFBackupDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    if (pBackupTree_ == NULL)
        return def;

    return pBackupTree_->OnDragOver(x, y, def);
}

BFBackupTree::BFTaskDropTarget::BFTaskDropTarget ()
{
}

BFBackupTree::BFTaskDropTarget::~BFTaskDropTarget ()
{
}

BEGIN_EVENT_TABLE(BFBackupCtrl, wxPanel)
    EVT_TOGGLEBUTTON (BFBACKUPCTRL_ID_MACROBUTTON, BFBackupCtrl::OnButton)
END_EVENT_TABLE()

BFBackupCtrl::BFBackupCtrl (wxWindow* pParent)
            : wxPanel(pParent, -1)
{
    // init sizer
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

    /* toolbar
    wxToolBar* pTool = new wxToolBar(this, -1);
    pTool->AddTool(-1, _T("label 1"), wxBitmap(_T("graphic\\dir.bmp"), wxBITMAP_TYPE_BMP));
    pTool->AddSeparator();
    pTool->AddTool(-1, _T("label 2"), wxBitmap(_T("graphic\\dir.bmp"), wxBITMAP_TYPE_BMP));
    pTool->Realize();
    pTool->SetMinSize(wxSize(pTool->GetSize().GetWidth()*8, 26));*/

    // init controls
    pBackupTree_    = new BFBackupTree(this);
    pMacroButton_   = new wxToggleButton(this, BFBACKUPCTRL_ID_MACROBUTTON, _("fill placeholders"));
    pMacroButton_->SetToolTip(_("display items in the backup tree with\nplaceholders (e.g. <date>) or fill them (e.g. 2008-12-24)"));
    pMacroButton_->SetValue(BFSettings::Instance().GetFillBlackfiskPlaceholders());
    BackupTree()->SetFillBlackfiskPlaceholders(BFSettings::Instance().GetFillBlackfiskPlaceholders());

    // arange
    pTopSizer->Add(pMacroButton_, wxSizerFlags(0).Center().Border(wxUP | wxDOWN, 5));
    pTopSizer->Add(pBackupTree_, wxSizerFlags(1).Expand());
    SetSizer(pTopSizer);
}

/*virtual*/ BFBackupCtrl::~BFBackupCtrl ()
{
}


BFBackupTree* BFBackupCtrl::BackupTree ()
{
    return pBackupTree_;
}

void BFBackupCtrl::OnButton (wxCommandEvent& rEvent)
{
    BackupTree()->SetFillBlackfiskPlaceholders(pMacroButton_->GetValue());
}
