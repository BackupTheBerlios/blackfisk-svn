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
#include <wx/dcclient.h>

#include "Progress.h"

#ifdef _DEBUG
#include "BFSystem.h"
#endif

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

    Freeze();
    UpdateBar   (pP);
    UpdateText  (pP);
    Thaw();
}


/*virtual*/ void BFProgressCtrlBase::UpdateBar (Progress* pP)
{
    if (pP == NULL || pBar_ == NULL)
        return;

    if (pBar_->GetValue() != pP->GetProgress())
        pBar_->SetValue ( pP->GetProgress() );
}


/*virtual*/ void BFProgressCtrlBase::UpdateText (Progress* pP)
{
    if (pP == NULL || pTextB_ == NULL)
        return;

    wxString strLabel = wxString::Format(_T("%d %%"), pP->GetProgress());

    if (strLabel != pTextB_->GetLabel())
    {
        pTextB_->SetLabel ( strLabel );
        Layout();
    }
}


void BFProgressCtrlBase::SetTextA (const wxChar* text)
{
    if (text == NULL || pTextA_ == NULL)
        return;

    pTextA_->SetLabel(text);
    Layout();
}

/*static*/ wxString& BFProgressCtrlBase::ShortenPath(wxString& strPath, int len)
{
    // check parameters
    if ( len > strPath.Len() || !(strPath.Contains(wxFILE_SEP_PATH)) )
        return strPath;

    // number of characters to cut incl "..."
    int iCut        = strPath.Len() - len + 3;
    int iCutLeft    = iCut / 2;
    int iCutRight   = iCut - iCutLeft;

    wxString strLeft    = strPath.Left(strPath.Len()/2);
    wxString strRight   = strPath.Right(strPath.Len()-strLeft.Len());

    strLeft.Replace(strLeft.Right(iCutLeft), _T(".."));
    strRight.Replace(strRight.Left(iCutRight), _T("."));

    strPath = strLeft + strRight;

    return strPath;
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
    pTextB_ = new wxStaticText(this, -1, "000 %", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
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
    pBar_   = new wxGauge(this,
                          -1,
                          100,
                          wxDefaultPosition,
                          wxSize(450, 15),
                          wxGA_HORIZONTAL | wxGA_SMOOTH);
    pTextB_ = new wxStaticText(this,
                               -1,
                               "   \n   ",
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxST_NO_AUTORESIZE | wxST_MARKUP);

    // arange
    pSubSizer->Add(pTextA_, wxSizerFlags(0).Center() );
    pSubSizer->Add(pTextC_, wxSizerFlags(0).Center().Border(wxLEFT, 5) );
    pSizer->Add(pSubSizer,  wxSizerFlags(0).Center() );
    pSizer->Add(pBar_,      wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 3));
    pSizer->Add(pTextB_,    wxSizerFlags(0).Expand());
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

    // check variables
    if (pPM == NULL || pTextA_ == NULL || pTextB_ == NULL || pTextC_ == NULL)
        return;

    // set the progress in %
    pTextC_->SetLabel ( wxString::Format(_T("%d %%"), pPM->GetProgress()) );

    // check the width of the message
    wxString strMsg = pPM->GetMessage();
    wxClientDC dc(this);
    dc.SetFont(pTextB_->GetFont());
    int widthCtrl = pTextB_->GetClientSize().GetWidth();
    int widthText = dc.GetTextExtent(strMsg).GetWidth();

    if ( widthText > widthCtrl )
    {
        // shorten the path
        ShortenPath(strMsg, (int)((double)strMsg.Len() / (double)widthText * (double)widthCtrl));

        // get the new text width in pixel
        widthText = dc.GetTextExtent(strMsg).GetWidth();
    }

    while ( widthText > widthCtrl )
    {
        // cut the path on one character
        ShortenPath(strMsg, strMsg.Len()-1);
        // get the new text width in pixel
        widthText = dc.GetTextExtent(strMsg).GetWidth();
    }

    pTextB_->SetLabel ( wxString::Format(_T("%s\n%s"), pPM->GetLabel(), strMsg) );

    // relayout
    //Layout();
}

