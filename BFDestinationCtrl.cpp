/**
 * implementation of class BFDestinationCtrl
 * 2007-03-28
 ***/

#include "BFDestinationCtrl.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "BFSystem.h"
#include "BFTaskDlg.h"

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

    // control
    pPickerCtrl_ = new wxDirPickerCtrl
                   (
                        this,
                        wxID_ANY,
                        wxEmptyString,
                        _("select a destination folder"),
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxDIRP_USE_TEXTCTRL
                    );

    // set size
    BFTaskBaseDlg::SetRowSize(pLabel, pPickerCtrl_);

    // sizer
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);

    // arrange/layout
    pSizer->Add(pLabel, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pSizer->Add(pPickerCtrl_);
    SetSizer(pSizer);
}

wxString BFDestinationCtrl::GetPath ()
{
    return pPickerCtrl_->GetPath();
}
