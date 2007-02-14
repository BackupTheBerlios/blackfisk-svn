/**
 * implementation of class OBTaskProgressDlg
 * 2006-05-09
 ***/

#include <wx/sizer.h>

#include "OBTaskProgressDlg.h"
#include "OBRootTask.h"
#include "Progress.h"


// ++++++++++++++++++++++++++++++
// ++ class OBProgressCtrlBase ++
// ++++++++++++++++++++++++++++++

/*protected*/ OBProgressCtrlBase::OBProgressCtrlBase (wxWindow* pParent,
                                                      Progress* pProgress)
                                   : wxPanel(pParent, -1),
                                     Observer(pProgress),
                                     pBar_(NULL),
                                     pTextA_(NULL),
                                     pTextB_(NULL)
{
}


/*virtual*/ OBProgressCtrlBase::~OBProgressCtrlBase ()
{
    int i = 0;
}


/*virtual*/ void OBProgressCtrlBase::ValueChanged (Subject* pSender)
{
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    UpdateBar   (pP);
    UpdateText  (pP);

    GetParent()->Update();
}


/*virtual*/ void OBProgressCtrlBase::UpdateBar (Progress* pP)
{
    if (pP == NULL || pBar_ == NULL)
        return;

    pBar_->SetValue ( pP->GetProgress() );
}


/*virtual*/ void OBProgressCtrlBase::UpdateText (Progress* pP)
{
    if (pP == NULL || pTextB_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%d %%"), pP->GetProgress()) );
}


void OBProgressCtrlBase::SetTextA (const wxChar* text)
{
    if (text == NULL || pTextA_ == NULL)
        return;

    pTextA_->SetLabel(text);
}


// +++++++++++++++++++++++++++++++
// ++ class OBProgressTotalCtrl ++
// +++++++++++++++++++++++++++++++


OBProgressTotalCtrl::OBProgressTotalCtrl (wxWindow* pParent, Progress* pProgress)
                   : OBProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}

/*virtual*/ OBProgressTotalCtrl::~OBProgressTotalCtrl ()
{
}

/*virtual*/ void OBProgressTotalCtrl::InitControls ()
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
// ++ class OBProgressTaskCtrl ++
// ++++++++++++++++++++++++++++++

/*virtual*/ void OBProgressTaskCtrl::InitControls ()
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


OBProgressTaskCtrl::OBProgressTaskCtrl (wxWindow* pParent, Progress* pProgress)
                    : OBProgressCtrlBase(pParent, pProgress)
{
    InitControls();
}


/*virtual*/ OBProgressTaskCtrl::~OBProgressTaskCtrl ()
{
}


/*virtual*/ void OBProgressTaskCtrl::UpdateText (Progress* pP)
{
    ProgressWithMessage* pPM = dynamic_cast<ProgressWithMessage*>(pP);

    if (pPM == NULL || pTextA_ == NULL || pTextB_ == NULL)
        return;

    pTextB_->SetLabel ( wxString::Format(_T("%s %d %%\n%s"), pPM->GetLabel(), pPM->GetProgress(), pPM->GetMessage()) );
}

// +++++++++++++++++++++++++++++
// ++ class OBTaskProgressDlg ++
// +++++++++++++++++++++++++++++


OBTaskProgressDlg::OBTaskProgressDlg (wxWindow* pParent,
                                      OBRootTask& rRootTask)
                 : wxDialog(pParent, -1, wxString(_("Task Progress"))),
                   pProgressTask_(NULL),
                   pProgressTotal_(NULL)
{
    Init(rRootTask);
}


void OBTaskProgressDlg::Init (OBRootTask& rRootTask)
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
    pCtrlTaskProgress_  = new OBProgressTaskCtrl(this, pProgressTask_);
    pSizerRight         ->Add(pCtrlTaskProgress_);
    pCtrlTotalProgress_ = new OBProgressTotalCtrl(this, pProgressTotal_);
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


Progress* OBTaskProgressDlg::GetProgressTotal ()
{
    return pProgressTotal_;
}


ProgressWithMessage* OBTaskProgressDlg::GetProgressTask ()
{
    return pProgressTask_;
}


//
/*virtual*/ OBTaskProgressDlg::~OBTaskProgressDlg ()
{
    if (pProgressTask_ != NULL)
        delete pProgressTask_;

    if (pProgressTotal_ != NULL)
        delete pProgressTotal_;
}

void OBTaskProgressDlg::SetCurrentTaskName (const wxChar* name)
{
    if (name == NULL || pCtrlTaskProgress_ == NULL)
        return;

    pCtrlTaskProgress_->SetTextA(name);
}
