/**
 * Name:        BFMessageDlg.cpp
 * Purpose:     BFMessageDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-02-23
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

#include "BFMessageDlg.h"

#include <wx/sizer.h>

#include "BFSystem.h"
#include "BFMainFrame.h"
#include "BFProjectSettings.h"
#include "BFRootTask.h"

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

    /* don't show MsgLOG
       MsgLOG is logged by class BFLog in the application log file */
    if (pSys->GetLastType() == MsgLOG)
        return;

    // behave ;)
    if (BFCore::Instance().IsWhileBackup())
        BehaviorWhileBackup();
    else
        BehaviorDefault();
}

void BFMessageDlg::BehaviorDefault ()
{
    BFSystem& rS = BFSystem::Instance();

    // caption
    SetCaption (BFSystem::GetTypeString(rS.GetLastType()));

    // icon
    SetStyle (wxOK | BFSystem::GetMsgStyle(rS.GetLastType()));

    // message and location
    if (rS.GetLastLocation().Len() == 0)
        SetMessage (rS.GetLastMessage().c_str());
    else
        SetMessage ((rS.GetLastMessage() + _T("\nLocation: ") + rS.GetLastLocation()).c_str());

    /* if you need to check the return value,
       just use GetReturnCode() */
    ShowModal();
}

void BFMessageDlg::BehaviorWhileBackup ()
{
    BFSystem& rS = BFSystem::Instance();
    BFMessageType type = rS.GetLastType();

/*  MsgWARNING,
    MsgERROR,
    MsgFATAL

    BFDO_STOPPRJ,
    BFDO_STOPTSK,
    BFDO_ASK,
    BFDO_IGNORE */

    // stop level
    BF_StopLevel stop = BFDO_ASK;

    // get the stop level
    switch (type)
    {
        case MsgWARNING:
            stop = BFRootTask::Instance().GetSettings().GetStopLevelOnWarning();
            break;

        case MsgERROR:
            stop = BFRootTask::Instance().GetSettings().GetStopLevelOnError();
            break;

        case MsgFATAL:
            stop = BFRootTask::Instance().GetSettings().GetStopLevelOnFatal();
            break;

        // don't handle other message types while a backup
        default:
            return;
            break;
    };

    // IGNORE
    if (stop == BFDO_IGNORE)
        return;

    // STOP PROJECT
    if (stop == BFDO_STOPPRJ)
    {
        BFRootTask::Instance().StopProject();
        return;
    }

    // STOP TASK
    if (stop == BFDO_STOPTSK)
    {
        BFRootTask::Instance().StopCurrentTask();
        return;
    }

    // ASK
    SetCaption (BFSystem::GetTypeString(rS.GetLastType()));
    SetStyle (wxOK | wxRESET | BFSystem::GetMsgStyle(rS.GetLastType()));

    //wxSizer* sizerBtn = dynamic_cast<wxSizer*>(GetSizer()->GetItem(2)->GetUserData());
    //wxSizer* pSizer = GetSizer();


    // message and location
    if (rS.GetLastLocation().Len() == 0)
        SetMessage (rS.GetLastMessage().c_str());
    else
        SetMessage ((rS.GetLastMessage() + _T("\nLocation: ") + rS.GetLastLocation()).c_str());

    ShowModal();
}

