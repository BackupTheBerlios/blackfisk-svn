/**
 * BFDestinationCtrl.h
 * definition of class BFDestinationCtrl
 * <comment>
 * 2007-03-28
 ***/

#ifndef BFDESTINATIONCTRL_H
#define BFDESTINATIONCTRL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/filepicker.h>

/// a control to edit destinations
class BFDestinationCtrl : public wxPanel
{
    private:
        ///
        wxDirPickerCtrl*        pPickerCtrl_;

        ///
        BFDestinationCtrl ();
        ///
        void Init ();

    public:
        /// constructor
        BFDestinationCtrl (wxWindow* pParent, const wxChar* strPath = wxEmptyString);

        /// virtual destructor
        virtual ~BFDestinationCtrl ();

        ///
        wxString GetPath ();
        //DECLARE_EVENT_TABLE();
};    // class BFDestinationCtrl

#endif    // BFDESTINATIONCTRL_H
