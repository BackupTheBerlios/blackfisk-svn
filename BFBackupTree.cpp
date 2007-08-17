/**
 * Name:        BFBackupTree.cpp
 * Purpose:     BFBackupTree and BFBackupCtrl class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-25
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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
    EVT_TREE_BEGIN_LABEL_EDIT   (wxID_ANY,                          BFBackupTree::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT     (wxID_ANY,                          BFBackupTree::OnEndLabelEdit)
END_EVENT_TABLE()


//
BFBackupTree::BFBackupTree (wxWindow* pParent)
            : wxTreeCtrl(pParent,
                         -1,
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS),
              Observer(&(BFRootTask::Instance())),
              bReplaceMacro_(BFSettings::Instance().GetReplaceMacros())
{
    //SetImageList ( &(BFMainFrame::Instance()->GetImageList()) );
    SetImageList ( BFIconTable::Instance() );
    SetDropTarget   ( new BFBackupDropTarget(this) );
    Init();
}


//
/*virtual*/ BFBackupTree::~BFBackupTree ()
{
}

void BFBackupTree::Init ()
{
    // delete all items in the treeCtrl
    DeleteAllItems();

    // recreate the treeCtrl with all tasks
    BFRootTask::Instance().InitThat(*this);

    // expand all items in the treeCtlr
    ExpandAll();
    SelectItem(lastItemId_);

    SetToolTip(_("files and directories for the backup"));
}
/*
wxString& BFBackupTree::ReplaceMacro(wxString& str)
{
    if (bReplaceMacro_)
        ;

    return str;
}*/

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

