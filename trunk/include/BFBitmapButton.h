/**
 * Name:        BFBitmapButton.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-05-14
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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


#ifndef BFBITMAPBUTTON_H
#define BFBITMAPBUTTON_H

#include <wx/bmpbuttn.h>

///
class BFBitmapButton : public wxBitmapButton
{
    public:
        /// ctor
        BFBitmapButton (wxWindow* pParent,
                        wxWindowID id,
                        const wxBitmap& rBitmap,
                        const wxString& strLabel,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxBU_AUTODRAW,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxButtonNameStr);

        /// virtual dtor
        virtual ~BFBitmapButton ();
};

#endif    // BFBITMAPBUTTON_H
