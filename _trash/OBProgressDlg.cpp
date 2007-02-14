/**
 * implementation of class OBProgressDlg
 * 2006-04-17
 ***/

#include "OBProgressDlg.h"
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/utils.h>
#include "Progress.h"


OBProgressDlg::OBProgressDlg (Progress* pProgress, wxWindow* pParent, const wxChar* title, const wxChar* message)
             : wxDialog (pParent, -1, wxString(title)),
               Observer (pProgress),
               pStaticText_(NULL),
               pProgressBar_(NULL)
{
    pStaticText_    = new wxStaticText(this, -1, message, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    pProgressBar_   = new wxGauge(this, -1, 100, wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH);
    pProgressText_  = new wxStaticText(this, -1, _("0000 %"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

    wxSize size1, size2, size3;

    size1 = size2 = pProgressBar_->GetSize();
    size1.SetWidth(size1.GetWidth() * 4);
    pProgressBar_->SetMinSize(size1);

    pProgressText_->Fit();
    size3 = pProgressText_->GetSize();
    pProgressText_->SetMinSize(size3);

    size2.SetWidth(size1.GetWidth() + size3.GetWidth());
    pStaticText_->SetMinSize(size2);

    wxSizer* pSizerVert = new wxBoxSizer (wxVERTICAL);
    wxSizer* pSizerHori = new wxBoxSizer (wxHORIZONTAL);
    wxSizer* pSizer3 = new wxBoxSizer (wxVERTICAL);

    int iSpacer = (size1.GetHeight() - size3.GetHeight()) / 2;
    pSizer3->AddSpacer(iSpacer);
    pSizer3->Add(pProgressText_, wxSizerFlags(0).Left().Border());
    pSizer3->AddSpacer(iSpacer);

    pSizerHori->Add(pProgressBar_, wxSizerFlags(0).Left().Border());
    pSizerHori->Add(pSizer3);

    pSizerVert->Add(pStaticText_, wxSizerFlags(1).Center().Border());
    pSizerVert->Add(pSizerHori, wxSizerFlags(0).Center());

    SetSizerAndFit(pSizerVert);
    Show();
}


//
/*virtual*/ OBProgressDlg::~OBProgressDlg ()
{
    int i = 0;
}


void OBProgressDlg::SetBarValue (Progress* pP)
{
    if (pP == NULL)
        return;

    pProgressBar_->SetValue ( pP->GetProgress() );
    pProgressText_->SetLabel ( wxString::Format(_T("%d %%"), pP->GetProgress()) );
}


/*virtual*/ void OBProgressDlg::ValueChanged (Subject* pSender)
{
    if (pSender != GetSubject())
        return;

    ProgressWithMessage* pPM = dynamic_cast<ProgressWithMessage*>(pSender);

    if (pPM == NULL)
    {   // standard Progress
        Progress* pP = dynamic_cast<Progress*>(pSender);

        if (pP == NULL)
            // error beccause it is any other object
            return;

        SetBarValue(pP);
    }
    else
    {   // progress with message text

        // set progress message
        pStaticText_->SetLabel(wxString::Format(_T("%s\n%s"), pPM->GetLabel(), pPM->GetMessage()));

        SetBarValue(pPM);
    }

    wxWindow* pWindow = GetParent();
    if (pWindow != NULL)
        pWindow->Update();

    /* TODO
    wxPaintDC   dc(pProgressBar_);
    wxRect      rec(pProgressBar_->GetRect());
    dc.SetTextForeground(*wxWHITE);
    dc.DrawText(wxString::Format("%d %%", pP->GetProgress()),
                rec.GetTop(),
                rec.GetLeft());*/
}
