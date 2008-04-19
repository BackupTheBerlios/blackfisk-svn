/**
 * Name:        BFPlaceholderButton.cpp
 * Purpose:     BFPlaceholderButton class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-18
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

#ifndef BFPLACEHOLDERBUTTON_H
#define BFPLACEHOLDERBUTTON_H

#include <wx/button.h>
#include <wx/textctrl.h>

///
class BFPlaceholderButton : public wxButton
{
    private:
        ///
        wxTextCtrl&         rTextCtrl_;

    public:
        /// constructor
        BFPlaceholderButton (wxWindow* pParent, wxTextCtrl& rTextCtrl);

        /// virtual destructor
        virtual ~BFPlaceholderButton ();

        ///
        void OnPressed(wxCommandEvent& rEvent);
        ///
        void OnPlaceholder(wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFPlaceholderButton

#endif    // BFPLACEHOLDERBUTTON_H
