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
#include "BFTaskBase.h"
#include "BFRootTask.h"
#include "BFRootTaskApp.h"
#include "BFTaskDlg.h"
#include "BFDestinationDlg.h"
#include "BFSettings.h"

#define BF_BACKUPTREE_PLACEHOLDER_MASK  "*<*>*"
#define BF_BACKUPTREE_FILLED_DATE_MASK  wxString::Format("*%s*", BFCore::Instance().GetDateString_Old())
#define BF_BACKUPTREE_FILLED_TIME_MASK  wxString::Format("*%s*", BFCore::Instance().GetTimeString_Old())

BEGIN_EVENT_TABLE(BFBackupTree, wxTreeCtrl)
    EVT_TREE_ITEM_ACTIVATED     (wxID_ANY,                              BFBackupTree::OnItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK   (wxID_ANY,                              BFBackupTree::OnItemMenu)
    EVT_MENU                    (BFBACKUPCTRL_ID_ADDDESTINATION,        BFBackupTree::OnAddDestination)
    EVT_MENU                    (BFBACKUPCTRL_ID_PROJECTSETTINGS,       BFBackupTree::OnProjectSettings)
    EVT_MENU                    (BFBACKUPCTRL_ID_CREATEDESTINATION,     BFBackupTree::OnCreateDestination)
    EVT_MENU                    (BFBACKUPCTRL_ID_TASKSETTINGS,          BFBackupTree::OnTaskSettings)
    EVT_MENU                    (BFBACKUPCTRL_ID_DELETETASK,            BFBackupTree::OnDeleteTask)
    EVT_MENU                    (BFBACKUPCTRL_ID_COPY_DIR,              BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_COPY_FILE,             BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_SYNC_DIR,              BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_ARCHIVE_DIR,           BFBackupTree::OnCreateBackup)
    EVT_MENU                    (BFBACKUPCTRL_ID_MODIFYDDESTINATION,    BFBackupTree::OnModifyDestination)
    EVT_TREE_BEGIN_LABEL_EDIT   (wxID_ANY,                              BFBackupTree::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT     (wxID_ANY,                              BFBackupTree::OnEndLabelEdit)
    EVT_MENU                    (BF_BACKUPTREE_REBUILD,                 BFBackupTree::OnRebuild)
    EVT_TREE_BEGIN_DRAG         (wxID_ANY,                              BFBackupTree::OnBeginDrag)
    EVT_MENU                    (BFBACKUPCTRL_ID_MODIFY_TO_COPY_DIR,    BFBackupTree::OnModifyTaskType)
    EVT_MENU                    (BFBACKUPCTRL_ID_MODIFY_TO_SYNC_DIR,    BFBackupTree::OnModifyTaskType)
    EVT_MENU                    (BFBACKUPCTRL_ID_MODIFY_TO_ARCHIVE_DIR, BFBackupTree::OnModifyTaskType)
END_EVENT_TABLE()


//
BFBackupTree::BFBackupTree (wxWindow* pParent)
            : wxTreeCtrl(pParent,
                         -1,
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS),
              Observer(&(BFRootTask::Instance())),
              oidCurrentDrag_(BFInvalidOID),
              bFillBlackfiskPlaceholders_(BFSettings::Instance().GetFillBlackfiskPlaceholders())
{
    SetImageList ( BFIconTable::Instance()->GetFilesystemImageList() );

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

void BFBackupTree::UpdateItems (VectorTreeItemId &vec)
{
    Freeze();

    for (ItVectorTreeItemId it = vec.begin();
         it != vec.end();
         ++it)
    {
        UpdateItem(*it);
    }

    Thaw();
}

void BFBackupTree::UpdateItem (wxTreeItemId id)
{
    BFTask* pTask = GetTaskByItem(id);

    if (pTask)
    {
        wxString str(pTask->GetName());

        if (bFillBlackfiskPlaceholders_)
            BFTask::FillBlackfiskPlaceholders(str);

        SetItemText(id, str);

        SetItemImage(id, GetTypeIconId(pTask->GetType()));
    }
}

void BFBackupTree::RefreshPlaceholders ()
{
    Freeze ();

    // only refresh filled placeholders!
    if ( !(GetFillBlackfiskPlaceholders()) )
        return;

    VectorTreeItemId vecIdsDate;
    VectorTreeItemId vecIdsTime;
    VectorTreeItemId vecIdsAll;

    // find all items with a label matching to the old filled placeholders
    if ( !(BFCore::Instance().GetDateString_Old().IsEmpty()) )
        vecIdsDate = FindItems (GetRootItem(), BF_BACKUPTREE_FILLED_DATE_MASK);

    if ( !(BFCore::Instance().GetTimeString_Old().IsEmpty()) )
        vecIdsTime = FindItems (GetRootItem(), BF_BACKUPTREE_FILLED_TIME_MASK);

    vecIdsAll.reserve(vecIdsDate.size() + vecIdsTime.size());
    vecIdsAll.insert( vecIdsAll.end(), vecIdsDate.begin(), vecIdsDate.end() );
    vecIdsAll.insert( vecIdsAll.end(), vecIdsTime.begin(), vecIdsTime.end() );

    // are there items to refresh?
    if (vecIdsAll.empty())
        return;

    wxString strPath;
    wxString strLabel;

    // iterate on the items
    for (ItVectorTreeItemId it = vecIdsAll.begin();
         it != vecIdsAll.end();
         ++it)
    {
        strPath = GetPathByItem(*it);

        while ( strPath.Matches(BF_BACKUPTREE_PLACEHOLDER_MASK)
             || strPath.Matches(BF_BACKUPTREE_PLACEHOLDER_MASK))
        {
            strLabel = GetItemText(*it);

            if ( strLabel.Matches(BF_BACKUPTREE_FILLED_DATE_MASK)
              || strLabel.Matches(BF_BACKUPTREE_FILLED_TIME_MASK))
            {
                strLabel = strPath.AfterLast(wxFILE_SEP_PATH);
                SetItemText(*it, BFTask::FillBlackfiskPlaceholders(strLabel));
            }

            // cut the last diretory from path
            strPath = strPath.BeforeLast(wxFILE_SEP_PATH);
        }
    }

    Thaw();
}

void BFBackupTree::UpdatePlaceholders ()
{
    Freeze();

    wxString strLabel;

    if (bFillBlackfiskPlaceholders_)
    // --- "<date>" => "2000-01-01" ---
    {
        VectorTreeItemId vecIds;

        // find all items with a placeholder in its label
        vecIds = FindItems(GetRootItem(), BF_BACKUPTREE_PLACEHOLDER_MASK);

        // iterate on the items
        for (ItVectorTreeItemId it = vecIds.begin();
             it != vecIds.end();
             ++it)
        {
            strLabel = GetItemText(*it);
            SetItemText(*it, BFTask::FillBlackfiskPlaceholders(strLabel));
        }
    }
    else
    // --- "2000-01-01" => "<date>" ---
    {
        BFTaskVector vecTasks;
        wxString strPlaceholder;

        // find all tasks with placeholders
        BFRootTask::Instance().FindAllTasksWithPlaceholders(vecTasks);

        // iterate over all relevant items
        for (BFTaskVectorIt it = vecTasks.begin();
             it != vecTasks.end();
             ++it)
        {
            // get the corrosponding tree-item
            wxTreeItemId id = FindItem (GetRootItem(), (*it)->GetOID());

            // ** name **
            if ((*it)->GetName().Matches(BF_BACKUPTREE_PLACEHOLDER_MASK))
                SetItemText(id, (*it)->GetName());


            // ** destination **
            strPlaceholder = (*it)->GetDestination();

            while (strPlaceholder.Matches(BF_BACKUPTREE_PLACEHOLDER_MASK))
            {
                strLabel = strPlaceholder.AfterLast(wxFILE_SEP_PATH);

                if (strLabel.Matches(BF_BACKUPTREE_PLACEHOLDER_MASK))
                {
                    // find the tree-item with the path
                    wxTreeItemId idCurr = FindItemByPath(GetRootItem(), strPlaceholder);

                    // reset the item text
                    if (idCurr.IsOk())
                        SetItemText(idCurr, strLabel);
                }

                // cut the last diretory from path
                strPlaceholder = strPlaceholder.BeforeLast(wxFILE_SEP_PATH);
            }
        }
    }

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


/*static*/ wxMenu& BFBackupTree::GenerateBackupMenu (wxMenu& menu, bool bOnDir, bool bBackup /*= true*/)
{
    wxMenuItem *pItem = NULL;
    int id;

    // check if it is a dir
    if ( bOnDir )
    {
        // ** backup dir copy **
        if (bBackup)
            id = BFBACKUPCTRL_ID_COPY_DIR;
        else
            id = BFBACKUPCTRL_ID_MODIFY_TO_COPY_DIR;

        pItem = new wxMenuItem(&menu, id, _("copy directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu.Append(pItem);

        // ** backup sync dir **
        if (bBackup)
            id = BFBACKUPCTRL_ID_SYNC_DIR;
        else
            id = BFBACKUPCTRL_ID_MODIFY_TO_SYNC_DIR;

        pItem = new wxMenuItem(&menu, id, _("synchronise directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_sync));
        menu.Append(pItem);

        // ** backup archive dir **
        if (bBackup)
            id = BFBACKUPCTRL_ID_ARCHIVE_DIR;
        else
            id = BFBACKUPCTRL_ID_MODIFY_TO_ARCHIVE_DIR;

        pItem = new wxMenuItem(&menu, id, _("archive/compress directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_zip));
        menu.Append(pItem);
    }
    else
    {
        // ** backup file copy **
        pItem = new wxMenuItem(&menu, BFBACKUPCTRL_ID_COPY_FILE, _("copy file"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_filecopy));
        menu.Append(pItem);
    }

    return menu;
}

void BFBackupTree::OnItemMenu(wxTreeEvent& rEvent)
{
    wxPoint         point   (rEvent.GetPoint());
    wxMenu          menu, menu_backup, menu_type;
    wxString        strLabel_ModifyTaskType(_("modify all sub-tasks to ..."));
    wxString        strLabel_DeleteTask(_("delete task"));
    wxString        strLabel_CreateDest(_("create destination directory"));
    wxString        strLabel_Backup(_("backup"));

    // remember the selected item
    lastItemId_ = rEvent.GetItem();

    // check if there are task-items as childs
    vecLastChildTasks_ = GetTaskItems(lastItemId_);

    if ( IsTask(lastItemId_) )
    {
        BFTask* pTask = GetTaskByItem(lastItemId_);

        if (pTask == NULL)
            return;

        // ** create destination directory **
        if (pTask->GetType() == TaskDIRCOPY)
            menu.Append(BFBACKUPCTRL_ID_CREATEDESTINATION, strLabel_CreateDest);

        // ** delete task **
        menu.Append(BFBACKUPCTRL_ID_DELETETASK, strLabel_DeleteTask);

        // ** modify types **
        if ( vecLastChildTasks_.size() > 0)
        {
            BFBackupTree::GenerateBackupMenu(menu_type, true, false);
            menu.AppendSubMenu(&menu_type, strLabel_ModifyTaskType);
        }

        // separator
        menu.AppendSeparator();

        // ** backup **
        if (pTask->GetType() == TaskFILECOPY)
            BFBackupTree::GenerateBackupMenu(menu_backup, false);
        else
            BFBackupTree::GenerateBackupMenu(menu_backup, true);

        menu.AppendSubMenu(&menu_backup, strLabel_Backup);

        // separator
        menu.AppendSeparator();

        // ** task settings **
        menu.Append(BFBACKUPCTRL_ID_TASKSETTINGS, _("Task settings"));
    }
    else
    {
        // for non-existing (while backup created) directories
        menu.Append(BFBACKUPCTRL_ID_CREATEDESTINATION, strLabel_CreateDest);

        // right click on root item
        if (GetRootItem() == lastItemId_)
        {
            // for real existing directories
            menu.Append(BFBACKUPCTRL_ID_ADDDESTINATION, _("add destination directory"));
            menu.Append(BFBACKUPCTRL_ID_DELETETASK, strLabel_DeleteTask);

            // ** modify types **
            if ( vecLastChildTasks_.size() > 0)
            {
                BFBackupTree::GenerateBackupMenu(menu_type, true, false);
                menu.AppendSubMenu(&menu_type, strLabel_ModifyTaskType);
            }

            menu.AppendSeparator();
            menu.Append(BFBACKUPCTRL_ID_PROJECTSETTINGS, _("Project settings"));
        }
        else
        {
            menu.Append(BFBACKUPCTRL_ID_MODIFYDDESTINATION, _("modify destination"));
            menu.Append(BFBACKUPCTRL_ID_DELETETASK, strLabel_DeleteTask);

            // ** modify types **
            if ( vecLastChildTasks_.size() > 0)
            {
                BFBackupTree::GenerateBackupMenu(menu_type, true, false);
                menu.AppendSubMenu(&menu_type, strLabel_ModifyTaskType);
            }

            menu.AppendSeparator();
            BFBackupTree::GenerateBackupMenu(menu_backup, true);
            menu.AppendSubMenu(&menu_backup, strLabel_Backup);
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
        bool bSelect = false;

        // get the task behind the item
        BFTask* pTask = GetTaskByItem(itemId);

        if (pTask)
        {
            if ( pTask->GetType() == TaskDIRCOPY )
                bSelect = true;
        }

        if (!bSelect)
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
                bSelect = true;
            }
        }

        if (bSelect)
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

    if ( pItemData )
      // && pItemData->GetOID() == BFInvalidOID)
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
    if ( pItemData )
    {
        wxString strOldPath = pTask->GetDestination() + wxFILE_SEP_PATH + pTask->GetName();
        wxString strNewPath = pItemData->GetPath() + wxFILE_SEP_PATH + pTask->GetName();

        pTask->SetDestination(pItemData->GetPath());

        if ( HasChildren(pTask) )
            BFRootTaskApp::Instance().ModifyDestination(strOldPath, strNewPath);

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
    dragSource.DoDragDrop( TRUE );
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

        if ( BFMainFrame::Instance()->QuestionYesNo(_("Should this task be deleted?")) )
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
        wxString str = wxString::Format("Should this %d tasks be deleted?\n", vecTasks.size());
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


wxTreeItemId BFBackupTree::AddDestination (const wxString& strPath)
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

        idLast = FindItem(idCurr, strCurrFilled, false);

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


wxTreeItemId BFBackupTree::AddVolume(wxTreeItemId idParent,
                                     const wxString& strVol,
                                     const wxString& strAdd)
{
    wxTreeItemId    idReturn;
    bool            bJustAdded = false;

    // find the right position for the volume item
    if (ItemHasChildren(GetRootItem()) == true)
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

wxTreeItemId BFBackupTree::AddTask (BFoid oid,
                                    BFTaskType type,
                                    const wxString& strName,
                                    const wxString& strDestination)
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
        GetTypeIconId(type),
        -1,
        new BFBackupTreeItemData ( oid, strFull )
    );

    return id;
}

void BFBackupTree::OnModifyTaskType (wxCommandEvent& rEvent)
{
    long iCount;
    wxString strQuestion;
    wxArrayString arrTasks;

    // the type
    BFTaskType type;
    switch (rEvent.GetId())
    {
        case BFBACKUPCTRL_ID_MODIFY_TO_COPY_DIR:
            type = TaskDIRCOPY;
            break;

        case BFBACKUPCTRL_ID_MODIFY_TO_SYNC_DIR:
            type = TaskSYNC;
            break;

        case BFBACKUPCTRL_ID_MODIFY_TO_ARCHIVE_DIR:
            type = TaskARCHIVE;
            break;

        default:
            BFSystem::Error((_T("unknown ID")), _T("BFBackupTree::OnModifyTaskType()"));
            return;
            break;
    };  // switch

    // exclude file tasks
    for (ItVectorTreeItemId it = vecLastChildTasks_.begin();
         it != vecLastChildTasks_.end();
         ++it)
    {
        BFTask* pTask = GetTaskByItem(*it);

        if (pTask)
        {
            if (pTask->GetType() == TaskFILECOPY
             || pTask->GetType() == type)
            {
                vecLastChildTasks_.erase(it);
                --it;
                ++iCount;
            }
            else
            {
                arrTasks.Add(wxString::Format("\t\t%s", pTask->GetName()));
            }
        }
    }

    // check if there are any tasks
    if (vecLastChildTasks_.empty())
        return;

    // build the question
    strQuestion = wxString::Format(_("The type of the following tasks will be changed to \"%s\".\n\n%s\n\nChange the type?"),
                                   BFTask::GetTypeDescription(type),
                                   wxJoin(arrTasks, '\n'));

    // ask the user
    if ( BFMainFrame::Instance()->QuestionYesNo(strQuestion) == false )
        return;

    // modify the type
    for (ItVectorTreeItemId it = vecLastChildTasks_.begin();
         it != vecLastChildTasks_.end();
         ++it)
    {
        BFTask* pTask = GetTaskByItem(*it);

        if (pTask)
            pTask->SetTaskType(type);
    }

    BFRootTask::Instance().SetModified(true);
    UpdateItems(vecLastChildTasks_);
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
                        arrString,
                        true);

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
    BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(itemId));

    if (pItemData)
    {
        return pItemData->GetPath();
    }
    else
    {
        BFSystem::Fatal("no tree item found", "BFBackupTree::GetPathByItem");
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

bool BFBackupTree::HasPath (wxTreeItemId itemId, const wxString& strPath)
{
    BFBackupTreeItemData* pData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(itemId));

    if (pData)
        if (strPath == pData->GetPath())
            return true;

    return false;
}

bool BFBackupTree::HasOID (wxTreeItemId itemId, BFoid oid)
{
    BFBackupTreeItemData* pData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(itemId));

    if (pData)
        if (pData->GetOID() == oid)
            return true;

    return false;
}

bool BFBackupTree::HasChildren (BFTask* pTask)
{
    if (pTask)
    {
        wxTreeItemId id = FindItem(GetRootItem(), pTask->GetOID());

        if (id.IsOk())
            if ( ItemHasChildren(id) )
                return true;
    }

    return false;
}

wxTreeItemId BFBackupTree::FindItem (wxTreeItemId idStart,
                                     const wxString& strLabel,
                                     bool bGoDeep /*= true*/)
{
    VectorTreeItemId vecIds = FindItems(idStart, strLabel, bGoDeep, false);

    if (vecIds.size() > 0)
        return vecIds[0];

    return wxTreeItemId();
}

wxTreeItemId BFBackupTree::FindItem (wxTreeItemId idStart, BFoid oid)
{
    wxTreeItemId            idCurr, idLast;
    wxTreeItemIdValue       idCookie;

    // check start item
    if (HasOID(idStart, oid))
        return idStart;

    if (ItemHasChildren(idStart))
    {
        for (idCurr = GetFirstChild(idStart, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idStart, idCookie))
        {
            idLast = FindItem(idCurr, oid);

            if (idLast.IsOk())
                return idLast;
        }
    }

    return wxTreeItemId();
}

wxTreeItemId BFBackupTree::FindItemByPath (wxTreeItemId idStart, const wxString& strPath)
{
    wxTreeItemId            idCurr, idLast;
    wxTreeItemIdValue       idCookie;
    wxString                str;

    // check start item
    if (HasPath(idStart, strPath))
        return idStart;

    if (ItemHasChildren(idStart))
    {
        for (idCurr = GetFirstChild(idStart, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idStart, idCookie))
        {
            idLast = FindItemByPath(idCurr, strPath);

            if (idLast.IsOk())
                return idLast;
        }
    }

    return wxTreeItemId();
}

VectorTreeItemId  BFBackupTree::FindItems (wxTreeItemId idStart,
                                           const wxString& strLabel,
                                           bool bGoDeep /*= true*/,
                                           bool bSearchForAll /*= true*/)
{
    VectorTreeItemId vecRes, vecDeep;
    wxTreeItemId idCurr;
    wxTreeItemIdValue idCookie;

    if (GetItemText(idStart).Matches(strLabel))
        vecRes.push_back(idStart);

    if (ItemHasChildren(idStart))
    {
        for (idCurr = GetFirstChild(idStart, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idStart, idCookie))
        {
            if ( !bSearchForAll && vecRes.size() == 1 )
                return vecRes;

            if (bGoDeep)
            {
                vecDeep = FindItems(idCurr, strLabel, true);
                vecRes.reserve ( vecRes.size() + vecDeep.size() );
                vecRes.insert ( vecRes.end(), vecDeep.begin(), vecDeep.end() );
            }
            else
            {
                if (GetItemText(idCurr).Matches(strLabel))
                    vecRes.push_back(idCurr);
            }
        }
    }

    return vecRes;
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

bool BFBackupTree::GetFillBlackfiskPlaceholders ()
{
    return bFillBlackfiskPlaceholders_;
}

void BFBackupTree::SetFillBlackfiskPlaceholders(bool bValue)
{
    if (bFillBlackfiskPlaceholders_ == bValue)
        return;

    bFillBlackfiskPlaceholders_ = bValue;
    UpdatePlaceholders();
}

BFBackupTreeItemData::BFBackupTreeItemData (BFoid oid, const wxString& strPath /*= wxEmptyString*/)
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

const wxString& BFBackupTreeItemData::GetPath ()
{
    return strPath_;
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
    EVT_CHECKBOX (BFBACKUPCTRL_ID_CHECK_PLACEHOLDERS, BFBackupCtrl::OnCheck_Placeholders)
END_EVENT_TABLE()

BFBackupCtrl::BFBackupCtrl (wxWindow* pParent)
            : wxPanel(pParent, -1)
{
    // init sizer
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

    // init controls
    pBackupTree_        = new BFBackupTree(this);
    pCheckPlaceholders_ = new wxCheckBox(this, BFBACKUPCTRL_ID_CHECK_PLACEHOLDERS, _("fill placeholders"));
    pCheckPlaceholders_->SetToolTip(_("display items in the backup tree with\nplaceholders (e.g. <date>) or fill them (e.g. 2008-12-24)"));
    pCheckPlaceholders_->SetValue(BFSettings::Instance().GetFillBlackfiskPlaceholders());
    BackupTree()->SetFillBlackfiskPlaceholders(BFSettings::Instance().GetFillBlackfiskPlaceholders());

    // arange
    pTopSizer->Add(pCheckPlaceholders_, wxSizerFlags(0).Center().Border());
    pTopSizer->Add(pBackupTree_,        wxSizerFlags(1).Expand());
    SetSizer(pTopSizer);
}

/*virtual*/ BFBackupCtrl::~BFBackupCtrl ()
{
}


BFBackupTree* BFBackupCtrl::BackupTree ()
{
    return pBackupTree_;
}

void BFBackupCtrl::OnCheck_Placeholders (wxCommandEvent& rEvent)
{
    BackupTree()->SetFillBlackfiskPlaceholders(pCheckPlaceholders_->GetValue());
}


/*static*/ BFIconTable::BFFilesystemIconId BFBackupTree::GetTypeIconId (BFTaskType type)
{
    switch (type)
    {
        case TaskARCHIVE:
            return BFIconTable::task_zip;

        case TaskDIRCOPY:
            return BFIconTable::task_dircopy;

        case TaskFILECOPY:
            return BFIconTable::task_filecopy;

        case TaskSYNC:
            return BFIconTable::task_sync;

        default:
            BFSystem::Fatal("no matching TaskType", "BFTask::GetTypeIconId");
            return BFIconTable::_filesystem_unused_;
    };
}
