/**
 * Name:        BFProjectSettingsCtrl.cpp
 * Purpose:     BFProjectSettingsCtrl class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
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

#include "BFProjectSettingsCtrl.h"

#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/gbsizer.h>

#include "BFProject.h"
#include "BFHelpCtrl.h"

#define BFSTOPLVL_RADIO_WARNING_PRJ  1 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_WARNING_TSK  2 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_WARNING_ASK  3 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_WARNING_IGN  4 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_ERROR_PRJ  5 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_ERROR_TSK  6 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_ERROR_ASK  7 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_ERROR_IGN  8 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_FATAL_PRJ  9 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_FATAL_TSK  10 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_FATAL_ASK  11 + wxID_HIGHEST
#define BFSTOPLVL_RADIO_FATAL_IGN  12 + wxID_HIGHEST

BEGIN_EVENT_TABLE(BFProjectSettingsCtrl, wxPanel)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_WARNING_PRJ, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_WARNING_TSK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_WARNING_ASK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_WARNING_IGN, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_ERROR_PRJ, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_ERROR_TSK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_ERROR_ASK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_ERROR_IGN, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_FATAL_PRJ, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_FATAL_TSK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_FATAL_ASK, BFProjectSettingsCtrl::OnRadio)
    EVT_RADIOBUTTON(BFSTOPLVL_RADIO_FATAL_IGN, BFProjectSettingsCtrl::OnRadio)
END_EVENT_TABLE()


BFProjectSettingsCtrl::BFProjectSettingsCtrl (wxWindow* pParent, BFHelpCtrl* pHelpCtrl)
                     : wxPanel (pParent, wxID_ANY),
                       iStopLevelOnFatal_(BFDO_STOPPRJ),
                       iStopLevelOnError_(BFDO_STOPPRJ),
                       iStopLevelOnWarning_(BFDO_STOPPRJ)
{
    Init(pHelpCtrl);
}

void BFProjectSettingsCtrl::Init (BFHelpCtrl* pHelpCtrl)
{
    // verbose
    wxString arrVerbose[] = { _("information"), _("warning"), _("error"), _("fatal error") };
    wxStaticText* pVerboseLabel = new wxStaticText(this, wxID_ANY, _("verbose level:"));
    pComboVerbose_ = new wxComboBox(this,
                                    wxID_ANY,
                                    arrVerbose[1],
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    4,
                                    arrVerbose,
                                    wxCB_READONLY);
    if (pHelpCtrl != NULL)
    {
        pHelpCtrl->Connect(pVerboseLabel, pComboVerbose_,
                           _("The verbose level while the running backup process. " \
                             "It differs from the application log verbosity level in the global settings!"));
    }

    // StopLevel
    wxSizer* pStopSizer = CreateStopLevelCtrl(pHelpCtrl);

    // backup-log
    wxStaticText* pBackupLogLabel = new wxStaticText(this, wxID_ANY, _("location of logfile:"));
    pPickerBackupLog_ = new wxDirPickerCtrl (this,
                                             wxID_ANY,
                                             wxEmptyString,
                                             _("Select a folder"),
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxDIRP_USE_TEXTCTRL);
    if (pHelpCtrl != NULL)
    {
        pHelpCtrl->Connect(pBackupLogLabel, pPickerBackupLog_,
                           _("It is the directory where the backup-logfile for the backup is stored."));
    }

    // sizer and arrange
    wxGridBagSizer* pSizer = new wxGridBagSizer (5, 5);

    pSizer->Add ( pBackupLogLabel,      wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL );
    pSizer->Add ( pPickerBackupLog_,    wxGBPosition(0, 1), wxGBSpan(1, 2), wxEXPAND);
    pSizer->Add ( pVerboseLabel,        wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL );
    pSizer->Add ( pComboVerbose_,       wxGBPosition(1, 1), wxGBSpan(1, 2), wxEXPAND );
    pSizer->Add ( pStopSizer,           wxGBPosition(2, 0), wxGBSpan(1, 3) );

    SetSizer ( pSizer );
}

//
/*virtual*/ BFProjectSettingsCtrl::~BFProjectSettingsCtrl ()
{
}


