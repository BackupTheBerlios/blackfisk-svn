/**
 * implementation of class BFIconTable
 * 2007-03-27
 ***/

#include "BFIconTable.h"

#include <wx/artprov.h>

#include "blackfisk.h"


/*static*/ BFIconTable* BFIconTable::psIconTable_ = NULL;

/*static*/ BFIconTable* BFIconTable::Instance ()
{
    if (psIconTable_ == NULL)
        psIconTable_ = new BFIconTable();

    return psIconTable_;
}


BFIconTable::BFIconTable ()
           : wxImageList(16, 16)
{
    Init();
}

/*virtual*/ BFIconTable::~BFIconTable ()
{
    if (psIconTable_ != NULL)
        delete psIconTable_;
}

void BFIconTable::Init ()
{
    RemoveAll();
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("logo.ico")), wxBITMAP_TYPE_ICO));
    Add(wxArtProvider::GetIcon(wxART_HARDDISK));
    Add(wxArtProvider::GetIcon(wxART_FLOPPY));
    Add(wxArtProvider::GetIcon(wxART_CDROM));
    Add(wxArtProvider::GetIcon(wxART_REMOVABLE));
    Add(wxArtProvider::GetIcon(wxART_FOLDER));
    Add(wxArtProvider::GetIcon(wxART_FOLDER_OPEN));
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_virtual.ico")), wxBITMAP_TYPE_ICO));
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_virtual_open.ico")), wxBITMAP_TYPE_ICO));
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_dc.ico")),   wxBITMAP_TYPE_ICO));
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_fc.ico")),   wxBITMAP_TYPE_ICO));
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_zip.ico")),  wxBITMAP_TYPE_ICO));
}
