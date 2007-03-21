/**
 * implementation of class BFBackupTree
 * 2006-05-25
 ***/

#include "blackfisk.h"
#include "BFApp.h"
#include "BFMainFrame.h"
#include "BFTask.h"
#include "BFBackupTree.h"
#include "BFRootTask.h"
#include "BFTaskDlg.h"
#include <wx/icon.h>
#include <wx/dnd.h>
#include <wx/tokenzr.h>
#include <wx/image.h>

BEGIN_EVENT_TABLE(BFBackupTree, wxTreeCtrl)
    EVT_TREE_ITEM_ACTIVATED     (wxID_ANY,                          BFBackupTree::OnItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK   (wxID_ANY,                          BFBackupTree::OnItemMenu)
    EVT_MENU                    (BFID_BACKUPCTRL_ADDDESTINATION,    BFBackupTree::OnAddDestination)
    EVT_MENU                    (BFID_BACKUPCTRL_COPY_DIR,          BFBackupTree::OnBackupCopy)
    EVT_MENU                    (BFID_BACKUPCTRL_COPY_FILE,         BFBackupTree::OnBackupCopy)
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
              Observer(&(BFRootTask::Instance()))
{
    SetImageList ( &(BFMainFrame::Instance()->GetImageList()) );
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
}

void BFBackupTree::OnItemActivated(wxTreeEvent& event)
{
    // check if it is a data object behind the tree-item-control
    if ( GetItemData(event.GetItem()) == NULL)
        return;

    // get the task object from the data layer
    BFTask* pTask = GetTaskByItem(event.GetItem());

    if (pTask == NULL)
    {
        BFSystem::Fatal(_("no task object found for the specified OID"), _T("BFBackupTree::OnItemActivated()"));
        return;
    }

    BFTaskBaseDlg::Show (pTask);
}

void BFBackupTree::OnItemMenu(wxTreeEvent& event)
{
    wxPoint         point   (event.GetPoint());
    wxTreeItemId    item    (event.GetItem());
    wxMenu          menu;

    // right click on root item
    if (GetRootItem() == item)
    {
        wxMenuItem  mItem1(&menu, BFID_BACKUPCTRL_ADDDESTINATION, _("add destination directory"));

        menu.Append(&mItem1);

        PopupMenu(&menu, point);
    }
}

/*virtual*/ wxDragResult BFBackupTree::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
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
        if (iImageId == BFICON_DIR
         || iImageId == BFICON_VOLUME)
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
    wxMenuItem*             pItem       = NULL;
    int                     idBmp, idItem;
    wxString                str;

    // ** copy entry **
    if (wxDir::Exists(filenames[0]))
    {
        idBmp   = BFICON_TASKDC;
        idItem  = BFID_BACKUPCTRL_COPY_DIR;
        str     = _("copy directory");
    }
    else
    {
        idBmp   = BFICON_TASKFC;
        idItem  = BFID_BACKUPCTRL_COPY_FILE;
        str     = _("copy file");
    }

    pItem = new wxMenuItem(&menu, idItem, str);
    pItem->SetBitmap(BFMainFrame::Instance()->GetImageList().GetBitmap(idBmp));
    menu.Append(pItem);

    // remember the filename for use in other methodes
    strDropedFilename_ = filenames[0];

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

void BFBackupTree::OnAddDestination (wxCommandEvent& event)
{
    // create dir dialog
    wxDirDialog dlg(this, _T("choose a destination directory"));

    // let the user choose a dir
    if (dlg.ShowModal() != wxID_OK)
        return;

    // add the destination dir to the tree
    SelectItem(AddDestination (dlg.GetPath()));
}

wxTreeItemId BFBackupTree::AddDestination (wxString strPath)
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
                        BFICON_VOLUME,
                        -1,
                        new BFBackupTreeItemData ( BFInvalidOID, strPath.Left(strPath.Find(strCurr) + strCurr.Len()) )
                     );
            continue;
        }

        // it is a directory
        idLast = AppendItem
                 (
                    idCurr,
                    strCurr,
                    BFICON_DIR,
                    -1,
                    new BFBackupTreeItemData ( BFInvalidOID, strPath.Left(strPath.Find(strCurr) + strCurr.Len()) )
                 );
    }   // while(token)

    return idLast;
}

void BFBackupTree::OnBackupCopy (wxCommandEvent& event)
{
    BFTask*         pTask       = NULL;
    BFTaskType      type;
    bool            bVerify     = true /* DEBUG: get default value */;
    wxArrayString   arrString;

    switch (event.GetId())
    {
        case BFID_BACKUPCTRL_COPY_DIR:
            type = TaskDIRCOPY;
            BFSystem::Info(_T("copy dir"));
            break;

        case BFID_BACKUPCTRL_COPY_FILE:
            type = TaskFILECOPY;
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
    BFTaskBaseDlg::Show(pTask);

    // the remembered filename and destination is not needed anymore
    strDropedFilename_      = wxEmptyString;
    strCurrentDestination_  = wxEmptyString;

    // refresh the backup tree view
    BFRootTask::Instance().broadcastObservers();
}

void BFBackupTree::OnBeginLabelEdit (wxTreeEvent& event)
{
    if (event.GetItem() != GetRootItem())
        event.Veto();
}

void BFBackupTree::OnEndLabelEdit (wxTreeEvent& event)
{
    if (event.GetItem() == GetRootItem())
        BFRootTask::Instance().SetName(event.GetLabel());
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
    BFBackupTreeItemData* pItemData = (BFBackupTreeItemData*)GetItemData(itemId);

    if (pItemData == NULL)
        return NULL;

    // get the task object from the data layer
    return BFRootTask::Instance().GetTask(pItemData->GetOID());
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




BFBackupCtrl::BFBackupCtrl (wxWindow* pParent)
            : wxPanel(pParent, -1)
{
    // init sizer
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

    // toolbar
    wxToolBar* pTool = new wxToolBar(this, -1);
    pTool->AddTool(-1, _T("label 1"), wxBitmap(_T("graphic\\dir.bmp"), wxBITMAP_TYPE_BMP));
    pTool->AddSeparator();
    pTool->AddTool(-1, _T("label 2"), wxBitmap(_T("graphic\\dir.bmp"), wxBITMAP_TYPE_BMP));
    pTool->Realize();
    pTool->SetMinSize(wxSize(pTool->GetSize().GetWidth()*8, 26));

    // init controls
    pBackupTree_ = new BFBackupTree(this);

    // arange
    pTopSizer->Add(pTool);
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
