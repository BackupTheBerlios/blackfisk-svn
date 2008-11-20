/**
 * Name:        BFProjectPlannerDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-06-30
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


#include "BFProjectPlannerDlg.h"

#include "BFCronCtrl.h"
#include "BFBitmapButton.h"
#include "BFIconTable.h"
#include "BFRootTaskApp.h"
#include "BFCore.h"
#include "BFSettings.h"
#include "ids.h"

#define BFPROJECTPLANNERDLG_ID_BUTTONOK          1 + BF_PROJECTPLANNER_ID_HIGHEST
#define BFPROJECTPLANNERDLG_ID_BUTTONCANCEL      2 + BF_PROJECTPLANNER_ID_HIGHEST

BEGIN_EVENT_TABLE(BFProjectPlannerDlg, wxDialog)
  EVT_CLOSE     (                                       BFProjectPlannerDlg::OnClose)
  EVT_BUTTON    (BFPROJECTPLANNERDLG_ID_BUTTONOK,       BFProjectPlannerDlg::OnButton_Ok)
  EVT_BUTTON    (BFPROJECTPLANNERDLG_ID_BUTTONCANCEL,   BFProjectPlannerDlg::OnButton_Cancel)
END_EVENT_TABLE()


BFProjectPlannerDlg::BFProjectPlannerDlg (wxWindow* pParent)
                   : wxDialog (pParent,
                               wxID_ANY,
                               "TEST"),
                     pCronCtrl_(NULL)
{
    // title
    SetTitle(wxString::Format("Project Planner for %s", BFRootTaskApp::Instance().GetProjectName()));

    // get crontabline
    wxArrayString arr;
    BFRootTaskApp::ParseCrontabline(BFRootTaskApp::Instance().GetCrontabline(), arr);

    wxStaticText* pStatic = NULL;

    // check crontabline
    if ( BFCronCtrl::IsCrontablineUsable(arr) )
    {
        // cron ctrl
        pCronCtrl_ = new BFCronCtrl(this, arr);
    }
    else
    {
        wxString str = _("Error: The given crontab line is not editable for the project planer.\n" \
                         "Maybe the crontab line was edited directly or by another application.\n" \
                         "\nThis is the line:\n");
        str = str + wxJoin(arr, ' ');

        pStatic = new wxStaticText(this, wxID_ANY, str);
    }

    // buttons
    BFBitmapButton* pButtonOk     = new BFBitmapButton(this,
                                                       BFPROJECTPLANNERDLG_ID_BUTTONOK,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::ok),
                                                       "OK");
    BFBitmapButton* pButtonCancel = new BFBitmapButton(this,
                                                       BFPROJECTPLANNERDLG_ID_BUTTONCANCEL,
                                                       BFIconTable::Instance()->GetBitmap(BFIconTable::cancel),
                                                       _("Cancel"));

    // sizer, arrange and show
    wxBoxSizer* pDlgSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
    if (pCronCtrl_)
        pDlgSizer->Add(pCronCtrl_,          wxSizerFlags(0).Border());
    else
        pDlgSizer->Add(pStatic,          wxSizerFlags(0).DoubleBorder());
    pDlgSizer->Add(pButtonSizer,        wxSizerFlags(0).Center());
    SetSizerAndFit(pDlgSizer);
    ShowModal();
}


/*virtual*/ BFProjectPlannerDlg::~BFProjectPlannerDlg ()
{
}


void BFProjectPlannerDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}

void BFProjectPlannerDlg::OnButton_Ok (wxCommandEvent& rEvent)
{
    if (pCronCtrl_)
    {
        wxString strOld = BFRootTaskApp::Instance().GetCrontabline();
        wxString strNew = pCronCtrl_->GetCrontabline();

        if (strOld != strNew)
            BFCore::Instance().ReplaceLineInFile(BFSettings::Instance().GetCrontab(), strOld, strNew);
    }

    Close();
}

void BFProjectPlannerDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}