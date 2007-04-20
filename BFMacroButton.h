/**
 * BFMacroButton.h
 * definition of class BFMacroButton
 * <comment>
 * 2007-04-18
 ***/

#ifndef BFMACROBUTTON_H
#define BFMACROBUTTON_H

#include <wx/button.h>
#include <wx/textctrl.h>

///
class BFMacroButton : public wxButton
{
    private:
        ///
        wxTextCtrl&         rTextCtrl_;

    public:
        /// constructor
        BFMacroButton (wxWindow* pParent, wxTextCtrl& rTextCtrl);

        /// virtual destructor
        virtual ~BFMacroButton ();

        ///
        void OnPressed(wxCommandEvent& rEvent);
        ///
        void OnMacro(wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFMacroButton

#endif    // BFMACROBUTTON_H
