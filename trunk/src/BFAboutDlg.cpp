/**
 * Name:        BFAboutDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-08-19
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


#include "BFAboutDlg.h"

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/statbmp.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFIconTable.h"
#include "BFHyperlinkCtrl.h"
#include "BFApp.h"

#define BFABOUTDLG_ID_VIEWLICENSE   1 + wxID_HIGHEST
#define BFABOUTDLG_ID_OK            2 + wxID_HIGHEST

BEGIN_EVENT_TABLE(BFAboutDlg, wxDialog)
  EVT_CLOSE     (                           BFAboutDlg::OnClose)
  EVT_BUTTON    (BFABOUTDLG_ID_VIEWLICENSE, BFAboutDlg::OnViewLicense)
  EVT_BUTTON    (BFABOUTDLG_ID_OK,          BFAboutDlg::OnOk)
END_EVENT_TABLE()


BFAboutDlg::BFAboutDlg ()
          : wxDialog(NULL,
                     wxID_ANY,
                     wxString(_("About ")) + BF_PRGNAME,
                     wxDefaultPosition,
                     wxDefaultSize,
                     wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
{
    wxFont font;

    // sizers
    wxBoxSizer* pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pNameSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxGridSizer* pMiddleSizer   = new wxGridSizer(2);
    wxBoxSizer* pMLeftSizer     = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pMRightSizer    = new wxBoxSizer(wxVERTICAL);

    //
    wxStaticBitmap* pLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxString::Format(_T("%s%s"), BF_GRAPHICDIR, _T("logo2.png")), wxBITMAP_TYPE_PNG));
    wxStaticText* pName = new wxStaticText(this, wxID_ANY, BFApp::GetFullApplicationName());
    font = pName->GetFont();
    font.SetPointSize(font.GetPointSize()+5);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pName->SetFont(font);
    wxStaticText* pCopy = new wxStaticText(this, wxID_ANY, _T("Copyright (C) 2005 Christian Buhtz <blackfisk@web.de>"));
    wxStaticText* pDesc = new wxStaticText(this, wxID_ANY, _T("A backup software that can handle very complex tasks.\nBut it has the goal that it is easy to use\nwithout reading a manual."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    BFHyperlinkCtrl* pWeb = new BFHyperlinkCtrl(this, wxID_ANY, _T("www.blackfisk.org"), _T("http://www.blackfisk.org"));
    wxStaticText* pLicense = new wxStaticText(this, wxID_ANY, _T("This program comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome\nto redistribute it under certain conditions;\nfor details select the menu Help/License\nor click this button"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxButton* pViewLicense = new wxButton(this, BFABOUTDLG_ID_VIEWLICENSE, _T("view License (GPLv3)"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    font = pLicense->GetFont();
    font.SetFaceName(_T("Courier New"));
    font.SetPointSize(8);
    pLicense->SetFont(font);

    wxStaticText* pDev = new wxStaticText(this, wxID_ANY, _T("Developers"));
    // Artists
    wxStaticText* pThird = new wxStaticText(this, wxID_ANY, _T("3rd-Party-Components"));
    wxStaticText* pTools = new wxStaticText(this, wxID_ANY, _T("used Tools"));
    wxStaticText* pSupporters = new wxStaticText(this, wxID_ANY, _T("Supporters"));
    font = pDev->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    font.SetPointSize(font.GetPointSize()+1);
    pDev->SetFont(font);
    pThird->SetFont(font);
    pTools->SetFont(font);
    pSupporters->SetFont(font);
    wxStaticText* pDev1 = new wxStaticText(this, wxID_ANY, _T("Christian Buhtz"));

    wxSizer* pThird1 = CreateThirdPartySizer(_T("http://wastebucket.solidsteel.nl/?page_id=5"),
                                             _T("wxSerialize"),
                                             _T(" from Jorgen Bodde"),
                                             _T(" serialize data to files"));
    wxSizer* pThird2 = CreateThirdPartySizer(_T("http://www.wxwidgets.org"),
                                             _T("wxWidgets"),
                                             _T(""),
                                             _T(" Cross-Platform GUI Library"));
    wxSizer* pTools1 = CreateThirdPartySizer(_T("http://www.codeblocks.org"),
                                             _T("Code::Blocks"),
                                             _T(""),
                                             _T(" The open source, cross platform C++ IDE."));
    wxSizer* pTools2 = CreateThirdPartySizer(_T("http://www.mingw.org"),
                                             _T("MinGW"),
                                             _T(""),
                                             _T(" GNU based compiler system for Windows"));
    wxSizer* pSupporters1 = CreateThirdPartySizer(_T("http://www.berlios.de"),
                                                  _T("BerliOS"),
                                                  _T(""),
                                                  _T(" free service to Open Source developers"));
    wxSizer* pSupporters2 = CreateThirdPartySizer(_T("http://www.gmane.org"),
                                                  _T("Gmane"),
                                                  _T(""),
                                                  _T(" mail-to-news-Gateway and\n mailing list archive"));
    wxSizer* pSupporters3 = CreateThirdPartySizer(_T("http://www.inwx.de"),
                                                  _T("InterNetworX"),
                                                  _T(""),
                                                  _T(" hosting the project domain"));
    wxButton* pOk = new wxButton(this, BFABOUTDLG_ID_OK, _T("OK"));

    // arrange
    pNameSizer->Add(pLogo, wxSizerFlags(0).Expand().Border());
    pNameSizer->Add(pName, wxSizerFlags(0).Center());
    pTopSizer->Add(pNameSizer, wxSizerFlags(0).Center());
    pTopSizer->Add(pCopy, wxSizerFlags(0).Center());
    pTopSizer->Add(pDesc, wxSizerFlags(0).Center().Border());
    pTopSizer->Add(pWeb, wxSizerFlags(0).Center());
    pTopSizer->Add(pLicense, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pTopSizer->Add(pViewLicense, wxSizerFlags(0).Center().Border(wxBOTTOM, 10));
    pMLeftSizer->Add(pDev, wxSizerFlags(0).Center());
    pMLeftSizer->Add(pDev1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pThird, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pMLeftSizer->Add(pThird1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pThird2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools, wxSizerFlags(0).Center().Border(wxTOP, 10));
    pMLeftSizer->Add(pTools1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMLeftSizer->Add(pTools2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters, wxSizerFlags(0).Center());
    pMRightSizer->Add(pSupporters1, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters2, wxSizerFlags(0).Border(wxBOTTOM, 3));
    pMRightSizer->Add(pSupporters3, wxSizerFlags(0).Border(wxBOTTOM, 3));
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

wxSizer* BFAboutDlg::CreateThirdPartySizer (const wxChar* strUrl,
                                            const wxChar* strUrlLabel,
                                            const wxChar* strAuthor,
                                            const wxChar* strDesc)
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
