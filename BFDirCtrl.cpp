/**
 * implementation of class BFDirCtrl
 * 2006-05-24
 ***/

#include "BFDirCtrl.h"

#include <wx/dnd.h>

#include "BFBackupTree.h"
#include "BFMainFrame.h"

BEGIN_EVENT_TABLE(BFDirCtrl, wxPanel)
  EVT_BUTTON  (BFDIRCTRL_ID_FILEBUTTON,         BFDirCtrl::OnButton_DirCtrl)
  EVT_MENU    (BFDIRCTRL_ID_ADDDESTINATION,     BFDirCtrl::OnAddAsDestination)
END_EVENT_TABLE()

//
BFDirCtrl::BFDirCtrl (wxWindow* pParent)
         : wxPanel(pParent, -1)
{
    // DirCtrl
    pDirCtrl_ = new wxGenericDirCtrl(this);
    pDirCtrl_->SetWindowStyle(pDirCtrl_->GetWindowStyle() & wxDIRCTRL_DIR_ONLY);

    // button to show files in the DirCtrl
    pButtonFiles_ = new wxButton(this, BFDIRCTRL_ID_FILEBUTTON, BFDIRCTRL_FILEBUTTON_ONLYDIRS);

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
    if ( (style & wxDIRCTRL_DIR_ONLY) == 0 )  // show dirs only ?
    {
        style = style | wxDIRCTRL_DIR_ONLY;
        pButtonFiles_->SetLabel(BFDIRCTRL_FILEBUTTON_WITHFILES);
    }
    else
    {
        style = style & ~wxDIRCTRL_DIR_ONLY;
        pButtonFiles_->SetLabel(BFDIRCTRL_FILEBUTTON_ONLYDIRS);
    }

    pDirCtrl_->SetWindowStyle(style);
    wxString strPath(pDirCtrl_->GetPath());
    pDirCtrl_->CollapsePath(strPath);
    pDirCtrl_->ExpandPath(strPath);

    pButtonFiles_->SetSize(pButtonFiles_->GetBestSize());
}


void BFDirCtrl::OnBeginDrag (wxTreeEvent& event)
{
    wxFileDataObject    my_data;
    wxDropSource        dragSource  ( this );

    // selected the now draging item
    pDirCtrl_->GetTreeCtrl()->SelectItem(event.GetItem());
    my_data.AddFile(pDirCtrl_->GetPath());
    dragSource.SetData(my_data);

    wxDragResult result = dragSource.DoDragDrop( TRUE );
}


void BFDirCtrl::OnItemMenu (wxTreeEvent& event)
{
    // remember this selected item
    lastItemId_ = event.GetItem();

    // create a menu
    wxMenu menu;

    // add a item to the menu
    wxMenuItem  mItem1(&menu, BFDIRCTRL_ID_ADDDESTINATION, _("add as destination"));
    menu.Append(&mItem1);

    // popup the menu
    pDirCtrl_->PopupMenu(&menu, event.GetPoint());
}

void BFDirCtrl::OnAddAsDestination (wxCommandEvent& event)
{
    // get the backup tree object
    BFBackupTree* pBackupTree = BFMainFrame::Instance()->BackupTree();

    if (pBackupTree == NULL)
    {
        BFSystem::Fatal(_("no backup tree available (pBackupTree == NULL)"), _T("BFDirCtrl::OnAddAsDestination()"));
        return;
    }

    // get selected item in the dir control
    wxDirItemData* pDirItem = dynamic_cast<wxDirItemData*>(pDirCtrl_->GetTreeCtrl()->GetItemData(lastItemId_));

    if (pDirItem == NULL)
        return;

    // check if it is a dir
    if ( !(pDirItem->m_isDir) )
    {
        BFSystem::Info(_("It is not possible to add a file as a destination!\nYou can just add a directory as a backup destination."));
        return;
    }

    // set and select the new added destination
    pBackupTree->SelectItem(pBackupTree->AddDestination(pDirItem->m_path));
}