void BFBackupTree::OnItemMenu(wxTreeEvent& rEvent)
{
    wxPoint         point   (rEvent.GetPoint());
    wxMenu          menu;

    // remember the selected item
    lastItemId_ = rEvent.GetItem();

    if ( IsTask(lastItemId_) )
    {
        BFTask* pTask = GetTaskByItem(lastItemId_);

        if (pTask == NULL)
            return;

        // create destination directory
        if (pTask->GetType() == TaskDIRCOPY)
            menu.Append(BFBACKUPCTRL_ID_CREATEDESTINATION, _("create destination directory"));

        // delete task
        menu.Append(BFBACKUPCTRL_ID_DELETETASK, _("delete task"));

        // separator if needed
        if (menu.GetMenuItemCount() > 0)
            menu.AppendSeparator();

        // task settings
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
            menu.AppendSeparator();
            menu.Append(BFBACKUPCTRL_ID_PROJECTSETTINGS, _("Project settings"));
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
    wxMenuItem*             pItem;
    wxString                str;

    // ** copy entry **
    if (wxDir::Exists(filenames[0]))
    {
        // copy directory
        pItem = new wxMenuItem(&menu, BFBACKUPCTRL_ID_COPY_DIR, _("copy directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_dircopy));
        menu.Append(pItem);

        // sync directory
        pItem = new wxMenuItem(&menu, BFBACKUPCTRL_ID_SYNC_DIR, _("synchronize directory"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_sync));
        menu.Append(pItem);
    }
    else
    {
        // copy file
        pItem = new wxMenuItem(&menu, BFBACKUPCTRL_ID_COPY_FILE, _("copy file"));
        pItem->SetBitmap(BFIconTable::Instance()->GetIcon(BFIconTable::task_filecopy));
        menu.Append(pItem);
    }


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
    {
        // get the data behind the last selected (by right-click) item
        BFBackupTreeItemData* pItemData = dynamic_cast<BFBackupTreeItemData*>(GetItemData(lastItemId_));

        if (pItemData == NULL)
        {
            BFSystem::Fatal(_("not tree item found"), _("BFBackupTree::OnCreateDestination"));
            return;
        }

        strPath = pItemData->GetPath();
    }

    new BFDestinationDlg(BFMainFrame::Instance(), strPath);
}

void BFBackupTree::OnTaskSettings (wxCommandEvent& rEvent)
{
    ShowTaskSettings(lastItemId_);
}

void BFBackupTree::OnDeleteTask (wxCommandEvent& rEvent)
{
    BFTask* pTask = GetTaskByItem(lastItemId_);

    if (pTask == NULL)
        return;

    if ( BFMainFrame::Instance()->QuestionYesNo(_("Do you realy want to delete this task?")) )
        BFRootTask::Instance().DeleteTask(pTask->GetOID());
}

wxTreeItemId BFBackupTree::AddDestination (wxString strPath)
{
    // add root
    wxTreeItemId    idLast, idCurr;
    wxString        strCurr, strAdd;

    wxStringTokenizer tkz(strPath, wxFILE_SEP_PATH);
    idLast = GetRootItem();

    while ( tkz.HasMoreTokens() )
    {
        idCurr  = idLast;
        strCurr = tkz.GetNextToken();
        BFTaskBase::ReplaceMacros(strCurr);
        idLast  = FindItem(idCurr, strCurr, false);

        // does the item exists
        if ( idLast.IsOk() )
        {
            idCurr = idLast;
            continue;
        }

        // the current path, stored behind the adding item
        strAdd = strPath.Left(strPath.Find(strCurr) + strCurr.Len());

        // ** it is a volume **
        if (strCurr[1] == _T(':'))
        {
            // append the volume item
            idLast = AppendItem
                     (
                        idCurr,
                        strCurr,
                        -1,
                        -1,
                        new BFBackupTreeItemData ( BFInvalidOID, strAdd)
                     );

            // set the volume icon
            wxFSVolume vol(strCurr + wxFILE_SEP_PATH);

            switch(vol.GetKind())
            {
                case wxFS_VOL_FLOPPY:
                    if ( strCurr.StartsWith(_T("a:")) || strCurr.StartsWith(_T("b:")) )
                        SetItemImage(idLast, BFIconTable::volume_floppy, wxTreeItemIcon_Normal);
                    else
                        SetItemImage(idLast, BFIconTable::volume_removable, wxTreeItemIcon_Normal);
                    break;

                case wxFS_VOL_DVDROM:
                case wxFS_VOL_CDROM:
                    SetItemImage(idLast, BFIconTable::volume_cdrom, wxTreeItemIcon_Normal);
                    break;

                case wxFS_VOL_NETWORK:
                case wxFS_VOL_DISK:
                case wxFS_VOL_OTHER:
                default:
                    SetItemImage(idLast, BFIconTable::volume_harddisk, wxTreeItemIcon_Normal);
                    break;
            };

            continue;
        }

        // ** it is a directory **
        idLast = AppendItem
                 (
                    idCurr,
                    strCurr,
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

wxTreeItemId BFBackupTree::AddTask (BFoid oid, BFTaskType type, const wxChar* strName, const wxChar* strDestination)
{
    wxString str(strName);
    wxString strFull;

    // create strFull
    strFull << strDestination;

    if (strFull.Last() != wxFILE_SEP_PATH)
        strFull << wxFILE_SEP_PATH;

    strFull << strName;

    if (bReplaceMacro_)
        BFTaskBase::ReplaceMacros(str);

    // add the destination items and the task item itself
    wxTreeItemId id = AppendItem
    (
        AddDestination(strDestination),
        str,
        BFTask::GetTypeIconId(type),
        -1,
        new BFBackupTreeItemData
        (
            oid,
            strFull.c_str()
        )
    );

    return id;
}

void BFBackupTree::OnCreateBackup (wxCommandEvent& rEvent)
{
    BFTask*         pTask       = NULL;
    BFTaskType      type;
    bool            bVerify     = true /* DEBUG: get default value */;
    wxArrayString   arrString;

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

        default:
            BFSystem::Error((_T("copy ERROR")), _T("BFBackupTree::OnBackupCopy()"));
            return;
            break;
    };  // switch

    // create and show the new task
    pTask = new BFTask (type,
                        strDropedFilename_,
                        strCurrentDestination_,
                        strDropedFilename_.AfterLast(wxFILE_SEP_PATH),
                        true /* DEBUG */,
                        CompressNOTUSED,
                        arrString);
    BFTaskDlg::Show(pTask);

    // the remembered filename and destination is not needed anymore
    strDropedFilename_      = wxEmptyString;
    strCurrentDestination_  = wxEmptyString;

    // refresh the backup tree view
    BFRootTask::Instance().broadcastObservers();
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

    Init();
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
        for (wxTreeItemId idCurr = GetFirstChild(idStart, idCookie);
             idCurr.IsOk();
             idCurr = GetNextChild(idStart, idCookie))
        {
            if (bGoDeep)
            {
                idRes = FindItem(idCurr, label);

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

wxTreeItemId BFBackupTree::FindItem (const wxChar* label)
{
    if (label == NULL)
        return wxTreeItemId();

    return FindItem(GetRootItem(), label);
}

void BFBackupTree::SetReplaceMacro(bool bValue)
{
    if (bReplaceMacro_ == bValue)
        return;

    bReplaceMacro_ = bValue;
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

    return pBackupTree_->OnDropFiles(x, y, filenames);
}

wxDragResult BFBackupTree::BFBackupDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    if (pBackupTree_ == NULL)
        return def;

    return pBackupTree_->OnDragOver(x, y, def);
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
    pMacroButton_   = new wxToggleButton(this, BFBACKUPCTRL_ID_MACROBUTTON, _("macros"));
    pMacroButton_->SetToolTip(_("display items in the backup tree with macros (e.g. <date>)\nor filled macros (e.g. 2008-12-24)"));
    pMacroButton_->SetValue(BFSettings::Instance().GetReplaceMacros());
    BackupTree()->SetReplaceMacro(BFSettings::Instance().GetReplaceMacros());

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
    BackupTree()->SetReplaceMacro(pMacroButton_->GetValue());
}
