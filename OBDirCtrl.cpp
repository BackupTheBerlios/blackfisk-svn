/**
 * implementation of class OBDirCtrl
 * 2006-05-24
 ***/

#include "OBDirCtrl.h"
#include <wx/dnd.h>

BEGIN_EVENT_TABLE(OBDirCtrl, wxPanel)
  EVT_BUTTON  (OBDIRCTRL_ID_FILEBUTTON, OBDirCtrl::OnButton_DirCtrl)
END_EVENT_TABLE()

//
OBDirCtrl::OBDirCtrl (wxWindow* pParent)
         : wxPanel(pParent, -1)
{
    // DirCtrl
    pDirCtrl_ = new wxGenericDirCtrl(this);
    pDirCtrl_->SetWindowStyle(pDirCtrl_->GetWindowStyle() & wxDIRCTRL_DIR_ONLY);

    // button to show files in the DirCtrl
    pButtonFiles_ = new wxButton(this, OBDIRCTRL_ID_FILEBUTTON, OBDIRCTRL_FILEBUTTON_ONLYDIRS);

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
        wxTreeEventHandler (OBDirCtrl::OnBeginDrag),
        NULL,
        this
    );

    pDirCtrl_->GetTreeCtrl()->Connect
    (
        wxID_ANY,
        wxEVT_COMMAND_TREE_ITEM_MENU,
        wxTreeEventHandler (OBDirCtrl::OnItemMenu),
        NULL,
        this
    );
}


/*virtual*/ OBDirCtrl::~OBDirCtrl ()
{
}


void OBDirCtrl::OnButton_DirCtrl (wxCommandEvent& rEvent)
{
    long style = pDirCtrl_->GetWindowStyle();
    if ( (style & wxDIRCTRL_DIR_ONLY) == 0 )  // show dirs only ?
    {
        style = style | wxDIRCTRL_DIR_ONLY;
        pButtonFiles_->SetLabel(OBDIRCTRL_FILEBUTTON_WITHFILES);
    }
    else
    {
        style = style & ~wxDIRCTRL_DIR_ONLY;
        pButtonFiles_->SetLabel(OBDIRCTRL_FILEBUTTON_ONLYDIRS);
    }

    pDirCtrl_->SetWindowStyle(style);
    wxString strPath(pDirCtrl_->GetPath());
    pDirCtrl_->CollapsePath(strPath);
    pDirCtrl_->ExpandPath(strPath);

    pButtonFiles_->SetSize(pButtonFiles_->GetBestSize());
}


void OBDirCtrl::OnBeginDrag (wxTreeEvent& event)
{
    wxFileDataObject    my_data;
    wxDropSource        dragSource  ( this );

    // selected the now draging item
    pDirCtrl_->GetTreeCtrl()->SelectItem(event.GetItem());
    my_data.AddFile(pDirCtrl_->GetPath());
    dragSource.SetData(my_data);

    wxDragResult result = dragSource.DoDragDrop( TRUE );
}


void OBDirCtrl::OnItemMenu (wxTreeEvent& event)
{
    wxPoint         point   (event.GetPoint());
    wxTreeItemId    item    (event.GetItem());
    wxMenu          menu;

    wxMenuItem  mItem1(&menu, wxID_ANY, _("add destination directory"));

    menu.Append(&mItem1);

    pDirCtrl_->PopupMenu(&menu, point);
}

