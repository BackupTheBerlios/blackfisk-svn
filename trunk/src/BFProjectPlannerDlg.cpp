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
                               "TEST")
{
    // title
    SetTitle(wxString::Format("Project Planner for %s", BFRootTaskApp::Instance().GetProjectName()));

    // cron ctrl
    pCronCtrl_ = new BFCronCtrl(this);

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
    pDlgSizer->Add(pCronCtrl_,          wxSizerFlags(0).Border());
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
    Close();
}

void BFProjectPlannerDlg::OnButton_Cancel (wxCommandEvent& rEvent)
{
    Close();
}
