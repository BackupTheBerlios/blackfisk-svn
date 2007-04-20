/**
 * implementation of class BFMacroButton
 * 2007-04-18
 ***/

#include "BFMacroButton.h"

#include "ctrlids.h"
#include "BFSystem.h"
#include "BFTaskBase.h"

BEGIN_EVENT_TABLE(BFMacroButton, wxButton)
    EVT_BUTTON  (BFMACROBUTTON_ID_PRESSED,      BFMacroButton::OnPressed)
    EVT_MENU    (BFMACROBUTTON_ID_MACRO_DATE,   BFMacroButton::OnMacro)
END_EVENT_TABLE()

//
BFMacroButton::BFMacroButton (wxWindow* pParent, wxTextCtrl& rTextCtrl)
             : wxButton (pParent,
                         BFMACROBUTTON_ID_PRESSED,
                         _("macro"),
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxNO_BORDER | wxBU_EXACTFIT),
               rTextCtrl_(rTextCtrl)
{
}


//
/*virtual*/ BFMacroButton::~BFMacroButton ()
{
}

void BFMacroButton::OnPressed(wxCommandEvent& rEvent)
{
    wxMenu menu;
    menu.Append(BFMACROBUTTON_ID_MACRO_DATE, BFTASK_MACRO_DATE);
    PopupMenu(&menu);
}

void BFMacroButton::OnMacro(wxCommandEvent& rEvent)
{
    rTextCtrl_.WriteText(BFTASK_MACRO_DATE);
    rTextCtrl_.SetFocus();
}
