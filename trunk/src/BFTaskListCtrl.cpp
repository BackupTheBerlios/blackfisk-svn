/**
 * Name:        BFTaskListCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-08
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


#include "BFTaskListCtrl.h"

#include <wx/gdicmn.h>
#include <wx/settings.h>

#include "BFRootTaskApp.h"
#include "BFTask.h"
#include "BFSystem.h"
#include "BFIsWorkingCtrl.h"

//
BFTaskListCtrl::BFTaskListCtrl (wxWindow* pParent)
              : wxStaticBoxSizer(wxVERTICAL, pParent),
                Observer(&(BFSystem::Instance())),
                pParent_(pParent),
                oidActive_(BFInvalidOID),
                bActiveTaskSucceeded_(true)
{
    BFRootTaskApp::Instance().InitThat(this);

    this->AddStretchSpacer(1);
    this->Add(new BFIsWorkingCtrl(pParent, 5), wxSizerFlags(0).Center().Border());
}


//
/*virtual*/ BFTaskListCtrl::~BFTaskListCtrl ()
{
}

/*virtual*/ void BFTaskListCtrl::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    BFMessageType tMsg = pSys->GetLastType();

    // error message?
    if ( tMsg == MsgWARNING
      || tMsg == MsgERROR
      || tMsg == MsgFATAL
      || BFRootTaskApp::Instance().GetStopCurrentTask())
    {
        bActiveTaskSucceeded_ = false;
    }
}

wxStaticText* BFTaskListCtrl::FindItem (BFoid oid)
{
    for (ItemVector::iterator it = vecItems_.begin();
         it != vecItems_.end();
         ++it)
    {
        if ((*it).first == oid)
            return (*it).second;
    }

    return NULL;
}


void BFTaskListCtrl::Append (const BFTask* pTask)
{
    // check parameters
    if (pTask == NULL)
        return;

    // create ctrl
    wxStaticText* pST = new wxStaticText(pParent_,
                                         wxID_ANY,
                                         pTask->GetName());//, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    // arrange
    this->Add(pST, wxSizerFlags(0).Center().Border());

    // remember oid and ctrl
    vecItems_.push_back(Item(pTask->GetOID(), pST));
}

bool BFTaskListCtrl::SetTask_Active (BFoid oid)
{
    // is there an active item before?
    if (oidActive_ != BFInvalidOID)
    {
        if (bActiveTaskSucceeded_)
            SetTask_Succeeded(oidActive_);
        else
            SetTask_Errror(oidActive_);
    }

    bActiveTaskSucceeded_ = true;

    // the new active item
    wxStaticText* pST = FindItem(oid);
    oidActive_ = oid;

    if (pST == NULL)
        return false;

    wxFont font = pST->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pST->SetFont(font);
    //pST->SetForegroundColour(*wxBLACK);
    pST->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    pST->Refresh();
    pParent_->Layout();

    return true;
}

bool BFTaskListCtrl::SetTask_Succeeded (BFoid oid)
{
    wxStaticText* pST = FindItem(oid);

    if (pST == NULL)
        return false;

    wxFont font = pST->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pST->SetFont(font);
    pST->SetForegroundColour(wxTheColourDatabase->Find("FOREST GREEN"));
    pST->Refresh();

    return true;
}

bool BFTaskListCtrl::SetTask_Warning (BFoid oid)
{
    return SetTask_Errror(oid);
}

bool BFTaskListCtrl::SetTask_Errror (BFoid oid)
{
    wxStaticText* pST = FindItem(oid);

    if (pST == NULL)
        return false;

    wxFont font = pST->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pST->SetFont(font);
    // RED
    pST->SetForegroundColour(wxTheColourDatabase->Find("RED"));
    pST->Refresh();

    return true;
}
