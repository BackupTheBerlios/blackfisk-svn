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
           : imgFilesystem_(16, 16),
             imgButtons_(16, 16),
             imgMessage_(64, 64)
{
    wxInitAllImageHandlers();
    //wxImage::AddHandler(new wxPNGHandler());
    Init();
}

/*virtual*/ BFIconTable::~BFIconTable ()
{
}

void BFIconTable::Init ()
{
    // filesystem icons
    imgFilesystem_.RemoveAll();
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("logo16.png")), wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_HARDDISK));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_FLOPPY));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_CDROM));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_REMOVABLE));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_closed.png")), wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_open.png")), wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_virtual_closed.png")), wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("folder_virtual_open.png")), wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_dc.png")),   wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_fc.png")),   wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_ar.png")),  wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("task_sy.png")),  wxBITMAP_TYPE_PNG));

    // button icons
    imgButtons_.RemoveAll();
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "yes16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "no16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "yes16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "no16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "stop_prj16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "stop_task16.png"), wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "ignore16.png"), wxBITMAP_TYPE_PNG));

    // message icons
    imgMessage_.RemoveAll();
    imgMessage_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "question64.png"), wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "info64.png"), wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "warning64.png"), wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "error64.png"), wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "fatal64.png"), wxBITMAP_TYPE_PNG));
}

wxImageList* BFIconTable::GetFilesystemImageList ()
{
    return &imgFilesystem_;
}

wxBitmap BFIconTable::GetBitmap (BFButtonIconId id) const
{
    return imgButtons_.GetBitmap(id);
}

wxBitmap BFIconTable::GetBitmap (BFMessageIconId id) const
{
    return imgMessage_.GetBitmap(id);
}

wxBitmap BFIconTable::GetBitmap (BFFilesystemIconId id) const
{
    return imgFilesystem_.GetBitmap(id);
}

wxIcon BFIconTable::GetIcon (BFButtonIconId id) const
{
    return imgButtons_.GetIcon(id);
}

wxIcon BFIconTable::GetIcon (BFMessageIconId id) const
{
    return imgMessage_.GetIcon(id);
}

wxIcon BFIconTable::GetIcon (BFFilesystemIconId id) const
{
    return imgFilesystem_.GetIcon(id);
}
