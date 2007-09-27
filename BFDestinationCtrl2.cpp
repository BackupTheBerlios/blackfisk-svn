/**
 * Name:        BFDestinationCtrl2.cpp
 * Purpose:     BFDestinationCtrl2 class implementation
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

#include "BFDestinationCtrl2.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "BFSystem.h"
#include "BFTaskDlg.h"
#include "BFMainFrame.h"
#include "BFPlaceholderButton.h"
#include "ctrlids.h"

//
BFDestinationCtrl2::BFDestinationCtrl2 (wxWindow*     pParent,
                                      const wxChar* strPath /*= wxEmptyString*/,
                                      bool          bWithLabel /*= true*/)
                 : wxBoxSizer(wxHORIZONTAL),
                   pPickerCtrl_(NULL)
{
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

    // arrange/layout panel
    wxBoxSizer* pSubSizer = new wxBoxSizer(wxHORIZONTAL);
    pSubSizer->Add(pPickerCtrl_,        wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Expand());
    pSubSizer->Add(pPlaceholderButton,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));

    // label
    wxStaticText* pLabel = NULL;
    if (bWithLabel)
        pLabel = CreateLabel(pParent);

    // set size
    //if (pLabel)
    //    pLabel->SetMinSize(wxSize(BFTaskDlg::lWidth1_, pLabel->GetSize().GetHeight()));
    //pPanel->SetMinSize(wxSize(BFTaskDlg::lWidth2_, pPanel->GetSize().GetHeight()+3));

    // arrange/layout the main control
    if (pLabel)
        this->Add(pLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    this->Add(pSubSizer, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Expand());

    pPickerCtrl_->SetPath(strPath);
}

//
/*virtual*/ BFDestinationCtrl2::~BFDestinationCtrl2 ()
{
}

wxStaticText* BFDestinationCtrl2::CreateLabel(wxWindow* pParent)
{
    return new wxStaticText (pParent, -1, _("destination path:"));
}

wxString BFDestinationCtrl2::GetPath ()
{
    return pPickerCtrl_->GetPath();
}

void BFDestinationCtrl2::SetPath (const wxString& strPath)
{
    pPickerCtrl_->SetPath(strPath);
}
