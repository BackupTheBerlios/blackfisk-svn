﻿/**
 * Name:        BFAboutDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-08-19
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


#include "BFAboutDlg.h"

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/statbmp.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFHyperlinkCtrl.h"
#include "BFApp.h"
#include "jbSerialize.h"
#include "BFEnvironment.h"

#define BFABOUTDLG_ID_VIEWLICENSE   1 + wxID_HIGHEST
#define BFABOUTDLG_ID_OK            2 + wxID_HIGHEST

BEGIN_EVENT_TABLE(BFAboutDlg, wxDialog)
  EVT_CLOSE     (                           BFAboutDlg::OnClose)
  EVT_BUTTON    (BFABOUTDLG_ID_VIEWLICENSE, BFAboutDlg::OnViewLicense)
  EVT_BUTTON    (BFABOUTDLG_ID_OK,          BFAboutDlg::OnOk)
END_EVENT_TABLE()


BFAboutDlg::BFAboutDlg ()
          : wxDialog(BFMainFrame::Instance(),
                     wxID_ANY,
                     wxString(_("About ")) + BF_PRGNAME,
                     wxDefaultPosition,
                     wxDefaultSize,
                     wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
{
    wxFont font;

    // sizers
    wxBoxSizer* pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* pMiddleSizer   = new wxGridSizer(2);
    wxBoxSizer* pMLeftSizer     = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pMRightSizer    = new wxBoxSizer(wxVERTICAL);

    wxStaticBitmap* pLogo = new wxStaticBitmap(this,
                                               wxID_ANY,
                                               wxBitmap(BFEnvironment::GetGraphicDir() + "logo_text.png", wxBITMAP_TYPE_PNG));

    wxStaticText* pName = new wxStaticText(this, wxID_ANY, wxString::Format("Version: %s", BFApp::GetVersion()));
    font = pName->GetFont();
    font.SetPointSize(font.GetPointSize()+5);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pName->SetFont(font);
    wxStaticText* pCopy = new wxStaticText(this, wxID_ANY, "Copyright (C) 2005 Christian Buhtz <blackfisk@web.de>");
    wxStaticText* pDesc = new wxStaticText(this, wxID_ANY, _("A backup software that can handle very complex tasks.\nBut it has the goal that it is easy to use\nwithout reading a manual."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    BFHyperlinkCtrl* pWeb = new BFHyperlinkCtrl(this, wxID_ANY, "www.blackfisk.org", "http://www.blackfisk.org");
    wxStaticText* pLicense = new wxStaticText(this, wxID_ANY, _("This program comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome\nto redistribute it under certain conditions;\nfor details select the menu Help/License\nor click this button"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxButton* pViewLicense = new wxButton(this, BFABOUTDLG_ID_VIEWLICENSE, _("view License (GPLv3)"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    font = pLicense->GetFont();
    font.SetFaceName("Courier New");
    font.SetPointSize(8);
    pLicense->SetFont(font);

    wxStaticText* pDev = new wxStaticText(this, wxID_ANY, _("Developers"));
    // Artists
    wxStaticText* pThird = new wxStaticText(this, wxID_ANY, _("3rd-Party-Components"));
    wxStaticText* pTools = new wxStaticText(this, wxID_ANY, _("used Tools"));
    wxStaticText* pSupporters = new wxStaticText(this, wxID_ANY, _("Supporters"));
    font = pDev->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    font.SetPointSize(font.GetPointSize()+1);
    pDev->SetFont(font);
    pThird->SetFont(font);
    pTools->SetFont(font);
    pSupporters->SetFont(font);
    wxStaticText* pDev1 = new wxStaticText(this, wxID_ANY, "Christian Buhtz");

    wxSizer* pThird1 = CreateThirdPartySizer("http://www.xs4all.nl/~jorgb/wb/index.html",
                                             "wxSerialize",
                                             wxString::Format
                                             (
                                                _(" (v%d.%d.%d) from Jorgen Bodde"),
                                                WXSERIALIZE_MAJOR_VERSION,
                                                WXSERIALIZE_MINOR_VERSION,
                                                WXSERIALIZE_RELEASE_VERSION
                                             ),
                                             _(" serialize data to files"));
    wxSizer* pThird2 = CreateThirdPartySizer("http://www.wxwidgets.org",
                                             "wxWidgets",
                                             wxString::Format
                                             (
                                                " (v%d.%d.%d)",
                                                wxMAJOR_VERSION,
                                                wxMINOR_VERSION,
                                                wxRELEASE_NUMBER
                                             ),
                                             _(" Cross-Platform GUI Library"));
    wxSizer* pThird3 = CreateThirdPartySizer("http://wxcron.berlios.de",
                                             "wxCron",
                                             "",
                                             _(" The in-build scheduler."));
    wxSizer* pThird4 = CreateThirdPartySizer("http://prismtone.jp",
                                             "FreeFont \"NORDIC round\"",
                                             " from PRISMTONE",
                                             wxT(" 前田 龍 (Ryuichi Maeda) alias PRISMTONE\n created the font used in the blackfiks logo."));


    wxSizer* pTools1 = CreateThirdPartySizer("http://www.codeblocks.org",
                                             "Code::Blocks",
                                             "",
                                             _(" The open source, cross platform C++ IDE."));
    wxSizer* pTools2 = CreateThirdPartySizer("http://www.mingw.org",
                                             "MinGW",
                                             "",
                                             _(" GNU based compiler system for Windows"));
    wxSizer* pTools3 = CreateThirdPartySizer("http://www.gimp.org",
                                             "GIMP",
                                             "",
                                             _(" GNU Image Manipulation Program"));
    wxSizer* pTools4 = CreateThirdPartySizer("http://www.inkscape.org",
                                             "Inkscape",
                                             "",
                                             _(" open source vector graphics editor"));
    wxSizer* pTools5 = CreateThirdPartySizer("http://icofx.ro",
                                             "IcoFX",
                                             "",
                                             _(" icon editor"));
    wxSizer* pTools6 = CreateThirdPartySizer("http://www.7-zip.org",
                                             "7-Zip",
                                             "",
                                             _(" a file archiver with a high compression ratio"));


    wxSizer* pSupporters1 = CreateThirdPartySizer("http://www.berlios.de",
                                                  "BerliOS",
                                                  "",
                                                  _(" free service to Open Source developers"));
    wxSizer* pSupporters2 = CreateThirdPartySizer("http://www.gmane.org",
                                                  "Gmane",
                                                  "",
                                                  _(" mail-to-news-Gateway and\n mailing list archive"));
    wxSizer* pSupporters3 = CreateThirdPartySizer("http://www.inwx.de",
                                                  "InterNetworX",
                                                  "",
                                                  _(" hosting the project domain"));
    wxSizer* pSupporters4 = CreateThirdPartySizer("http://publictimestamp.org",
                                                  "Jan Kechel (Publictimestamp.org)",
                                                  "",
                                                  _(" The maintainer of publictimestamp.org."));

    wxButton* pOk = new wxButton(this, BFABOUTDLG_ID_OK, "OK");

    // arrange
    pNameSizer->Add(pLogo, wxSizerFlags(0).Expand());
    pNameSizer->Add(pName, wxSizerFlags(0).Center());

    pTopSizer->Add(pNameSizer, wxSizerFlags(0).Center().Border(wxBOTTOM, 10));
    pTopSizer->Add(pCopy, wxSizerFlags(0).Center());
    pTopSizer->Add(pDesc, wxSizerFlags(0).Center().Border());
    pTopSizer->Add(pWeb, wxSizerFlags(0).Center());
    pTopSizer->Add(pLicense, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pTopSizer->Add(pViewLicense, wxSizerFlags(0).Center().Border(wxBOTTOM, 10));

    pMLeftSizer->Add(pDev, wxSizerFlags(0).Center());
    pMLeftSizer->Add(pDev1, wxSizerFlags(0).Border(wxBOTTOM, 3));

    pMLeftSizer->Add(pTools, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pMLeftSizer->Add(pTools1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools3, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools4, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools5, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools6, wxSizerFlags(0).Border(wxBOTTOM, 3));

    pMRightSizer->Add(pSupporters, wxSizerFlags(0).Center());
    pMRightSizer->Add(pSupporters1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters3, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters4, wxSizerFlags(0).Border(wxBOTTOM, 3));

    pMRightSizer->Add(pThird, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pMRightSizer->Add(pThird1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pThird2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pThird3, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pThird4, wxSizerFlags(0).Border(wxBOTTOM, 3));

    pMiddleSizer->Add(pMLeftSizer, wxSizerFlags(0).Expand());
    pMiddleSizer->Add(pMRightSizer, wxSizerFlags(0).Expand());

    pTopSizer->Add(pMiddleSizer, wxSizerFlags(0).Expand().Center().Border(wxLEFT | wxRIGHT, 20));
    pTopSizer->Add(pOk, wxSizerFlags(0).Center().Border(wxALL, 25));

    SetSizerAndFit(pTopSizer);
    CenterOnParent();
    ShowModal();
}

/*virtual*/ BFAboutDlg::~BFAboutDlg ()
{
}

wxSizer* BFAboutDlg::CreateThirdPartySizer (const wxString& strUrl,
                                            const wxString& strUrlLabel,
                                            const wxString& strAuthor,
                                            const wxString& strDesc)
{
    wxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* pSubSizer = new wxBoxSizer(wxHORIZONTAL);
    BFHyperlinkCtrl* pUrl = new BFHyperlinkCtrl(this, wxID_ANY, strUrlLabel, strUrl);
    wxFont font = pUrl->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pUrl->SetFont(font);
    pSubSizer->Add(pUrl);
    wxStaticText* pAuthor = new wxStaticText(this, wxID_ANY, strAuthor);
    pSubSizer->Add(pAuthor);
    pTopSizer->Add(pSubSizer);
    wxStaticText* pDesc = new wxStaticText(this, wxID_ANY, strDesc);
    pTopSizer->Add(pDesc);

    return pTopSizer;
}

void BFAboutDlg::OnOk (wxCommandEvent& rEvent)
{
    Close();
}

void BFAboutDlg::OnViewLicense (wxCommandEvent& rEvent)
{
    Close();
    BFMainFrame::Instance()->OnShowLicense (rEvent);
}

void BFAboutDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}
