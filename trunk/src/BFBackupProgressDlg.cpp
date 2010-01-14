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
#include "BFBackup.h"
#include "BFLogViewDlg.h"
#include "BFThread_BackupRunner.h"
#include "BFMainFrame.h"
#include "BFBackupInfoCtrl.h"
#include "BFCore.h"
#include "BFTaskListCtrl.h"
#include "BFOperation.h"
#include "BFBackupTree.h"
#include "BFBitmapButton.h"
#include "BFIconTable.h"
#include "BFMessageDlg.h"

#define BFBACKUPPROGRESSDLG_ID_BUTTON_STOPTASK  1 + wxID_HIGHEST
#define BFBACKUPPROGRESSDLG_ID_BUTTON_STOPPRJ   2 + wxID_HIGHEST

BEGIN_EVENT_TABLE(BFBackupProgressDlg, wxDialog)
  EVT_CLOSE   (                                         BFBackupProgressDlg::OnClose)
  EVT_BUTTON  (BFBACKUPPROGRESSDLG_ID_BUTTON_STOPTASK,  BFBackupProgressDlg::OnStopTask)
  EVT_BUTTON  (BFBACKUPPROGRESSDLG_ID_BUTTON_STOPPRJ,   BFBackupProgressDlg::OnStopBackup)
  EVT_MENU    (BF_BACKUPPROGRESSDLG_QUESTION,           BFBackupProgressDlg::OnQuestion)
END_EVENT_TABLE()

/*static*/ BFBackupProgressDlg* BFBackupProgressDlg::sp_backup_progress_dlg_ = NULL;

BFBackupProgressDlg::BFBackupProgressDlg (wxWindow* pParent)
                   : wxDialog(pParent,
                              -1,
                              _("Task Progress"))
{
	// title
	SetTitle(wxString::Format(_("Task Progress (%s)"), BFBackup::Instance().GetProjectName()));

	// static variable
    sp_backup_progress_dlg_ = this;

	// minimize main window
    BFMainFrame::Instance()->Iconize();
    BFMainFrame::Instance()->Disable();

	// init and show
    Init();
    Show();
    Raise();
    Center();

	// start threading
    pMutex_ = new wxMutex();
    pCondition_ = new wxCondition(*pMutex_);
    BFBackup::Instance().Run_Start();
}

void BFBackupProgressDlg::OnClose(wxCloseEvent& rEvent)
{
    if (BFCore::Instance().IsWhileBackup())
    {
        rEvent.Veto();
    }
    else
    {
        BFMainFrame::Instance()->Enable();
        BFMainFrame::Instance()->Raise();
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
    pCtrlTaskProgress_  = new BFProgressTaskCtrl(this, BFBackup::Instance().GetProgressTask());
    pCtrlTotalProgress_ = new BFProgressTotalCtrl(this, BFBackup::Instance().GetProgressTotal());

    // backup info ctrl
    BFBackupInfoCtrl* pInfoCtrl = new BFBackupInfoCtrl(this);

    // * buttons *
    pButtonStopTask_ = new BFBitmapButton (this,
                                           BFBACKUPPROGRESSDLG_ID_BUTTON_STOPTASK,
                                           BFIconTable::GetBitmap(BFIconTable::stop_task),
                                           _("stop task"));

    pButtonStopBackup_ = new BFBitmapButton (this,
                                             BFBACKUPPROGRESSDLG_ID_BUTTON_STOPPRJ,
                                             BFIconTable::GetBitmap(BFIconTable::stop_prj),
                                             _("stop backup"));
    pButtonSizer->Add (pButtonStopTask_, wxSizerFlags(0).Expand().Border());
    pButtonSizer->Add (pButtonStopBackup_, wxSizerFlags(0).Expand().Border());

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

void BFBackupProgressDlg::SetCurrentOperation (BFOperation* pOperation)
{
    if (pOperation == NULL || pCtrlTaskProgress_ == NULL || pCtrlTaskList_ == NULL)
        return;

    // reactivate stop-task button
    pButtonStopTask_->Enable(true);

    // get pos of current task
    long lPos = BFBackup::Instance().GetTaskPosition(pOperation);

    // set task name in progress ctrl
    pCtrlTaskProgress_->SetTextA
    (
        wxString::Format(_("%s (%d of %d)"),
                            pOperation->Task()->GetName(),
                            lPos+1,
                            BFBackup::Instance().GetTaskCount())
    );

    // set active task in task-list-ctrl
    pCtrlTaskList_->SetTask_Active(pOperation->Task()->GetOID());
}

void BFBackupProgressDlg::OnStopTask (wxCommandEvent& rEvent)
{
    BFBackup::Instance().StopCurrentOperation();
    pButtonStopTask_->Enable(false);
}

void BFBackupProgressDlg::OnStopBackup (wxCommandEvent& rEvent)
{
    BFBackup::Instance().StopBackup();
    pButtonStopBackup_->Enable(false);
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
    BFMessageDlg dlg(BF_MSGDLG_BACKUP_QUESTION,
                     rEvent.GetString() + _("\n\nHow to continue?"),
                     wxEmptyString,
                     BFMessageDlg::GetDlgType((BFMessageType)rEvent.GetExtraLong()));

    BF_StopLevel answer = BFDO_ASK;

    switch(dlg.ShowModal())
    {
        case BF_MSGDLG_ID_STOPPRJ:
            answer = BFDO_STOPPRJ;
            break;

        case BF_MSGDLG_ID_STOPTSK:
            answer = BFDO_STOPTSK;
            break;

        case BF_MSGDLG_ID_IGNORE:
            answer = BFDO_IGNORE;
            break;
    };

    // set answer
    BFThread_BackupRunner::CurrentlyRunning()->SetUsersStopAnswer(answer);

    // resume the asking thread
    wxMutexLocker lock( *pMutex_ );
    GetCondition()->Signal();
}
