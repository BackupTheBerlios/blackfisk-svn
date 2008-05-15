/**
 * Name:        BFDestinationCtrl.cpp
 * Purpose:     BFDestinationCtrl class implementation
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

#include "BFDestinationCtrl.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>

#include "BFSystem.h"
#include "BFTaskDlg.h"
#include "BFMainFrame.h"
#include "BFPlaceholderButton.h"
#include "BFHelpCtrl.h"

//
BFDestinationCtrl::BFDestinationCtrl (wxWindow*     pParent,
                                      const wxChar* strPath /*= wxEmptyString*/,
                                      bool          bWithLabel /*= true*/)
                 : wxBoxSizer(wxHORIZONTAL),
                   pLabel_(NULL),
                   pPickerCtrl_(NULL)
{
    // label
    if (bWithLabel)
        pLabel_ = new wxStaticText (pParent, -1, _("destination path:"));

    // control
    pPickerCtrl_ = new wxDirPickerCtrl
                   (
                        pParent,
                        wxID_ANY,
                        wxEmptyString,
                        _("select a destination folder"),
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxDIRP_USE_TEXTCTRL
                    );
    pPickerCtrl_->SetTextCtrlProportion(4);
    pPickerCtrl_->SetPickerCtrlProportion(1);

    //
    wxButton* pPlaceholderButton = new BFPlaceholderButton(pParent, *(pPickerCtrl_->GetTextCtrl()));

    // arrange/layout
    if (pLabel_)
        Add(pLabel_, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));

    Add(pPickerCtrl_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Expand());

    Add(pPlaceholderButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));

    // set path
    pPickerCtrl_->SetPath(strPath);
}

//
/*virtual*/ BFDestinationCtrl::~BFDestinationCtrl ()
{
}

wxString BFDestinationCtrl::GetPath ()
{
    return pPickerCtrl_->GetPath();
}

void BFDestinationCtrl::SetPath (const wxString& strPath)
{
    pPickerCtrl_->SetPath(strPath);
}

void BFDestinationCtrl::ConnectHelpText (BFHelpCtrl* pHelpCtrl, const wxString& strHelp)
{
    if (pLabel_)
        pHelpCtrl->Connect(pLabel_, strHelp);

    pHelpCtrl->Connect(pPickerCtrl_, strHelp);
}
