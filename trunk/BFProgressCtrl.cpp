/**
 * Name:        BFProgressCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-09
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFProgressCtrl.h"

#include <wx/sizer.h>

#include "Progress.h"


// ++++++++++++++++++++++++++++++
// ++ class BFProgressCtrlBase ++
// ++++++++++++++++++++++++++++++

/*protected*/ BFProgressCtrlBase::BFProgressCtrlBase (wxWindow* pParent,
                                                      Progress* pProgress)
                                   : wxPanel(pParent, -1),
                                     Observer(pProgress),
                                     pBar_(NULL),
                                     pTextA_(NULL),
                                     pTextB_(NULL),
                                     pTextC_(NULL)
{
}


/*virtual*/ BFProgressCtrlBase::~BFProgressCtrlBase ()
{
}


/*virtual*/ void BFProgressCtrlBase::ValueChanged (Subject* pSender)
{
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    UpdateBar   (pP);
    UpdateText  (pP);

/*    wxWindow* pC = this;
    while (pC->GetParent() != NULL)
        pC = pC->GetParent();

    //BFMainFrame::App()->Yield();
    pC->Update();*/
}


/*virtual*/ void BFProgressCtrlBase::UpdateBar (Progress* pP)
{
    if (pP == NULL || pBar_ == NULL)
        return;

    pBar_->SetValue ( pP->GetProgress() );
}


/*virtual*/ void BFProgressCtrlBase::UpdateText (Progress* pP)
{
    if (pP == NULL || pTextB_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%d %%"), pP->GetProgress()) );
    Layout();
}


void BFProgressCtrlBase::SetTextA (const wxChar* text)
{
    if (text == NULL || pTextA_ == NULL)
        return;

    pTextA_->SetLabel(text);
    Layout();
}


// +++++++++++++++++++++++++++++++
// ++ class BFProgressTotalCtrl ++
// +++++++++++++++++++++++++++++++


BFProgressTotalCtrl::BFProgressTotalCtrl (wxWindow* pParent, Progress* pProgress)
                   : BFProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}

/*virtual*/ BFProgressTotalCtrl::~BFProgressTotalCtrl ()
{
}

/*virtual*/ void BFProgressTotalCtrl::InitControls ()
{
    if (pTextA_ != NULL || pTextB_ != NULL || pBar_ != NULL)
        return;

    // sizer
    wxStaticBoxSizer*   pTopSizer       = new wxStaticBoxSizer(wxVERTICAL, this);

    // controls
    pTextA_ = new wxStaticText(this, -1, _("Total"));
    wxFont font = pTextA_->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pTextA_->SetFont(font);
    pTextB_ = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    pTextB_->SetFont(font);
    pBar_   = new wxGauge(this,
                          -1,
                          100,
                          wxDefaultPosition,
                          wxDefaultSize/*wxSize(300, 30)*/,
                          wxGA_VERTICAL | wxGA_SMOOTH);

    // arange
    pTopSizer   ->Add ( pTextA_,    wxSizerFlags(0).Center() );
    pTopSizer   ->Add ( pTextB_,    wxSizerFlags(0).Center() );
    pTopSizer   ->Add ( pBar_,      wxSizerFlags(1).Expand() );
    SetSizer( pTopSizer );
}


// ++++++++++++++++++++++++++++++
// ++ class BFProgressTaskCtrl ++
// ++++++++++++++++++++++++++++++

/*virtual*/ void BFProgressTaskCtrl::InitControls ()
{
    if (pTextA_ != NULL || pTextB_ != NULL || pTextC_ != NULL || pBar_ != NULL)
        return;

    // sizer
    wxStaticBoxSizer*   pSizer = new wxStaticBoxSizer(wxVERTICAL, this);
    wxBoxSizer*         pSubSizer = new wxBoxSizer(wxHORIZONTAL);

    // controls
    pTextA_ = new wxStaticText(this, -1, wxEmptyString);//, _T("<label>"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    wxFont font = pTextA_->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pTextA_->SetFont(font);
    pTextC_ = new wxStaticText(this, -1, wxEmptyString);
    pTextC_->SetFont(font);
    pBar_   = new wxGauge(this, -1, 100, wxDefaultPosition, wxSize(450, 15), wxGA_HORIZONTAL | wxGA_SMOOTH);
    pTextB_ = new wxStaticText(this, -1, _T("<message>\n<message2>"));


    // arange
    pSubSizer->Add(pTextA_, wxSizerFlags(0).Center() );
    pSubSizer->Add(pTextC_, wxSizerFlags(0).Center().Border(wxLEFT, 5) );
    pSizer->Add(pSubSizer,  wxSizerFlags(0).Center() );
    pSizer->Add(pBar_,      wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 3));
    pSizer->Add(pTextB_);
    SetSizerAndFit(pSizer);
}


BFProgressTaskCtrl::BFProgressTaskCtrl (wxWindow* pParent, Progress* pProgress)
                    : BFProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}


/*virtual*/ BFProgressTaskCtrl::~BFProgressTaskCtrl ()
{
}


/*virtual*/ void BFProgressTaskCtrl::UpdateText (Progress* pP)
{
    ProgressWithMessage* pPM = dynamic_cast<ProgressWithMessage*>(pP);

    if (pPM == NULL || pTextA_ == NULL || pTextB_ == NULL || pTextC_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%s\n%s"), pPM->GetLabel(), pPM->GetMessage()) );
    pTextC_->SetLabel ( wxString::Format(_T("%d %%"), pPM->GetProgress()) );
    Layout();
}
