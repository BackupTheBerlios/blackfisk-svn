/**
 * Name:        BFTaskProgressDlg.cpp
 * Purpose:     BFTaskProgressDlg class implementation
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

#include "BFTaskProgressDlg.h"

#include <wx/sizer.h>

#include "BFProgressCtrl.h"
#include "Progress.h"
#include "BFRootTask.h"
#include "BFLogViewDlg.h"

BEGIN_EVENT_TABLE(BFTaskProgressDlg, wxDialog)
  EVT_CLOSE   (BFTaskProgressDlg::OnClose)
END_EVENT_TABLE()


BFTaskProgressDlg::BFTaskProgressDlg (wxWindow* pParent)
                 : wxDialog(pParent, -1, wxString(_("Task Progress")))
{
    wxGetApp().MainFrame()->Iconize();
    Init();
    Show();
    Raise();
    BFRootTask::Instance().Run_Start(this);
}

void BFTaskProgressDlg::OnClose(wxCloseEvent& rEvent)
{
    if (BFCore::Instance().IsWhileBackup())
    {
        rEvent.Veto();
    }
    else
    {
        //new BFThread_LogView();
        Destroy();
        /*wxMutexGuiEnter();
            BFLogViewDlg* pDlg = new BFLogViewDlg(NULL, BFRootTask::Instance().GetLastLogFiles());
            pDlg->Show();
        wxMutexGuiLeave();*/
        //wxGetApp().MainFrame()->Show();
    }
}

void BFTaskProgressDlg::Init ()
{
    // sizer
    wxBoxSizer*         pTopSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer*         pSideSizer      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer*         pButtonSizer    = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer*         pSizerRight     = new wxBoxSizer(wxVERTICAL);

    // arange sizers
    pTopSizer   ->Add(pSideSizer);
    pTopSizer   ->Add(pButtonSizer);

    // controls for * left side *
    pListBox_           = new wxListBox(this, -1);
    pListBox_           ->SetMinSize(wxSize(100, 400));
    pSideSizer          ->Add(pListBox_);

    // controls for * right side *
    pCtrlTaskProgress_  = new BFProgressTaskCtrl(this, BFRootTask::Instance().GetProgressTask());
    pSizerRight         ->Add(pCtrlTaskProgress_);
    pCtrlTotalProgress_ = new BFProgressTotalCtrl(this, BFRootTask::Instance().GetProgressTotal());
    pSizerRight         ->Add(pCtrlTotalProgress_);
    pSideSizer          ->Add(pSizerRight);

    // * buttons *
    wxButton* pButton   = new wxButton(this, -1, _("pButton"));
    pButtonSizer        ->Add(pButton);

    // init controls
    BFRootTask::Instance().InitThat(*pListBox_);

    SetSizerAndFit(pTopSizer);
}


//
/*virtual*/ BFTaskProgressDlg::~BFTaskProgressDlg ()
{
}

void BFTaskProgressDlg::SetCurrentTaskName (const wxChar* name)
{
    if (name == NULL || pCtrlTaskProgress_ == NULL)
        return;

    pCtrlTaskProgress_->SetTextA(name);
}
