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

#include <wx/sizer.h>

#include "BFTaskProgressDlg.h"
#include "BFRootTask.h"
#include "Progress.h"


// ++++++++++++++++++++++++++++++
// ++ class BFProgressCtrlBase ++
// ++++++++++++++++++++++++++++++

/*protected*/ BFProgressCtrlBase::BFProgressCtrlBase (wxWindow* pParent,
                                                      Progress* pProgress)
                                   : wxPanel(pParent, -1),
                                     Observer(pProgress),
                                     pBar_(NULL),
                                     pTextA_(NULL),
                                     pTextB_(NULL)
{
}


/*virtual*/ BFProgressCtrlBase::~BFProgressCtrlBase ()
{
}


/*virtual*/ void BFProgressCtrlBase::ValueChanged (Subject* pSender)
{
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    UpdateBar   (pP);
    UpdateText  (pP);

    GetParent()->Update();
}


/*virtual*/ void BFProgressCtrlBase::UpdateBar (Progress* pP)
{
    if (pP == NULL || pBar_ == NULL)
        return;

    pBar_->SetValue ( pP->GetProgress() );
}


/*virtual*/ void BFProgressCtrlBase::UpdateText (Progress* pP)
{
    if (pP == NULL || pTextB_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%d %%"), pP->GetProgress()) );
}


void BFProgressCtrlBase::SetTextA (const wxChar* text)
{
    if (text == NULL || pTextA_ == NULL)
        return;

    pTextA_->SetLabel(text);
}


// +++++++++++++++++++++++++++++++
// ++ class BFProgressTotalCtrl ++
// +++++++++++++++++++++++++++++++


BFProgressTotalCtrl::BFProgressTotalCtrl (wxWindow* pParent, Progress* pProgress)
                   : BFProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}

/*virtual*/ BFProgressTotalCtrl::~BFProgressTotalCtrl ()
{
}

/*virtual*/ void BFProgressTotalCtrl::InitControls ()
{
    if (pTextA_ != NULL || pTextB_ != NULL || pBar_ != NULL)
        return;

    // sizer
    wxStaticBoxSizer*   pTopSizer       = new wxStaticBoxSizer(wxVERTICAL, this);
    wxBoxSizer*         pLabelSizer     = new wxBoxSizer(wxHORIZONTAL);

    // controls
    pTextA_ = new wxStaticText(this, -1, _("Total"));
    pTextB_ = new wxStaticText(this, -1, _T("000 %"));
    pBar_   = new wxGauge(this, -1, 100, wxDefaultPosition, wxSize(300, 30), wxGA_HORIZONTAL);

    // arange
    pLabelSizer ->Add ( pTextA_, wxSizerFlags(0).Border(wxALL, 1) );
    pLabelSizer ->AddSpacer (5);
    pLabelSizer ->Add ( pTextB_, wxSizerFlags(0).Border(wxALL, 1) );
    pTopSizer   ->Add ( pLabelSizer );
    pTopSizer   ->Add ( pBar_, wxSizerFlags(0).Border(wxVERTICAL, 3) );
    SetSizerAndFit( pTopSizer );
}


// ++++++++++++++++++++++++++++++
// ++ class BFProgressTaskCtrl ++
// ++++++++++++++++++++++++++++++

/*virtual*/ void BFProgressTaskCtrl::InitControls ()
{
    if (pTextA_ != NULL || pTextB_ != NULL || pBar_ != NULL)
        return;

    // sizer
    wxStaticBoxSizer* pSizer = new wxStaticBoxSizer(wxVERTICAL, this);

    // controls
    pTextA_ = new wxStaticText(this, -1, _T("<label>"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    pBar_   = new wxGauge(this, -1, 100, wxDefaultPosition, wxSize(300, 15), wxGA_HORIZONTAL | wxGA_SMOOTH);
    pTextB_ = new wxStaticText(this, -1, _T("<message>\n<message2>"));

    // arange
    pSizer->Add(pTextA_, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1) );
    pSizer->Add(pBar_, wxSizerFlags(0).Border(wxVERTICAL, 3));
    pSizer->Add(pTextB_);
    SetSizerAndFit(pSizer);
}


BFProgressTaskCtrl::BFProgressTaskCtrl (wxWindow* pParent, Progress* pProgress)
                    : BFProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}


/*virtual*/ BFProgressTaskCtrl::~BFProgressTaskCtrl ()
{
}


/*virtual*/ void BFProgressTaskCtrl::UpdateText (Progress* pP)
{
    ProgressWithMessage* pPM = dynamic_cast<ProgressWithMessage*>(pP);

    if (pPM == NULL || pTextA_ == NULL || pTextB_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%s %d %%\n%s"), pPM->GetLabel(), pPM->GetProgress(), pPM->GetMessage()) );
}

// +++++++++++++++++++++++++++++
// ++ class BFTaskProgressDlg ++
// +++++++++++++++++++++++++++++


BFTaskProgressDlg::BFTaskProgressDlg (wxWindow* pParent,
                                      BFRootTask& rRootTask)
                 : wxDialog(pParent, -1, wxString(_("Task Progress"))),
                   pProgressTask_(NULL),
                   pProgressTotal_(NULL)
{
    Init(rRootTask);
}


void BFTaskProgressDlg::Init (BFRootTask& rRootTask)
{
    // init progress objects
    pProgressTask_  = new ProgressWithMessage();
    pProgressTotal_ = new ProgressTotal(rRootTask.GetTaskCount(), pProgressTask_);

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
    pCtrlTaskProgress_  = new BFProgressTaskCtrl(this, pProgressTask_);
    pSizerRight         ->Add(pCtrlTaskProgress_);
    pCtrlTotalProgress_ = new BFProgressTotalCtrl(this, pProgressTotal_);
    pSizerRight         ->Add(pCtrlTotalProgress_);
    pSideSizer          ->Add(pSizerRight);

    // * buttons *
    wxButton* pButton   = new wxButton(this, -1, _("pButton"));
    pButtonSizer        ->Add(pButton);

    // init controls
    rRootTask.InitThat(*pListBox_);

    // show the window while creation
    SetSizerAndFit(pTopSizer);
    Show();
}


Progress* BFTaskProgressDlg::GetProgressTotal ()
{
    return pProgressTotal_;
}


ProgressWithMessage* BFTaskProgressDlg::GetProgressTask ()
{
    return pProgressTask_;
}


//
/*virtual*/ BFTaskProgressDlg::~BFTaskProgressDlg ()
{
    if (pProgressTask_ != NULL)
        delete pProgressTask_;

    if (pProgressTotal_ != NULL)
        delete pProgressTotal_;
}

void BFTaskProgressDlg::SetCurrentTaskName (const wxChar* name)
{
    if (name == NULL || pCtrlTaskProgress_ == NULL)
        return;

    pCtrlTaskProgress_->SetTextA(name);
}
