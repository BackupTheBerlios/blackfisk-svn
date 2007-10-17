/**
 * Name:        BFIconTable.cpp
 * Purpose:     BFIconTable class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-27
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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
    wxImage::AddHandler(new wxPNGHandler());
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
    Add(wxIcon(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_syn.ico")),  wxBITMAP_TYPE_ICO));
}