wxSizer* BFProjectSettingsCtrl::CreateStopLevelCtrl (BFHelpCtrl* pHelpCtrl)
{
    wxString strTip(_("The message handling while a backup.\n" \
                      " *stop project*: stop the complete backup process\n" \
                      " *stop task*: stop the running task and run the next one\n" \
                      " *ask*: ask the user what should be done\n" \
                      " *log*: just write the message to the logfile"));

    // the lables
    wxStaticText* pLabelWarning = new wxStaticText(this, wxID_ANY, _("...on warnings"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelError   = new wxStaticText(this, wxID_ANY, _("...on errors"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelFatal   = new wxStaticText(this, wxID_ANY, _("...on fatal errors"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelStopPrj = new wxStaticText(this, wxID_ANY, _("stop\nproject"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelStopTsk = new wxStaticText(this, wxID_ANY, _("stop\ntask"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelAsk     = new wxStaticText(this, wxID_ANY, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxStaticText* pLabelIgnore  = new wxStaticText(this, wxID_ANY, _("log"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    if (pHelpCtrl != NULL)
    {
        pHelpCtrl->Connect(pLabelWarning, strTip);
        pHelpCtrl->Connect(pLabelError, strTip);
        pHelpCtrl->Connect(pLabelFatal, strTip);
        pHelpCtrl->Connect(pLabelStopPrj, strTip);
        pHelpCtrl->Connect(pLabelStopTsk, strTip);
        pHelpCtrl->Connect(pLabelAsk, strTip);
        pHelpCtrl->Connect(pLabelIgnore, strTip);
    }

    // size the lables
    pLabelStopPrj->SetMinSize (wxSize(50, pLabelStopPrj->GetSize().GetHeight()));
    pLabelStopTsk->SetMinSize (wxSize(50, pLabelStopTsk->GetSize().GetHeight()));
    pLabelAsk->SetMinSize (wxSize(50, pLabelAsk->GetSize().GetHeight()));
    pLabelIgnore->SetMinSize (wxSize(50, pLabelIgnore->GetSize().GetHeight()));

    // the radio buttons
    pRadioWarningPrj_ = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_WARNING_PRJ,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxRB_GROUP);
    pRadioWarningTsk_ = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_WARNING_TSK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioWarningAsk_ = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_WARNING_ASK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioWarningIgn_ = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_WARNING_IGN,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioErrorPrj_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_ERROR_PRJ,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxRB_GROUP);
    pRadioErrorTsk_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_ERROR_TSK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioErrorAsk_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_ERROR_ASK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioErrorIgn_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_ERROR_IGN,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioFatalPrj_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_FATAL_PRJ,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize,
                                            wxRB_GROUP);
    pRadioFatalTsk_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_FATAL_TSK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioFatalAsk_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_FATAL_ASK,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);
    pRadioFatalIgn_   = new wxRadioButton(this,
                                            BFSTOPLVL_RADIO_FATAL_IGN,
                                            wxEmptyString,
                                            wxDefaultPosition,
                                            wxDefaultSize);

    // hiden dummy radio button to close the last group
    wxRadioButton* pRadioKill = new wxRadioButton(this, wxID_ANY,
                                                        wxEmptyString,
                                                        wxDefaultPosition,
                                                        wxDefaultSize,
                                                        wxRB_GROUP);
    pRadioKill->Hide();

    // arrange
    wxStaticBoxSizer* pStopStaticSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("behavior..."));
    wxGridSizer* pStopGridSizer = new wxFlexGridSizer(5);
    pStopGridSizer->AddStretchSpacer();
    pStopGridSizer->Add(pLabelStopPrj,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxDOWN, 5));
    pStopGridSizer->Add(pLabelStopTsk,  wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxDOWN, 5));
    pStopGridSizer->Add(pLabelAsk,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxDOWN, 5));
    pStopGridSizer->Add(pLabelIgnore,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxDOWN, 5));
    pStopGridSizer->Add(pLabelWarning,      wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxUP | wxDOWN, 3));
    pStopGridSizer->Add(pRadioWarningPrj_,  wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioWarningTsk_,  wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioWarningAsk_,  wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioWarningIgn_,  wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pLabelError,        wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxUP | wxDOWN, 3));
    pStopGridSizer->Add(pRadioErrorPrj_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioErrorTsk_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioErrorAsk_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioErrorIgn_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pLabelFatal,        wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxUP | wxDOWN, 3));
    pStopGridSizer->Add(pRadioFatalPrj_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioFatalTsk_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioFatalAsk_,    wxSizerFlags(0).Align(wxALIGN_CENTER));
    pStopGridSizer->Add(pRadioFatalIgn_,    wxSizerFlags(0).Align(wxALIGN_CENTER));

    pStopStaticSizer->Add(pStopGridSizer);

    return pStopStaticSizer;
}

void BFProjectSettingsCtrl::OnRadio (wxCommandEvent& event)
{
    // on warning
    if (event.GetId() == BFSTOPLVL_RADIO_WARNING_PRJ)
    {
        iStopLevelOnWarning_ = BFDO_STOPPRJ;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_WARNING_TSK)
    {
        iStopLevelOnWarning_ = BFDO_STOPTSK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_WARNING_ASK)
    {
        iStopLevelOnWarning_ = BFDO_ASK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_WARNING_IGN)
    {
        iStopLevelOnWarning_ = BFDO_IGNORE;
        return;
    }

    // on error
    if (event.GetId() == BFSTOPLVL_RADIO_ERROR_PRJ)
    {
        iStopLevelOnError_ = BFDO_STOPPRJ;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_ERROR_TSK)
    {
        iStopLevelOnError_ = BFDO_STOPTSK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_ERROR_ASK)
    {
        iStopLevelOnError_ = BFDO_ASK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_ERROR_IGN)
    {
        iStopLevelOnError_ = BFDO_IGNORE;
        return;
    }

    // on fatal
    if (event.GetId() == BFSTOPLVL_RADIO_FATAL_PRJ)
    {
        iStopLevelOnFatal_ = BFDO_STOPPRJ;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_FATAL_TSK)
    {
        iStopLevelOnFatal_ = BFDO_STOPTSK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_FATAL_ASK)
    {
        iStopLevelOnFatal_ = BFDO_ASK;
        return;
    }

    if (event.GetId() == BFSTOPLVL_RADIO_FATAL_IGN)
    {
        iStopLevelOnFatal_ = BFDO_IGNORE;
        return;
    }
}

/*int BFProjectSettingsCtrl::GetLabelWidth ()
{
    return iLabelWidth_;
}*/

void BFProjectSettingsCtrl::GetData (BFProjectSettings& rSettings)
{
    // verbose
    pComboVerbose_->SetSelection(rSettings.GetVerboseLevel());

    // backup log
    pPickerBackupLog_->SetPath(rSettings.GetBackupLogLocation());

    // set stop level variables
    iStopLevelOnWarning_ = rSettings.GetStopLevelOnWarning();
    iStopLevelOnError_ = rSettings.GetStopLevelOnError();
    iStopLevelOnFatal_ = rSettings.GetStopLevelOnFatal();

    // set radio button for warnings
    switch (iStopLevelOnWarning_)
    {
        case BFDO_STOPPRJ:
            pRadioWarningPrj_->SetValue(true);
            break;

        case BFDO_STOPTSK:
            pRadioWarningTsk_->SetValue(true);
            break;

        case BFDO_ASK:
            pRadioWarningAsk_->SetValue(true);
            break;

        case BFDO_IGNORE:
            pRadioWarningIgn_->SetValue(true);
            break;

        default:
            // should never be reached
            break;
    };

    // set radio button for error
    switch (iStopLevelOnError_)
    {
        case BFDO_STOPPRJ:
            pRadioErrorPrj_->SetValue(true);
            break;

        case BFDO_STOPTSK:
            pRadioErrorTsk_->SetValue(true);
            break;

        case BFDO_ASK:
            pRadioErrorAsk_->SetValue(true);
            break;

        case BFDO_IGNORE:
            pRadioErrorIgn_->SetValue(true);
            break;

        default:
            // should never be reached
            break;
    };

    // set radio button for fatals
    switch (iStopLevelOnFatal_)
    {
        case BFDO_STOPPRJ:
            pRadioFatalPrj_->SetValue(true);
            break;

        case BFDO_STOPTSK:
            pRadioFatalTsk_->SetValue(true);
            break;

        case BFDO_ASK:
            pRadioFatalAsk_->SetValue(true);
            break;

        case BFDO_IGNORE:
            pRadioFatalIgn_->SetValue(true);
            break;

        default:
            // should never be reached
            break;
    };
}

void BFProjectSettingsCtrl::SetData (BFProjectSettings& rSettings)
{
    bool bModified = false;

    // verbose
    if (rSettings.GetVerboseLevel() != pComboVerbose_->GetSelection())
    {
        rSettings.SetVerboseLevel((BF_VerboseLevel)pComboVerbose_->GetSelection());
        bModified = true;
    }

    // backup log
    if (rSettings.GetBackupLogLocation() != pPickerBackupLog_->GetPath())
    {
        rSettings.SetBackupLogLocation(pPickerBackupLog_->GetPath());
        bModified = true;
    }

    // stop levels
    if (rSettings.GetStopLevelOnWarning() != iStopLevelOnWarning_)
    {
        rSettings.SetStopLevelOnWarning(iStopLevelOnWarning_);
        bModified = true;
    }

    if (rSettings.GetStopLevelOnError() != iStopLevelOnError_)
    {
        rSettings.SetStopLevelOnError(iStopLevelOnError_);
        bModified = true;
    }

    if (rSettings.GetStopLevelOnFatal() != iStopLevelOnFatal_)
    {
        rSettings.SetStopLevelOnFatal(iStopLevelOnFatal_);
        bModified = true;
    }

    if (bModified)
        if (&(BFProject::Instance().GetSettings()) == &rSettings)
            BFProject::Instance().SetModified();
}
