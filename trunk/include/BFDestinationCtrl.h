/**
 * Name:        BFDestinationCtrl.cpp
 * Purpose:     BFDestinationCtrl class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-28
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

#ifndef BFDESTINATIONCTRL_H
#define BFDESTINATIONCTRL_H

#include <wx/wx.h>
#include <wx/sizer.h>

class wxDirPickerCtrl;
class wxStaticText;
class BFHelpCtrl;

/// a control to edit destinations
class BFDestinationCtrl : public wxBoxSizer
{
    private:
        ///
        wxStaticText*           pLabel_;
        ///
        wxDirPickerCtrl*        pPickerCtrl_;

        ///
        BFDestinationCtrl ();

    public:
        /// constructor
        BFDestinationCtrl (wxWindow* pParent, const wxChar* strPath = wxEmptyString, bool bWithLabel = true);

        /// virtual destructor
        virtual ~BFDestinationCtrl ();

        ///
        wxString GetPath ();
        ///
        void SetPath (const wxString& strPath);

        ///
        void ConnectHelpText (BFHelpCtrl* pHelpCtrl, const wxString& strHelp);
};    // class BFDestinationCtrl

#endif    // BFDESTINATIONCTRL_H
