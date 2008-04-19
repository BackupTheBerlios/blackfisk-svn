/**
 * Name:        BFIsWorkingCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-11
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#include "BFIsWorkingCtrl.h"

#define BFISWORKINGCTRL_HIGHLIGHT_0 "."
#define BFISWORKINGCTRL_HIGHLIGHT_1 "o"
#define BFISWORKINGCTRL_HIGHLIGHT_2 "o"
#define BFISWORKINGCTRL_HIGHLIGHT_3 "O"

//
BFIsWorkingCtrl::BFIsWorkingCtrl (wxWindow* pParent, long lLength)
               : wxPanel (pParent)
{
    pGBSizer_ = new wxGridBagSizer();

    CreatePositions (lLength);

    this->Connect
    (
        wxID_ANY,
        wxEVT_TIMER,
        wxTimerEventHandler (BFIsWorkingCtrl::OnTimer),
        NULL,
        this
    );

    pTimer_ = new wxTimer(this);
    pTimer_->Start(200);

    SetSizerAndFit(pGBSizer_);
}


//
/*virtual*/ BFIsWorkingCtrl::~BFIsWorkingCtrl ()
{
    pTimer_->Stop();
    delete pTimer_;
}

void BFIsWorkingCtrl::CreatePositions (long lLength)
{
    vecItems_.clear();
    std::vector<wxGBPosition> vecP;

    // we need a odd length
    if ( (lLength % 2) == 0 )
        ++lLength;

    // number of positions
    long lCount = lLength * 2 - 2;

    // top to right
    wxGBPosition posCurr;
    posCurr.SetCol((lCount/4));
    posCurr.SetRow(0);
    for (int i = 0; i < (lCount / 4); ++i)
    {
        vecP.push_back(wxGBPosition(posCurr.GetRow()+i,
                                    posCurr.GetCol()+i));

    }

    // right to bottom
    posCurr.SetCol(lLength-1);
    posCurr.SetRow((lCount/4));
    for (int i = 0; i < (lCount / 4); ++i)
    {
        vecP.push_back(wxGBPosition(posCurr.GetRow()+i,
                                    posCurr.GetCol()-i));
    }

    // bottom to left
    posCurr.SetCol((lCount/4));
    posCurr.SetRow(lLength-1);
    for (int i = 0; i < (lCount / 4); ++i)
    {
        vecP.push_back(wxGBPosition(posCurr.GetRow()-i,
                                    posCurr.GetCol()-i));
    }

    // left to top
    posCurr.SetCol(0);
    posCurr.SetRow((lCount/4));
    for (int i = 0; i < (lCount / 4); ++i)
    {
        vecP.push_back(wxGBPosition(posCurr.GetRow()-i,
                                    posCurr.GetCol()+i));
    }

    // layout them on sizer
    wxStaticText* pST = new wxStaticText(this, wxID_ANY, "W");
    wxFont font = pST->GetFont();
    font.SetPointSize(font.GetPointSize()-2);
    font.SetFamily(wxFONTFAMILY_MODERN);
    pST->SetFont(font);
    wxSize sizeMin = pST->GetSize();
    sizeMin.SetWidth(wxMax(sizeMin.GetWidth(), sizeMin.GetHeight()));
    sizeMin.SetHeight(sizeMin.GetWidth());
    pST->Destroy();
    pGBSizer_->SetEmptyCellSize(sizeMin);
    for (std::vector<wxGBPosition>::iterator it = vecP.begin();
         it != vecP.end();
         ++it)
    {
        pST = new wxStaticText(this,
                               wxID_ANY,
                               BFISWORKINGCTRL_HIGHLIGHT_0,
                               wxDefaultPosition,
                               sizeMin,
                               wxALIGN_CENTRE | wxST_NO_AUTORESIZE);
        pST->SetMinSize(sizeMin);
        pST->SetFont(font);
        //pST->SetBackgroundColour(*wxRED);
        vecItems_.push_back(pST);
        pGBSizer_->Add(pST, (*it), wxGBSpan(), wxALIGN_CENTER);
    }

    itCurrPos_ = vecItems_.begin();
}

void BFIsWorkingCtrl::HighlightItem (long lFromCurrentPos, const wxString& str)
{
    long lSteps = lFromCurrentPos;

    // step to related item relativ to current item position
    while (lSteps != 0)
    {
        if (lSteps > 0)
        {
            ++itCurrPos_;

            // check if it is the last item
            if (itCurrPos_ == vecItems_.end())
                itCurrPos_ = vecItems_.begin();

            --lSteps;
        }
        else
        {
            if (itCurrPos_ == vecItems_.begin())
                itCurrPos_ = vecItems_.end()-1;
            else
                --itCurrPos_;

            ++lSteps;
        }
    }

    // highlyt
    wxStaticText* pST = (*itCurrPos_);
    wxFont font = pST->GetFont();
    if (str == BFISWORKINGCTRL_HIGHLIGHT_0)
        font.SetWeight(wxFONTWEIGHT_NORMAL);
    else
        font.SetWeight(wxFONTWEIGHT_BOLD);
    pST->SetFont(font);
    pST->SetLabel(str);

    // step back to current pos
    while (lSteps != lFromCurrentPos)
    {
        if (lSteps > lFromCurrentPos)
        {
            ++itCurrPos_;

            // check if it is the last item
            if (itCurrPos_ == vecItems_.end())
                itCurrPos_ = vecItems_.begin();

            --lSteps;
        }
        else
        {
            if (itCurrPos_ == vecItems_.begin())
                itCurrPos_ = vecItems_.end()-1;
            else
                --itCurrPos_;

            ++lSteps;
        }
    }
}

void BFIsWorkingCtrl::OnTimer(wxTimerEvent& rEvent)
{
    Freeze();

    // normalize item
    HighlightItem(-2, BFISWORKINGCTRL_HIGHLIGHT_0);

    // step to next pos
    ++itCurrPos_;

    // check if it is the last item
    if (itCurrPos_ == vecItems_.end())
        itCurrPos_ = vecItems_.begin();

    // highlyt the itmes
    HighlightItem(-2, BFISWORKINGCTRL_HIGHLIGHT_1);
    HighlightItem(-1, BFISWORKINGCTRL_HIGHLIGHT_2);
    HighlightItem(0, BFISWORKINGCTRL_HIGHLIGHT_3);

    Thaw();
}
