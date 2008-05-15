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
                            const wxString& strTitle /*= wxEmptyString*/)
            : wxDialog(NULL, wxID_ANY, strTitle)
{
    // sizer
    wxBoxSizer* pSizerV = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pSizerHa = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pSizerHb = new wxBoxSizer(wxHORIZONTAL);

    // title
    SetTitle(GetMessageTypeString(type));

    // icon
    wxStaticBitmap* pSBitmap = new wxStaticBitmap(this, wxID_ANY, GetMessageTypeBitmap(type));

    // check message width
    wxString strMyMsg = strMsg;
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
    /*static wxBitmap bmpQuestion = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "question64.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpInfo     = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "info64.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpWarning  = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "warning64.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpError    = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "error64.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpFatal    = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "fatal64.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpDefault  = wxBitmap();*/

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
    /*static wxBitmap bmpOk       = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "yes16.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpCancel   = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "no16.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpYes      = bmpOk;
    static wxBitmap bmpNo       = bmpCancel;
    static wxBitmap bmpStopPrj  = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "stop_prj16.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpStopTsk  = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "stop_task16.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpIgnore   = wxBitmap(wxString::Format("%s%s", BF_GRAPHICDIR, "ignore16.png"), wxBITMAP_TYPE_PNG);
    static wxBitmap bmpDefault  = wxBitmap();*/

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

/*
wxBitmap BFMessageDlg::CreateButtonBitmap (wxWindowID id)
{
    wxString strFilename;
    wxString strLabel;

    switch (id)
    {
        case BF_MSGDLG_ID_OK:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "yes16.png");
            strLabel = _("OK");
            break;

        case BF_MSGDLG_ID_YES:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "yes16.png");
            strLabel = _(" Yes");
            break;

        case BF_MSGDLG_ID_CANCEL:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "no16.png");
            strLabel = _(" Cancel");
            break;

        case BF_MSGDLG_ID_NO:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "no16.png");
            strLabel = _(" No");
            break;

        case BF_MSGDLG_ID_STOPPRJ:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "stop_prj16.png");
            strLabel = _(" Stop Project");
            break;

        case BF_MSGDLG_ID_STOPTSK:
            strFilename = wxString::Format("%s%s", BF_GRAPHICDIR, "stop_task16.png");
            strLabel = _(" Stop Task");
            break;

        default:
            strLabel = _(" (unknown ID)");
            break;
    }

    // string size
    wxWindowDC* pWindowDC = new wxWindowDC(this);
    wxSize sizeText = pWindowDC->GetTextExtent(wxString::Format("%s", strLabel));

    // load bitmap
    wxBitmap bmp2(strFilename, wxBITMAP_TYPE_PNG);
    wxBitmap bmp(bmp2.GetWidth() + sizeText.GetWidth(), bmp2.GetHeight() + sizeText.GetHeight());

    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), wxBRUSHSTYLE_SOLID));
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.Clear();
    dc.DrawLabel(strLabel,
                 bmp2,
                 wxRect(0,
                        0,
                        bmp.GetWidth(),
                        bmp.GetHeight()),
                 wxALIGN_CENTRE);

    return bmp;
}*/


void BFMessageDlg::OnButton (wxCommandEvent& rEvent)
{
    EndModal(rEvent.GetId());
}
