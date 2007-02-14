/**
 * OBDirCtrl.h
 * definition of class OBDirCtrl
 * <comment>
 * 2006-05-24
 ***/

#ifndef OBDIRCTRL_H
#define OBDIRCTRL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/dirctrl.h>
#include <wx/button.h>


#define OBDIRCTRL_FILEBUTTON_ONLYDIRS   _("show directories only")
#define OBDIRCTRL_FILEBUTTON_WITHFILES  _("show with files")
#define OBDIRCTRL_ID_FILEBUTTON         1 + wxID_HIGHEST


/** it is the complete control to handle the filesystem */
class OBDirCtrl : public wxPanel
{
    private:
        ///
        wxGenericDirCtrl*   pDirCtrl_;
        ///
        wxButton*           pButtonFiles_;

        ///
        OBDirCtrl ();

    protected:

    public:
        /// constructor
        OBDirCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~OBDirCtrl ();

        ///
        void OnButton_DirCtrl (wxCommandEvent& rEvent);

        ///
        void OnBeginDrag (wxTreeEvent& event);
        ///
        void OnItemMenu (wxTreeEvent& event);

        DECLARE_EVENT_TABLE();
};    // class OBDirCtrl

#endif    // OBDIRCTRL_H
