/**
 * implementation of class OBBackupTree
 * 2006-05-25
 ***/

#include "OctopusBackup.h"
#include "OBApp.h"
#include "OBMainFrame.h"
#include "OBTask.h"
#include "OBBackupTree.h"
#include "OBRootTask.h"
#include "OBTaskDlg.h"
#include <wx/icon.h>
#include <wx/dnd.h>
#include <wx/tokenzr.h>

BEGIN_EVENT_TABLE(OBBackupTree, wxTreeCtrl)
    EVT_TREE_ITEM_ACTIVATED     (wxID_ANY,                          OBBackupTree::OnItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK   (wxID_ANY,                          OBBackupTree::OnItemMenu)
    EVT_MENU                    (OBID_BACKUPCTRL_ADDDESTINATION,    OBBackupTree::OnAddDestination)
    EVT_MENU                    (OBID_BACKUPCTRL_COPY_DIR,          OBBackupTree::OnBackupCopy)
    EVT_MENU                    (OBID_BACKUPCTRL_COPY_FILE,         OBBackupTree::OnBackupCopy)
    EVT_TREE_BEGIN_LABEL_EDIT   (wxID_ANY,                          OBBackupTree::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT     (wxID_ANY,                          OBBackupTree::OnEndLabelEdit)
END_EVENT_TABLE()
//
OBBackupTree::OBBackupTree (wxWindow* pParent)
            : wxTreeCtrl(pParent,
                         -1,
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS),
              Observer(&(OBRootTask::Instance())),
              imageList_(16, 16)
{
    // init image list
    /* OBICON_PROJECT   */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("project.ico")),   wxBITMAP_TYPE_ICO));
    /* OBICON_VOLUME    */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("volume.ico")),    wxBITMAP_TYPE_ICO));
    /* OBICON_DIR       */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("dir.ico")),       wxBITMAP_TYPE_ICO));
    /* OBICON_TASKDC    */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("task_dc.ico")),   wxBITMAP_TYPE_ICO));
    /* OBICON_TASKFC    */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("task_fc.ico")),   wxBITMAP_TYPE_ICO));
    /* OBICON_TASKZIP   */ imageList_.Add(wxIcon(wxString::Format(_T("%s%s"), OB_GRAPHICDIR, _T("task_zip.ico")),  wxBITMAP_TYPE_ICO));

    SetImageList( &imageList_ );

    SetDropTarget ( new OBBackupDropTarget(this) );

    Init();
}


//
/*virtual*/ OBBackupTree::~OBBackupTree ()
{
}

void OBBackupTree::Init ()
{
    // delete all items in the treeCtrl
    DeleteAllItems();

    // recreate the treeCtrl with all tasks
    OBRootTask::Instance().InitThat(*this);

    // expand all items in the treeCtlr
    ExpandAll();
}

void OBBackupTree::OnItemActivated(wxTreeEvent& event)
{
    // get the data object behind the tree-item-control
    OBBackupTreeItemData* pData = (OBBackupTreeItemData*)GetItemData(event.GetItem());

    // it is a no data holding item
    if (pData == NULL)
        return;

    // get the task object from the data layer
    OBTask* pTask = NULL;

    if (pData->GetOID() != OBInvalidOID)
         pTask = OBRootTask::Instance().GetTask(pData->GetOID());
    else
        return;

    if (pTask == NULL)
    {
        OBSystem::Fatal(_("no task object found for the specified OID"), _T("OBBackupTree::OnItemActivated()"));
        return;
    }

    // show the task
    OBTaskDlg(GetParent(), *pTask);
}

void OBBackupTree::OnItemMenu(wxTreeEvent& event)
{
    wxPoint         point   (event.GetPoint());
    wxTreeItemId    item    (event.GetItem());
    wxMenu          menu;

    // right click on root item
    if (GetRootItem() == item)
    {
        wxMenuItem  mItem1(&menu, OBID_BACKUPCTRL_ADDDESTINATION, _("add destination directory"));

        menu.Append(&mItem1);

        PopupMenu(&menu, point);
    }
}

/*virtual*/ wxDragResult OBBackupTree::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    wxTreeItemId    itemId(HitTest(wxPoint(x, y)));

    // clean all selections
    UnselectAll();

    // is an item selected?
    if ( itemId.IsOk() )
    {
        // get the imageId of the selected item
        int iImageId(GetItemImage(itemId));

        // identify the item
        if (iImageId == OBICON_DIR
         || iImageId == OBICON_VOLUME)
        {
            // select it if it is not
            if (GetSelection() != itemId)
                SelectItem( itemId );
        }
    }

    return def;
}

bool OBBackupTree::OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (filenames.Count() == 0)
        return false;

    wxMenu                  menu;
    wxMenuItem*             pItem       = NULL;
    int                     idBmp, idItem;
    wxString                str;

    // ** copy entry **
    if (wxDir::Exists(filenames[0]))
    {
        idBmp   = OBICON_TASKDC;
        idItem  = OBID_BACKUPCTRL_COPY_DIR;
        str     = _("copy directory");
    }
    else
    {
        idBmp   = OBICON_TASKFC;
        idItem  = OBID_BACKUPCTRL_COPY_FILE;
        str     = _("copy file");
    }

    pItem = new wxMenuItem(&menu, idItem, str);
    pItem->SetBitmap(imageList_.GetBitmap(idBmp));
    menu.Append(pItem);

    // remember the filename for use in other methodes
    strDropedFilename_ = filenames[0];

    // remember the destination directory (by draging over it) if ther is one
    OBBackupTreeItemData*   pItemData   = NULL;
    wxTreeItemId            itemId      = HitTest(wxPoint(x, y));

    if ( itemId.IsOk() )
        pItemData = (OBBackupTreeItemData*)GetItemData(itemId);

    if ( pItemData
      && pItemData->GetOID() == OBInvalidOID)
        strCurrentDestination_ = pItemData->GetPath();

    // show the menu
    PopupMenu(&menu, wxPoint(x, y));

    return true;
}

