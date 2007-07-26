/**
 * Name:        BFMessageDlg.cpp
 * Purpose:     BFMessageDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-23
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

#include "BFMessageDlg.h"
#include "BFSystem.h"
#include "BFMainFrame.h"

//
BFMessageDlg::BFMessageDlg (BFMainFrame* pParent)
            : wxMessageDialog(pParent, wxEmptyString),
              Observer(&(BFSystem::Instance()))
{
}


//
/*virtual*/ BFMessageDlg::~BFMessageDlg ()
{
}


void BFMessageDlg::SetMessage (const wxChar* msg)
{
    if (msg == NULL)
        m_message = wxEmptyString;
    else
        m_message = msg;
}

void BFMessageDlg::SetCaption (const wxChar* cap)
{
    if (cap == NULL)
        m_caption = wxEmptyString;
    else
        m_caption = cap;
}

void BFMessageDlg::SetStyle (long style)
{
    SetMessageDialogStyle(style);
}

/*virtual*/ void BFMessageDlg::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    // check arguments
    if (pSys == NULL)
        return;

    // check message level
    if (pSys->GetLastType() > pSys->GetMsgLevel())
        return;

    // check for message of type MsgBACKUP
    if (pSys->GetLastType() == MsgBACKUP) //&& pSys->GetObserverCount() > 0)
        return;

    // caption
    SetCaption (BFSystem::GetTypeString(pSys->GetLastType()));

    // icon
    SetStyle (wxOK | BFSystem::GetMsgStyle(pSys->GetLastType()));

    // message and location
    if (pSys->GetLastLocation().Len() == 0)
        SetMessage (pSys->GetLastMessage().c_str());
    else
        SetMessage ((pSys->GetLastMessage() + _T("\nLocation: ") + pSys->GetLastLocation()).c_str());

    /* if you need to check the return value,
       just use GetReturnCode() */
    ShowModal();
}

