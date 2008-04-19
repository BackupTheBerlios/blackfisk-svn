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

#include "BFSystem.h"
#include "BFTaskDlg.h"
#include "BFMainFrame.h"
#include "BFPlaceholderButton.h"
#include "ctrlids.h"

//
BFDestinationCtrl::BFDestinationCtrl (wxWindow*     pParent,
                                      const wxChar* strPath /*= wxEmptyString*/,
                                      bool          bWithLabel /*= true*/)
                 : wxPanel(pParent),
                   pPickerCtrl_(NULL)
{
    // panel
    wxPanel* pPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // control
    pPickerCtrl_ = new wxDirPickerCtrl
                   (
                        pPanel,
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
    wxButton* pPlaceholderButton = new BFPlaceholderButton(pPanel, *(pPickerCtrl_->GetTextCtrl()));

    // arrange/layout panel
    wxBoxSizer* pSubSizer = new wxBoxSizer(wxHORIZONTAL);
    pSubSizer->Add(pPickerCtrl_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Expand());
    pSubSizer->Add(pPlaceholderButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPanel->SetSizer(pSubSizer);

    // label
    wxStaticText* pLabel = NULL;
    if (bWithLabel)
        pLabel = CreateLabel(this);

    // set size
    if (pLabel)
        pLabel->SetMinSize(wxSize(BFTaskDlg::lWidth1_, pLabel->GetSize().GetHeight()));
    pPanel->SetMinSize(wxSize(BFTaskDlg::lWidth2_, pPanel->GetSize().GetHeight()+3));

    // arrange/layout the main control
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
    if (pLabel)
        pSizer->Add(pLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSizer->Add(pPanel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Expand());
    SetSizer(pSizer);

    pPickerCtrl_->SetPath(strPath);
}

//
/*virtual*/ BFDestinationCtrl::~BFDestinationCtrl ()
{
}

wxStaticText* BFDestinationCtrl::CreateLabel(wxWindow* pParent)
{
    return new wxStaticText (pParent, -1, _("destination path:"));
}

wxString BFDestinationCtrl::GetPath ()
{
    return pPickerCtrl_->GetPath();
}

void BFDestinationCtrl::SetPath (const wxString& strPath)
{
    pPickerCtrl_->SetPath(strPath);
}
