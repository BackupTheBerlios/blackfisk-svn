/**
 * Name:        BFBackupProgressDlg.cpp
 * Purpose:     BFBackupProgressDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-09
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFBackupProgressDlg.h"

#include <wx/sizer.h>

#include "BFProgressCtrl.h"
#include "Progress.h"
#include "BFRootTaskApp.h"
#include "BFLogViewDlg.h"
#include "BFBackupQuestionDlg.h"
#include "BFThread_ProjectRunner.h"
#include "BFMainFrame.h"
#include "BFBackupInfoCtrl.h"
#include "BFCore.h"
#include "BFTaskListCtrl.h"
#include "BFTask.h"
#include "BFBackupTree.h"

BEGIN_EVENT_TABLE(BFBackupProgressDlg, wxDialog)
  EVT_CLOSE   (                         BFBackupProgressDlg::OnClose)
  EVT_BUTTON  (BF_BTNID_STOPTASK,       BFBackupProgressDlg::OnStopTask)
  EVT_BUTTON  (BF_BTNID_STOPPRJ,        BFBackupProgressDlg::OnStopProject)
  EVT_MENU    (BF_BACKUPPROGRESSDLG_QUESTION, BFBackupProgressDlg::OnQuestion)
END_EVENT_TABLE()

/*static*/ BFBackupProgressDlg* BFBackupProgressDlg::sp_backup_progress_dlg_ = NULL;

BFBackupProgressDlg::BFBackupProgressDlg (wxWindow* pParent)
                   : wxDialog(pParent, -1, wxString(_("Task Progress")))
{
    sp_backup_progress_dlg_ = this;

    BFMainFrame::Instance()->Iconize();
    Init();
    Show();
    Raise();
    Center();
    pMutex_ = new wxMutex();
    pCondition_ = new wxCondition(*pMutex_);
    BFRootTaskApp::Instance().Run_Start();
}

void BFBackupProgressDlg::OnClose(wxCloseEvent& rEvent)
{
    if (BFCore::Instance().IsWhileBackup())
    {
        rEvent.Veto();
    }
    else
    {
        Destroy();
    }
}

void BFBackupProgressDlg::Init ()
{
    // sizer
    wxBoxSizer*         pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer*         pSizerA         = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer*         pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer*         pSizerB         = new wxBoxSizer(wxVERTICAL);

    // task list
    pCtrlTaskList_      = new BFTaskListCtrl(this);

    // progress ctrls
    pCtrlTaskProgress_  = new BFProgressTaskCtrl(this, BFRootTaskApp::Instance().GetProgressTask());
    pCtrlTotalProgress_ = new BFProgressTotalCtrl(this, BFRootTaskApp::Instance().GetProgressTotal());

    // backup info ctrl
    BFBackupInfoCtrl* pInfoCtrl = new BFBackupInfoCtrl(this);

    // * buttons *
    pButtonSizer->Add (new wxButton(this, BF_BTNID_STOPTASK, _("stop task")),   wxSizerFlags(0).Expand().Border());
    pButtonSizer->Add (new wxButton(this, BF_BTNID_STOPPRJ, _("stop project")), wxSizerFlags(0).Expand().Border());

    // arrange
    pSizerB         ->Add(pCtrlTaskProgress_,   wxSizerFlags(0).Expand().Border(wxBOTTOM, 5));
    pSizerB         ->Add(pInfoCtrl,            wxSizerFlags(0).Expand());
    pSizerA         ->Add(pCtrlTaskList_,       wxSizerFlags(0).Expand());
    pSizerA         ->Add(pSizerB,              wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT));
    pSizerA         ->Add(pCtrlTotalProgress_,  wxSizerFlags(0).Expand().Border(wxRIGHT, 15));
    pTopSizer       ->Add(pSizerA,              wxSizerFlags(0));
    pTopSizer       ->Add(pButtonSizer,         wxSizerFlags(0).Center());

    SetSizerAndFit(pTopSizer);
}


//
/*virtual*/ BFBackupProgressDlg::~BFBackupProgressDlg ()
{
    delete pCondition_;
    sp_backup_progress_dlg_ = NULL;
}

void BFBackupProgressDlg::SetCurrentTask (BFTask* pTask)
{
    if (pTask == NULL || pCtrlTaskProgress_ == NULL || pCtrlTaskList_ == NULL)
        return;

    // get pos of current task
    long lPos = BFRootTaskApp::Instance().GetTaskPosition(pTask);

    // set task name in progress ctrl
    pCtrlTaskProgress_->SetTextA
    (
        wxString::Format(_("%s (%d of %d)"),
                            pTask->GetName(),
                            lPos+1,
                            BFRootTaskApp::Instance().GetTaskCount())
    );

    // set active task in task-list-ctrl
    pCtrlTaskList_->SetTask_Active(pTask->GetOID());
}

void BFBackupProgressDlg::OnStopTask (wxCommandEvent& rEvent)
{
    BFRootTaskApp::Instance().StopCurrentTask();
}

void BFBackupProgressDlg::OnStopProject (wxCommandEvent& rEvent)
{
    BFRootTaskApp::Instance().StopProject();
}

wxMutex* BFBackupProgressDlg::GetMutex ()
{
    return pMutex_;
}

wxCondition* BFBackupProgressDlg::GetCondition ()
{
    return pCondition_;
}

void BFBackupProgressDlg::OnQuestion (wxCommandEvent& rEvent)
{
    // ask questioin
    BF_StopLevel answer = BFBackupQuestionDlg::Ask(rEvent.GetString(), (BFMessageType)rEvent.GetExtraLong());

    // set answer
    BFThread_ProjectRunner::CurrentlyRunning()->SetUsersStopAnswer(answer);

    // resume the asking thread
    wxMutexLocker lock( *pMutex_ );
    GetCondition()->Signal();
}
