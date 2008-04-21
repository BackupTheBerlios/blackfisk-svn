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

#include <wx/cshelp.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include "BFSystem.h" // XXX
/*static*/ wxHelpProvider* BFHelpCtrl::spHelpProvider_ = NULL;

/*static*/ void BFHelpCtrl::InitHelpProvider ()
{
    if (spHelpProvider_ == NULL)
    {
        // default help provider
        wxHelpProvider* pX = wxHelpProvider::Get();
        spHelpProvider_ = new wxSimpleHelpProvider;
        wxHelpProvider::Set(spHelpProvider_);
    }
}

BFHelpCtrl::BFHelpCtrl (wxWindow* pParent,
                        int iMaxHelpLines /*= 3*/)
          : wxPanel(pParent),
            pLastWindow_(NULL),
            bOnEnter_(false)
{
    // set help provider
    InitHelpProvider();

    // sub panel
    wxPanel* pSubPanel = new wxPanel(this);

    // text
    wxString strDefault;
    for (int i = 0; i < iMaxHelpLines; ++i)
        strDefault << _T('\n');
    pHelpText_ = new wxStaticText(pSubPanel,
                                  wxID_ANY,
                                  strDefault);

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


void BFHelpCtrl::Connect (wxWindow* pWindow)
{
    if (pWindow == NULL)
        return;

    pWindow->Connect
    (
        wxID_ANY,
        wxEVT_ENTER_WINDOW,
        wxMouseEventHandler (BFHelpCtrl::OnEnterWindow),
        NULL,
        this
    );

    pWindow->Connect
    (
        wxID_ANY,
        wxEVT_LEAVE_WINDOW,
        wxMouseEventHandler (BFHelpCtrl::OnLeaveWindow),
        NULL,
        this
    );
}

void BFHelpCtrl::Connect (wxWindow* pWindow,
                          const wxString& strHelpText)
{
    pWindow->SetHelpText(strHelpText);
    Connect(pWindow);
}

void BFHelpCtrl::Connect (wxWindow* pWindow1,
                          wxWindow* pWindow2,
                          const wxString& strHelpText)
{
    Connect(pWindow1, strHelpText);
    Connect(pWindow2, strHelpText);
}

void BFHelpCtrl::Connect (wxWindow* pWindow1,
                          wxWindow* pWindow2,
                          wxWindow* pWindow3,
                          const wxString& strHelpText)
{
    Connect(pWindow1, strHelpText);
    Connect(pWindow2, strHelpText);
    Connect(pWindow3, strHelpText);
}

void BFHelpCtrl::OnEnterWindow (wxMouseEvent& rEvent)
{
    bOnEnter_ = true;

    wxPoint point;
    wxWindow* pWindow = wxFindWindowAtPointer(point);

    if (pLastWindow_ == pWindow)
    {
        bOnEnter_ = false;
        return;
    }
    else
        pLastWindow_ = pWindow;

    if (pWindow != NULL)
    {
        pHelpText_->SetLabel(pWindow->GetHelpText());
        pHelpText_->Wrap(pHelpText_->GetParent()->GetSize().GetWidth()-5);
    }

    bOnEnter_ = false;
}

void BFHelpCtrl::OnLeaveWindow (wxMouseEvent& rEvent)
{
    if (bOnEnter_)
        return;

    pHelpText_->SetLabel("");
}
