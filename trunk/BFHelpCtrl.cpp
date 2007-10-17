/**
 * Name:        BFHelpCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-12
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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


#include "BFHelpCtrl.h"


BFHelpCtrl::BFHelpCtrl (wxWindow* pParent, int iMaxHelpLines /*= 3*/)
          : wxPanel(pParent),
            pLastWindow_(NULL)
{
    // sub panel
    wxPanel* pSubPanel = new wxPanel(this);

    // text
    wxString strDefault;
    for (int i = 0; i < iMaxHelpLines; ++i)
        strDefault << _T('\n');
    pHelpText_ = new wxStaticText(pSubPanel, wxID_ANY, strDefault, wxDefaultPosition, wxDefaultSize, wxST_MARKUP);


    // colour
    wxColour colour = pSubPanel->GetBackgroundColour();
    colour.Set(colour.Red()+35, colour.Green()+35, colour.Blue()+35);
    pSubPanel->SetBackgroundColour(colour);

    // sizer and arrange
    wxStaticBoxSizer* pSizer = new wxStaticBoxSizer(wxVERTICAL, this);
    wxBoxSizer* pSubSizer = new wxBoxSizer(wxVERTICAL);
    pSubSizer->Add(pHelpText_, wxSizerFlags(0).Expand().Border().Center());
    pSubPanel->SetSizer(pSubSizer);
    pSizer->Add(pSubPanel, wxSizerFlags(0).Expand());
    SetSizer(pSizer);
}

/*virtual*/ BFHelpCtrl::~BFHelpCtrl ()
{
}

void BFHelpCtrl::ConnectMotionEvent (wxWindow* pWindow)
{
    if (pWindow == NULL)
        return;

    pWindow->Connect
    (
        wxID_ANY,
        wxEVT_MOTION,
        wxMouseEventHandler (BFHelpCtrl::OnMotion),
        NULL,
        this
    );
}

void BFHelpCtrl::OnMotion (wxMouseEvent& rEvent)
{
    wxPoint point;
    wxWindow* pWindow = wxFindWindowAtPointer(point);

    if (pLastWindow_ == pWindow)
        return;
    else
        pLastWindow_ = pWindow;

    if (pWindow != NULL)
    {
        pHelpText_->SetLabel(pWindow->GetHelpText());
        pHelpText_->Wrap(pHelpText_->GetParent()->GetSize().GetWidth()-5);
    }
}
