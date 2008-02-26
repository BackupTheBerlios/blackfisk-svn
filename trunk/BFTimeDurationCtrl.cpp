/**
 * Name:        BFTimeDurationCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-26
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


#include "BFTimeDurationCtrl.h"

#include "Progress.h"

//
BFTimeDurationCtrl::BFTimeDurationCtrl (wxWindow* pParent,
                                        Progress* pProgressToObserve)
                  : Observer(pProgressToObserve),
                    wxGridBagSizer(0, 5),
                    pParent_(pParent),
                    timeBegin_(wxDateTime::Now())
{
    wxStaticText* pLabelElapsed = new wxStaticText(pParent, wxID_ANY, _("elapsed time: "));
    wxFont font = pLabelElapsed->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pLabelElapsed->SetFont(font);
    pElapsedTime_ = new wxStaticText(pParent, wxID_ANY, "00:00:00");

    wxStaticText* pLabelRemaining = new wxStaticText(pParent, wxID_ANY, _("remaining time: "));
    pLabelRemaining->SetFont(font);
    pRemainingTime_ = new wxStaticText(pParent, wxID_ANY, "00:00:00");

    Add(pLabelElapsed,    wxGBPosition(0, 0));
    Add(pElapsedTime_,    wxGBPosition(0, 1));
    Add(pLabelRemaining,  wxGBPosition(1, 0));
    Add(pRemainingTime_,  wxGBPosition(1, 1));
}


//
/*virtual*/ BFTimeDurationCtrl::~BFTimeDurationCtrl ()
{
}


/*virtual*/ void BFTimeDurationCtrl::ValueChanged (Subject* pSender)
{
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    //
    short sProgress = pP->GetProgress();
    if (sProgress == 0)
        return;

    pParent_->Freeze();

    // elapsed time
    wxTimeSpan spanElapsed = wxDateTime::Now().Subtract(timeBegin_);
    pElapsedTime_->SetLabel(spanElapsed.Format("%H:%M:%S"));

    // remaining time
    /*short sA = 100 - sProgress;
    wxLongLong llA = spanElapsed.GetSeconds();
    llA = llA / sProgress;*/
    wxTimeSpan spanRemaining( wxTimeSpan::Seconds(spanElapsed.GetSeconds() / sProgress * (100 - sProgress) ) );
    pRemainingTime_->SetLabel(spanRemaining.Format("%H:%M:%S"));

    pParent_->Thaw();
}
