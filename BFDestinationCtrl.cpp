/**
 * implementation of class BFDestinationCtrl
 * 2007-03-28
 ***/

#include "BFDestinationCtrl.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/colour.h>

#include "BFSystem.h"
#include "BFTaskDlg.h"
#include "BFMainFrame.h"

BEGIN_EVENT_TABLE(BFDestinationCtrl, wxPanel)
    EVT_BUTTON  (BFDESTCTRL_ID_MACROBUTTON,     BFDestinationCtrl::OnButton)
    EVT_MENU    (BFDESTCTRL_ID_MACRO_DATE,      BFDestinationCtrl::OnMacro)
END_EVENT_TABLE()

//
BFDestinationCtrl::BFDestinationCtrl (wxWindow* pParent, const wxChar* strPath /*= wxEmptyString*/)
                 : wxPanel(pParent),
                   pPickerCtrl_(NULL),
                   pMacroButton_(NULL)
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
    //pPanel->SetBackgroundColour(wxColour(100, 10, 10));

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
    pMacroButton_ = new wxButton
                    (
                        pPanel,
                        BFDESTCTRL_ID_MACROBUTTON,
                        _("macro"),
                        wxDefaultPosition,
                        wxDefaultSize,
                        wxNO_BORDER | wxBU_EXACTFIT
                    );

    // arrange/layout panel
    wxBoxSizer* pSubSizer = new wxBoxSizer(wxHORIZONTAL);
    pSubSizer->Add(pPickerCtrl_, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Expand());
    pSubSizer->Add(pMacroButton_, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
    pPanel->SetSizer(pSubSizer);

    // set size
    //BFTaskBaseDlg::SetRowSize(pLabel, pPanel);
    pLabel->SetMinSize(wxSize(BFTaskBaseDlg::lWidth1_, pLabel->GetSize().GetHeight()));
    pPanel->SetMinSize(wxSize(BFTaskBaseDlg::lWidth2_, pPanel->GetSize().GetHeight()+3));

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

void BFDestinationCtrl::OnButton(wxCommandEvent& rEvent)
{
    wxMenu menu;
    menu.Append(BFDESTCTRL_ID_MACRO_DATE, BFTASK_MACRO_DATE);
    pMacroButton_->PopupMenu(&menu);
}

void BFDestinationCtrl::OnMacro(wxCommandEvent& rEvent)
{
    pPickerCtrl_->GetTextCtrl()->WriteText(BFTASK_MACRO_DATE);
    pPickerCtrl_->GetTextCtrl()->SetFocus();
}
