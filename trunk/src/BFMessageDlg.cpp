/**
 * Name:        BFMessageDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-05-14
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


#include "BFMessageDlg.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/bmpbuttn.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>

#include "blackfisk.h"
#include "BFBitmapButton.h"
#include "BFMainFrame.h"
#include "BFIconTable.h"

BEGIN_EVENT_TABLE(BFMessageDlg, wxDialog)
    EVT_BUTTON(BF_MSGDLG_ID_OK,         BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_YES,        BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_CANCEL,     BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_NO,         BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_STOPPRJ,    BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_STOPTSK,    BFMessageDlg::OnButton)
    EVT_BUTTON(BF_MSGDLG_ID_IGNORE,     BFMessageDlg::OnButton)
END_EVENT_TABLE()

//
BFMessageDlg::BFMessageDlg (BFMessageDlg_Type type,
                            const wxString& strMsg,
                            const wxString& strTitle /*= wxEmptyString*/,
                            BFMessageDlg_Type type_icon /*= BF_MSGDLG_NOTUSED*/)
            : wxDialog(NULL, wxID_ANY, strTitle)
{
    // sizer
    wxBoxSizer* pSizerV = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pSizerHa = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSizerHb = new wxBoxSizer(wxHORIZONTAL);

    // title
    SetTitle(GetMessageTypeString(type));

    // icon
    if (type_icon == BF_MSGDLG_NOTUSED)
        type_icon = type;

    wxStaticBitmap* pSBitmap = new wxStaticBitmap(this, wxID_ANY, GetMessageTypeBitmap(type_icon));

    // check message width
    wxString strMyMsg = strMsg;
    if (BFMainFrame::Instance() != NULL)
        BFMainFrame::Instance()->Wrap(strMyMsg, wxGetDisplaySize().GetWidth() / 10 * 5);

    // message
    wxStaticText* pSText = new wxStaticText(this,
                                            wxID_ANY,
                                            strMyMsg,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxALIGN_LEFT);

    //
    pSizerHa->Add(pSBitmap, wxSizerFlags(0).Border());
    pSizerHa->Add(pSText, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));

    // buttons
    switch (type)
    {
        case BF_MSGDLG_QUESTION_YESNO:
            CreateButtons_Question (pSizerHb, false);
            break;

        case BF_MSGDLG_QUESTION_YESNOCANCEL:
            CreateButtons_Question (pSizerHb, true);
            break;

        case BF_MSGDLG_BACKUP_QUESTION:
            CreateButtons_BackupQuestion (pSizerHb);
            break;

        default:
            CreateButtons_Default (pSizerHb);
            break;
    }

    // arrange
    pSizerV->Add(pSizerHa, wxSizerFlags(0).Center().Border());
    pSizerV->Add(pSizerHb, wxSizerFlags(0).Center().Border());
    SetSizerAndFit(pSizerV);
    Center();
    ShowModal();
}

//
/*virtual*/ BFMessageDlg::~BFMessageDlg ()
{
}

void BFMessageDlg::CreateButtons_Default (wxSizer* pSizer)
{
    if (pSizer == NULL)
        return;

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_OK, GetButtonBitmap(BF_MSGDLG_ID_OK), "OK") );
}

void BFMessageDlg::CreateButtons_Question (wxSizer* pSizer, bool bWithCancel)
{
    if (pSizer == NULL)
        return;

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_YES, GetButtonBitmap(BF_MSGDLG_ID_YES), _("Yes")) );

    pSizer->AddSpacer(10);

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_NO, GetButtonBitmap(BF_MSGDLG_ID_NO), _("No")) );

    pSizer->AddSpacer(10);

    if (bWithCancel)
        pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_CANCEL, GetButtonBitmap(BF_MSGDLG_ID_CANCEL), _("Cancel")) );
}

void BFMessageDlg::CreateButtons_BackupQuestion (wxSizer* pSizer)
{
    if (pSizer == NULL)
        return;

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_STOPTSK, GetButtonBitmap(BF_MSGDLG_ID_STOPTSK), _("Stop Task")) );

    pSizer->AddSpacer(10);

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_STOPPRJ, GetButtonBitmap(BF_MSGDLG_ID_STOPPRJ), _("Stop Project")) );

    pSizer->AddSpacer(10);

    pSizer->Add( new BFBitmapButton(this, BF_MSGDLG_ID_IGNORE, GetButtonBitmap(BF_MSGDLG_ID_IGNORE), _("Ignore")) );
}

/*static*/ wxString BFMessageDlg::GetMessageTypeString (BFMessageDlg_Type type)
{
    switch (type)
    {
        case BF_MSGDLG_QUESTION_YESNO:
        case BF_MSGDLG_QUESTION_YESNOCANCEL:
        case BF_MSGDLG_BACKUP_QUESTION:
            return _("Question");

        case BF_MSGDLG_INFO:
            return _("Information");

        case BF_MSGDLG_WARNING:
            return _("Warning");

        case BF_MSGDLG_ERROR:
            return _("Error");

        case BF_MSGDLG_FATAL:
            return _("Fatal Error");

        default:
            return _("unknown message dialog type");
    }
}

/*static*/ const wxBitmap BFMessageDlg::GetMessageTypeBitmap (BFMessageDlg_Type type)
{
    switch (type)
    {
        case BF_MSGDLG_QUESTION_YESNO:
        case BF_MSGDLG_QUESTION_YESNOCANCEL:
        case BF_MSGDLG_BACKUP_QUESTION:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::question);

        case BF_MSGDLG_INFO:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::info);

        case BF_MSGDLG_WARNING:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::warning);

        case BF_MSGDLG_ERROR:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::error);

        case BF_MSGDLG_FATAL:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::fatal);

        default:
            return wxBitmap();
    }
}

const wxBitmap BFMessageDlg::GetButtonBitmap (wxWindowID id)
{
    switch (id)
    {
        case BF_MSGDLG_ID_OK:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::ok);

        case BF_MSGDLG_ID_YES:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::yes);

        case BF_MSGDLG_ID_CANCEL:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::cancel);

        case BF_MSGDLG_ID_NO:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::no);

        case BF_MSGDLG_ID_STOPPRJ:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::stop_prj);

        case BF_MSGDLG_ID_STOPTSK:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::stop_task);

        case BF_MSGDLG_ID_IGNORE:
            return BFIconTable::Instance()->GetBitmap(BFIconTable::ignore);

        default:
            return wxBitmap();
    }
}

void BFMessageDlg::OnButton (wxCommandEvent& rEvent)
{
    EndModal(rEvent.GetId());
}

/*static*/ BFMessageDlg_Type BFMessageDlg::GetDlgType (BFMessageType type)
{
    switch(type)
    {
        case MsgINFO:
        case MsgBACKUP:
        case MsgLOG:
        case MsgDEBUG:
            return BF_MSGDLG_INFO;

        case MsgWARNING:
            return BF_MSGDLG_WARNING;

        case MsgERROR:
            return BF_MSGDLG_ERROR;

        default:
        case MsgFATAL:
        case MsgUNKNOWN:
            return BF_MSGDLG_FATAL;
    };
}