void OBBackupTree::OnAddDestination (wxCommandEvent& event)
{
    // create dir dialog
    wxDirDialog dlg(this, _T("choose a destination directory"));

    // let the user choose a dir
    if (dlg.ShowModal() != wxID_OK)
        return;

    // add the destination dir to the tree
    SelectItem(AddDestination (dlg.GetPath()));
}

wxTreeItemId OBBackupTree::AddDestination (wxString strPath)
{
    // add root
    wxTreeItemId    idLast, idCurr;
    wxString        strCurr;

    wxStringTokenizer tkz(strPath, wxFILE_SEP_PATH);
    idLast = GetRootItem();

    while ( tkz.HasMoreTokens() )
    {
        idCurr  = idLast;
        strCurr = tkz.GetNextToken();
        idLast  = FindItem(idCurr, strCurr, false);

        if ( idLast.IsOk() )
        {
            idCurr = idLast;
            continue;
        }

        // it is a volume
        if (strCurr[1] == ':')
        {
            idLast = AppendItem
                     (
                        idCurr,
                        strCurr,
                        OBICON_VOLUME,
                        -1,
                        new OBBackupTreeItemData ( OBInvalidOID, strPath.Left(strPath.Find(strCurr) + strCurr.Len()) )
                     );
            continue;
        }

        // it is a directory
        idLast = AppendItem
                 (
                    idCurr,
                    strCurr,
                    OBICON_DIR,
                    -1,
                    new OBBackupTreeItemData ( OBInvalidOID, strPath.Left(strPath.Find(strCurr) + strCurr.Len()) )
                 );
    }   // while(token)

    return idLast;
}

void OBBackupTree::OnBackupCopy (wxCommandEvent& event)
{
    OBTask*         pTask       = NULL;
    OBTaskType      type;
    bool            bVerify     = true /* DEBUG: get default value */;
    wxArrayString   arrString;

    switch (event.GetId())
    {
        case OBID_BACKUPCTRL_COPY_DIR:
            type = TaskDIRCOPY;
            OBSystem::Info(_T("copy dir"));
            break;

        case OBID_BACKUPCTRL_COPY_FILE:
            type = TaskFILECOPY;
            break;

        default:
            OBSystem::Error((_T("copy ERROR")), _T("OBBackupTree::OnBackupCopy()"));
            return;
            break;
    };  // switch

    // create and show the new task
    pTask = new OBTask (type,
                        strDropedFilename_,
                        strCurrentDestination_,
                        strDropedFilename_.AfterLast(wxFILE_SEP_PATH),
                        true /* DEBUG */,
                        CompressNOTUSED,
                        arrString);
    OBTaskDlg(GetParent(), *pTask);

    // the remembered filename and destination is not needed anymore
    strDropedFilename_      = wxEmptyString;
    strCurrentDestination_  = wxEmptyString;

    // refresh the backup tree view
    OBRootTask::Instance().broadcastObservers();
}

void OBBackupTree::OnBeginLabelEdit (wxTreeEvent& event)
{
    if (event.GetItem() != GetRootItem())
        event.Veto();
}

void OBBackupTree::OnEndLabelEdit (wxTreeEvent& event)
{
    if (event.GetItem() == GetRootItem())
        OBRootTask::Instance().SetName(event.GetLabel());
}

/*virtual*/ void OBBackupTree::ValueChanged (Subject* pSender)
{
    if ( pSender != &(OBRootTask::Instance()) )
        return;

    Init();
}

OBTask* OBBackupTree::GetTaskByItem (wxTreeItemId itemId)
{
    if ( !(itemId.IsOk()) )
        return NULL;

    // get the data object behind the item
    // because it holds the taskOID
    OBBackupTreeItemData* pItemData = (OBBackupTreeItemData*)GetItemData(itemId);

    if (pItemData == NULL)
        return NULL;

    // get the task object from the data layer
    return OBRootTask::Instance().GetTask(pItemData->GetOID());
}

wxTreeItemId OBBackupTree::FindItem (wxTreeItemId idStart, const wxChar* label, bool bGoDeep /*= true*/)
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

wxTreeItemId OBBackupTree::FindItem (const wxChar* label)
{
    if (label == NULL)
        return wxTreeItemId();

    return FindItem(GetRootItem(), label);
}

OBBackupTreeItemData::OBBackupTreeItemData (OBoid oid, const wxChar* strPath /*= NULL*/)
                : oid_(oid),
                  strPath_(strPath)
{
}

/*virtual*/ OBBackupTreeItemData::~OBBackupTreeItemData ()
{
}

OBoid OBBackupTreeItemData::GetOID ()
{
    return oid_;
}

const wxChar* OBBackupTreeItemData::GetPath ()
{
    return strPath_.c_str();
}

OBBackupTree::OBBackupDropTarget::OBBackupDropTarget (OBBackupTree* pBackupTree)
                                   : pBackupTree_(pBackupTree)
{ }

OBBackupTree::OBBackupDropTarget::~OBBackupDropTarget ()
{ }

bool OBBackupTree::OBBackupDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (pBackupTree_ == NULL)
        return false;

    return pBackupTree_->OnDropFiles(x, y, filenames);
}

wxDragResult OBBackupTree::OBBackupDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    if (pBackupTree_ == NULL)
        return def;

    return pBackupTree_->OnDragOver(x, y, def);
}
