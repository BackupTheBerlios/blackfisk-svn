/**
 * Name:        BFBackupQuestionDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-29
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


#include "BFBackupQuestionDlg.h"

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/artprov.h>

#include "BFProjectSettings.h"
#include "BFMainFrame.h"

BEGIN_EVENT_TABLE(BFBackupQuestionDlg, wxDialog)
    EVT_BUTTON(BF_BTNID_IGNORE,       BFBackupQuestionDlg::OnButton)
    EVT_BUTTON(BF_BTNID_STOPPRJ,      BFBackupQuestionDlg::OnButton)
    EVT_BUTTON(BF_BTNID_STOPTASK,     BFBackupQuestionDlg::OnButton)
END_EVENT_TABLE()

//
BFBackupQuestionDlg::BFBackupQuestionDlg (const wxString& strMsg, BFMessageType type)
                   : wxDialog(BFMainFrame::Instance(),
                              wxID_ANY,
                              BFSystem::GetTypeString(type),
                              wxDefaultPosition,
                              wxDefaultSize,
                              wxCAPTION)
{
    // sizer
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pIconText = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pButtons = new wxBoxSizer(wxHORIZONTAL);

    // icon
    wxArtID artId;

    switch (type)
    {
        case MsgWARNING:
            artId = wxART_WARNING;
            break;

        case MsgERROR:
        case MsgFATAL:
        default:
            artId = wxART_ERROR;
            break;
    }

    pIconText->Add (new wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetIcon(artId, wxART_MESSAGE_BOX)), 0, wxCENTER);

    // check msg width
    wxString strMyMsg = strMsg;
    wxClientDC dc(this);
    dc.SetFont(GetFont());
    wxSize sizeMsg = dc.GetTextExtent(strMyMsg);
    int iWidthToUse = ::wxGetDisplaySize().GetWidth() / 10 * 9;

    if (sizeMsg.GetWidth() > iWidthToUse)
    {
        int idx = 0;
        while (idx < strMyMsg.Length() / 2)
        {
            idx = strMyMsg.Mid(idx).Find(' ');

            if (idx == wxNOT_FOUND)
                idx = strMyMsg.Length() / 2;
        }

        strMyMsg.insert(idx, '\n');
    }

    // text
    pIconText->Add (CreateTextSizer(strMyMsg + _("\n\nHow is to be continued?")), 0, wxALIGN_CENTER | wxLEFT, 10);

    // buttons
    pButtons->Add (new wxButton(this, BF_BTNID_STOPTASK, _("stop task")),   wxSizerFlags(0).Expand().Border());
    pButtons->Add (new wxButton(this, BF_BTNID_STOPPRJ, _("stop project")), wxSizerFlags(0).Expand().Border());
    pButtons->Add (new wxButton(this, BF_BTNID_IGNORE, _("ignore")),        wxSizerFlags(0).Expand().Border());

    // layout
    pTopSizer->Add (pIconText,   1, wxCENTER | wxLEFT | wxRIGHT | wxTOP, 10 );
    pTopSizer->Add (pButtons,    0, wxCENTER | wxALL,                    10);
    SetSizerAndFit (pTopSizer);
    Center();
}

void BFBackupQuestionDlg::OnButton (wxCommandEvent& rEvent)
{
    EndModal(rEvent.GetId());
}

//
/*virtual*/ BFBackupQuestionDlg::~BFBackupQuestionDlg ()
{
}


/*static*/ BF_StopLevel BFBackupQuestionDlg::Ask (const wxString& strMsg, BFMessageType type)
{

    BFBackupQuestionDlg* pDlg = new BFBackupQuestionDlg(strMsg, type);

    BF_StopLevel stop;

    switch(pDlg->ShowModal())
    {
        case BF_BTNID_STOPTASK:
            stop = BFDO_STOPTSK;
            break;

        case BF_BTNID_IGNORE:
            stop = BFDO_IGNORE;
            break;

        default:
        case BF_BTNID_STOPPRJ:
            stop = BFDO_STOPPRJ;
            break;
    }

    pDlg->Destroy();

    return stop;
}
