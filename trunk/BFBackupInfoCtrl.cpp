/**
 * Name:        BFBackupInfoCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-11-13
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


#include "BFBackupInfoCtrl.h"

#include <wx/defs.h>

#include "BFSystem.h"
#include "BFRootTaskApp.h"

BFBackupInfoCtrl::BFBackupInfoCtrl (wxWindow* pParent)
                : wxPanel(pParent),
                  Observer(&(BFSystem::Instance())),
                  lCountWarnings_(0),
                  lCountErrors_(0),
                  lCountFatal_(0),
                  pLogCtrl_(NULL),
                  pWarningsCounter_(NULL),
                  pErrorsCounter_(NULL),
                  pFatalErrorsCounter_(NULL)

{
    Init();
}


/*virtual*/ BFBackupInfoCtrl::~BFBackupInfoCtrl ()
{
}

void BFBackupInfoCtrl::Init ()
{
    wxFont font;

    // log control
    pLogCtrl_ = new wxTextCtrl(this,
                               wxID_ANY,
                               wxEmptyString,
                               wxDefaultPosition,
                               wxSize(100, 300),
                               wxTE_MULTILINE | wxTE_READONLY);

    pLogCtrl_->SetEditable(false);
    font = pLogCtrl_->GetFont();
    font.SetFaceName(_T("Courier New"));
    font.SetFamily(wxFONTFAMILY_MODERN);
    font.SetPointSize(8);
    pLogCtrl_->SetFont(font);

    // message counters
    wxStaticText* pStatVerbose = new wxStaticText(this, wxID_ANY, _("Verbose-Level"));
    wxStaticText* pStatVerboseLvl = new wxStaticText(this, wxID_ANY, BFSystem::GetVerboseString(BFRootTaskApp::Instance().GetVerboseLevel()));
    wxStaticText* pStatWarning = new wxStaticText(this, wxID_ANY, _("Warnings:"));
    wxStaticText* pStatError = new wxStaticText(this, wxID_ANY, _("Errors:"));
    wxStaticText* pStatFatalError = new wxStaticText(this, wxID_ANY, _("Fatal Errors:"));
    font = pStatVerbose->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    pStatVerbose->SetFont(font);
    pStatWarning->SetFont(font);
    pStatError->SetFont(font);
    pStatFatalError->SetFont(font);
    pWarningsCounter_ = new wxStaticText(this, wxID_ANY, _T("000"));
    pErrorsCounter_ = new wxStaticText(this, wxID_ANY, _T("000"));
    pFatalErrorsCounter_ = new wxStaticText(this, wxID_ANY, _T("000"));

    // sizer and arrange
    wxBoxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* pGridSizer = new wxFlexGridSizer(4, 2, 7);
    pGridSizer->Add(pStatVerbose);
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, _T("              ")));
    pGridSizer->Add(pStatWarning);
    pGridSizer->Add(pWarningsCounter_);
    pGridSizer->Add(pStatVerboseLvl, wxSizerFlags(0).Center());
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, _T("              ")));
    pGridSizer->Add(pStatError);
    pGridSizer->Add(pErrorsCounter_);
    pGridSizer->AddStretchSpacer(0);
    pGridSizer->Add(new wxStaticText(this, wxID_ANY, _T("              ")));
    pGridSizer->Add(pStatFatalError);
    pGridSizer->Add(pFatalErrorsCounter_);
    pTopSizer->Add(pGridSizer, wxSizerFlags(0).Border().Center());
    pTopSizer->Add(pLogCtrl_, wxSizerFlags(0).Expand());
    SetSizer(pTopSizer);

    RefreshCounterCtrls();
}

void BFBackupInfoCtrl::RefreshCounterCtrls()
{
    pWarningsCounter_->SetLabel(wxString::Format(_T("%d"), lCountWarnings_));
    pErrorsCounter_->SetLabel(wxString::Format(_T("%d"), lCountErrors_));
    pFatalErrorsCounter_->SetLabel(wxString::Format(_T("%d"), lCountFatal_));
}

/*virtual*/ void BFBackupInfoCtrl::ValueChanged (Subject* pSender)
{
    // check the sender
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    // add message
    if ( pSys->HandleLastMessage(BFRootTaskApp::Instance().GetVerboseLevel()) )
    {
        wxString str;

        str << _T('[') << BFSystem::GetTypeString(pSys->GetLastType()) << _T(']');
        str << _T(' ') << pSys->GetLastMessage() << _T('\n');

        pLogCtrl_->SetInsertionPoint(0);
        pLogCtrl_->WriteText(str);
    }

    // count message type
    switch(pSys->GetLastType())
    {
        case MsgWARNING:
            ++lCountWarnings_;
            break;

        case MsgERROR:
            ++lCountErrors_;
            break;

        case MsgFATAL:
            ++lCountFatal_;
            break;
    };

    RefreshCounterCtrls();
}
