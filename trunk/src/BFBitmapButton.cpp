/**
 * Name:        BFBitmapButton.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-05-14
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#include "BFBitmapButton.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>

//
BFBitmapButton::BFBitmapButton (wxWindow* pParent,
                                wxWindowID id,
                                const wxBitmap& rBitmap,
                                const wxString& strLabel,
                                const wxPoint& pos /*= wxDefaultPosition*/,
                                const wxSize& size /*= wxDefaultSize*/,
                                long style /*= wxBU_AUTODRAW*/,
                                const wxValidator& validator /*= wxDefaultValidator*/,
                                const wxString& name /*= wxButtonNameStr*/)
              : wxBitmapButton(pParent,
                               id,
                               wxBitmap(),
                               pos,
                               size,
                               style,
                               validator,
                               name)
{
    // string size
    wxWindowDC* pWindowDC = new wxWindowDC(this);
    wxSize sizeText = pWindowDC->GetTextExtent(wxString::Format("%sWW", strLabel));

    // load bitmap
    wxBitmap bmp(rBitmap.GetWidth() + sizeText.GetWidth(),
                 rBitmap.GetHeight() + sizeText.GetHeight());

    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), wxBRUSHSTYLE_SOLID));
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.Clear();
    dc.DrawLabel(strLabel,
                 rBitmap,
                 wxRect(0,
                        0,
                        bmp.GetWidth(),
                        bmp.GetHeight()),
                 wxALIGN_CENTRE);

    SetBitmapLabel(bmp);
}


//
/*virtual*/ BFBitmapButton::~BFBitmapButton ()
{
}


