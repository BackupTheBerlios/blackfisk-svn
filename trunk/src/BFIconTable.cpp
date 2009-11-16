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

#include "BFEnvironment.h"


/*static*/ BFIconTable BFIconTable::sIconTable_;

/*static*/ BFIconTable& BFIconTable::Instance ()
{
    if ( sIconTable_.imgFilesystem_.GetImageCount() == 0 )
        sIconTable_.Init();

    return sIconTable_;
}


BFIconTable::BFIconTable ()
           : imgFilesystem_(16, 16),
             imgButtons_(16, 16),
             imgMessage_(64, 64)
{
    wxImage::AddHandler(new wxPNGHandler());
    wxImage::AddHandler(new wxICOHandler());
    //Init();
}

/*virtual*/ BFIconTable::~BFIconTable ()
{
}

void BFIconTable::Init ()
{
    // filesystem icons
    imgFilesystem_.RemoveAll();
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "logo16.png", wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_HARDDISK));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_FLOPPY));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_CDROM));
    imgFilesystem_.Add(wxArtProvider::GetIcon(wxART_REMOVABLE));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "folder_closed.png",           wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "folder_open.png",             wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "folder_virtual_closed.png",   wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "folder_virtual_open.png",     wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "task_dc.png",                 wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "task_fc.png",                 wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "task_ar.png",                 wxBITMAP_TYPE_PNG));
    imgFilesystem_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "task_sy.png",                 wxBITMAP_TYPE_PNG));

    // button icons
    imgButtons_.RemoveAll();
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "yes16.png",      wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "no16.png",       wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "yes16.png",      wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "no16.png",       wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "stop_prj16.png", wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "stop_task16.png",wxBITMAP_TYPE_PNG));
    imgButtons_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "ignore16.png",   wxBITMAP_TYPE_PNG));

    // message icons
    imgMessage_.RemoveAll();
    imgMessage_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "question64.png", wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "info64.png",     wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "warning64.png",  wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "error64.png",    wxBITMAP_TYPE_PNG));
    imgMessage_.Add(wxBitmap(BFEnvironment::GetGraphicDir() + "fatal64.png",    wxBITMAP_TYPE_PNG));
}

/*static*/ wxIcon BFIconTable::GetMainFrameIcon ()
{
    return wxIcon(BFEnvironment::GetGraphicDir() + "logo.ico", wxBITMAP_TYPE_ICO);
}

wxImageList* BFIconTable::GetFilesystemImageList ()
{
    return &imgFilesystem_;
}

/*static*/ wxBitmap BFIconTable::GetBitmap (BFButtonIconId id)
{
    return Instance().imgButtons_.GetBitmap(id);
}

/*static*/ wxBitmap BFIconTable::GetBitmap (BFMessageIconId id)
{
    return Instance().imgMessage_.GetBitmap(id);
}

/*static*/ wxBitmap BFIconTable::GetBitmap (BFFilesystemIconId id)
{
    return Instance().imgFilesystem_.GetBitmap(id);
}

/*static*/ wxIcon BFIconTable::GetIcon (BFButtonIconId id)
{
    return Instance().imgButtons_.GetIcon(id);
}

/*static*/ wxIcon BFIconTable::GetIcon (BFMessageIconId id)
{
    return Instance().imgMessage_.GetIcon(id);
}

/*static*/ wxIcon BFIconTable::GetIcon (BFFilesystemIconId id)
{
    return Instance().imgFilesystem_.GetIcon(id);
}
