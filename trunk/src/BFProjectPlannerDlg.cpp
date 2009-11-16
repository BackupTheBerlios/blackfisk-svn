/**
 * Name:        BFProjectPlannerDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-06-30
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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
#include "BFBackup.h"
#include "BFCore.h"
#include "BFSettings.h"
#include "ids.h"

#include <wx/tglbtn.h>

#define BFPROJECTPLANNERDLG_ID_BUTTONOK          1 + BF_PROJECTPLANNER_ID_HIGHEST
#define BFPROJECTPLANNERDLG_ID_BUTTONCANCEL      2 + BF_PROJECTPLANNER_ID_HIGHEST
#define BFPROJECTPLANNERDLG_ID_BUTTONSCHEDULE    3 + BF_PROJECTPLANNER_ID_HIGHEST

BEGIN_EVENT_TABLE(BFProjectPlannerDlg, wxDialog)
  EVT_CLOSE         (                                       BFProjectPlannerDlg::OnClose)
  EVT_BUTTON        (BFPROJECTPLANNERDLG_ID_BUTTONOK,       BFProjectPlannerDlg::OnButton_Ok)
  EVT_BUTTON        (BFPROJECTPLANNERDLG_ID_BUTTONCANCEL,   BFProjectPlannerDlg::OnButton_Cancel)
  EVT_TOGGLEBUTTON  (BFPROJECTPLANNERDLG_ID_BUTTONSCHEDULE, BFProjectPlannerDlg::OnButton_Schedule)
END_EVENT_TABLE()


BFProjectPlannerDlg::BFProjectPlannerDlg (wxWindow* pParent)
                   : wxDialog (pParent,
                               wxID_ANY,
                               "TEST"),
                     pCronCtrl_(NULL)
{
    // title
    SetTitle(wxString::Format("Project Planner for %s", BFBackup::Instance().GetProjectName()));

    // get crontabline
    wxString        strCrontabLine = BFBackup::Instance().GetCrontabline();

	// cron ctrl
	pCronCtrl_ = new BFCronCtrl(this, strCrontabLine);

    // schedule button
    pButtonSchedule_ = new wxToggleButton(this,
                                          BFPROJECTPLANNERDLG_ID_BUTTONSCHEDULE,
                                          _("Scheduler activated"));

    // standard buttons
    BFBitmapButton* pButtonOk     = new BFBitmapButton(this,
                                                       BFPROJECTPLANNERDLG_ID_BUTTONOK,
                                                       BFIconTable::Instance().GetBitmap(BFIconTable::ok),
                                                       "OK");
    BFBitmapButton* pButtonCancel = new BFBitmapButton(this,
                                                       BFPROJECTPLANNERDLG_ID_BUTTONCANCEL,
                                                       BFIconTable::Instance().GetBitmap(BFIconTable::cancel),
                                                       _("Cancel"));

    // scheduler active?
    ToogleButton_Schedule ( !(strCrontabLine.IsEmpty()) );

    // sizer, arrange and show
    wxBoxSizer* pDlgSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(pButtonOk,        wxSizerFlags(0).Border());
    pButtonSizer->Add(pButtonCancel,    wxSizerFlags(0).Border());
	pDlgSizer->Add (pButtonSchedule_,   wxSizerFlags(0).Center().DoubleBorder());
	pDlgSizer->Add(pCronCtrl_,          wxSizerFlags(0).Border());
	pDlgSizer->Add(pButtonSizer,        wxSizerFlags(0).Center().Border(wxALL, 10));
    SetSizerAndFit(pDlgSizer);
    CenterOnParent();
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
		pCronCtrl_->SetData();

        if ( pButtonSchedule_->GetValue() )
        {
            wxString strOld = BFBackup::Instance().GetCrontabline();
            wxString strNew = pCronCtrl_->GetCrontabline();

            if (strOld != strNew)
                BFCore::Instance().ReplaceLineInFile(BFSettings::Instance().GetCrontab(), strOld, strNew);
        }
        else
        {
            BFCore::Instance().DeleteLineInFile(BFSettings::Instance().GetCrontab(),
                                                BFBackup::Instance().GetCrontabline());
        }
    }

    Close();
}

void BFProjectPlannerDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}

void BFProjectPlannerDlg::OnButton_Schedule (wxCommandEvent& rEvent)
{
    ToogleButton_Schedule(pButtonSchedule_->GetValue());
}

void BFProjectPlannerDlg::ToogleButton_Schedule (bool bValue)
{
    if ( bValue != pButtonSchedule_->GetValue() )
        pButtonSchedule_->SetValue (bValue);

    if ( bValue )
    {
        pButtonSchedule_->SetLabel(_("Scheduler active"));
        if (pCronCtrl_)
            pCronCtrl_->Enable();
    }
    else
    {
        pButtonSchedule_->SetLabel(_("Scheduler inactive"));
        if (pCronCtrl_)
            pCronCtrl_->Enable(false);
    }
}
