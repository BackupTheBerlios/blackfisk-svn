/**
 * implementation of class BFBackupCtrl
 * 2006-06-12
 ***/

#include "BFBackupCtrl.h"
#include "BFBackupTree.h"
#include <wx/image.h>


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
    BFBackupTree* pBackupTree = new BFBackupTree(this);

    // arange
    pTopSizer->Add(pTool);
    pTopSizer->Add(pBackupTree, wxSizerFlags(1).Expand());
    SetSizer(pTopSizer);
}

/*virtual*/ BFBackupCtrl::~BFBackupCtrl ()
{
}


