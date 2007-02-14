/**
 * implementation of class OBMessageWindow
 * 2006-04-06
 ***/

#include "OBMessageWindow.h"
#include <wx/listbox.h>

//
OBMessageWindow::OBMessageWindow (wxWindow* pParent)
             : wxWindow (pParent, -1)
{
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    pListBox_ = new wxListBox(this, -1);
    pSizer->Add( pListBox_, wxSizerFlags(1).Expand().Border(wxALL, 5) );
    SetSizer(pSizer);
    Show(TRUE);
}


//
/*virtual*/ OBMessageWindow::~OBMessageWindow ()
{
}


void OBMessageWindow::Add (wxString strMsg)
{
    pListBox_->Append(strMsg);
}
