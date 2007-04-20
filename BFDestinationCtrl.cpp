/**
 * implementation of class BFDestinationCtrl
 * 2007-03-28
 ***/

#include "BFDestinationCtrl.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "BFSystem.h"
#include "BFTaskDlg.h"
#include "BFMainFrame.h"
#include "BFMacroButton.h"
#include "ctrlids.h"

//
BFDestinationCtrl::BFDestinationCtrl (wxWindow* pParent, const wxChar* strPath /*= wxEmptyString*/)
                 : wxPanel(pParent),
                   pPickerCtrl_(NULL)
{
    Init();

    pPickerCtrl_->SetPath(strPath);
}

//
/*virtual*/ BFDestinationCtrl::~BFDestinationCtrl ()
{
}


void BFDestinationCtrl::Init ()
{
    // label
    wxStaticText* pLabel = new wxStaticText (this, -1, _("destination path:"));

    // panel
    wxPanel* pPanel = new wxPanel(this);

    // control
    pPickerCtrl_ = new wxDirPickerCtrl
                   (
                        pPanel,
                        wxID_ANY,
                        wxEmptyString,
                        _("select a destination folder"),
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxDIRP_USE_TEXTCTRL
                    );
    pPickerCtrl_->SetTextCtrlProportion(4);
    pPickerCtrl_->SetPickerCtrlProportion(1);

    // macro button
    wxButton* pMacroButton = new BFMacroButton(pPanel, *(pPickerCtrl_->GetTextCtrl()));

    // arrange/layout panel
    wxBoxSizer* pSubSizer = new wxBoxSizer(wxHORIZONTAL);
    pSubSizer->Add(pPickerCtrl_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Expand());
    pSubSizer->Add(pMacroButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPanel->SetSizer(pSubSizer);

    // set size
    pLabel->SetMinSize(wxSize(BFTaskDlg::lWidth1_, pLabel->GetSize().GetHeight()));
    pPanel->SetMinSize(wxSize(BFTaskDlg::lWidth2_, pPanel->GetSize().GetHeight()+3));

    // arrange/layout the main control
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
    pSizer->Add(pLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSizer->Add(pPanel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Expand());
    SetSizer(pSizer);
}

wxString BFDestinationCtrl::GetPath ()
{
    return pPickerCtrl_->GetPath();
}

void BFDestinationCtrl::SetPath (const wxString& strPath)
{
    pPickerCtrl_->SetPath(strPath);
}
