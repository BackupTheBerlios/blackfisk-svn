/**
 * BFDirCtrl.h
 * definition of class BFDirCtrl
 * <comment>
 * 2006-05-24
 ***/

#ifndef BFDIRCTRL_H
#define BFDIRCTRL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/dirctrl.h>
#include <wx/button.h>


#define BFDIRCTRL_FILEBUTTON_ONLYDIRS   _("show directories only")
#define BFDIRCTRL_FILEBUTTON_WITHFILES  _("show with files")

#define BFDIRCTRL_ID_FILEBUTTON             1 + wxID_HIGHEST
#define BFDIRCTRL_ID_ADDDESTINATION         2 + wxID_HIGHEST


/** it is the complete control to handle the filesystem */
class BFDirCtrl : public wxPanel
{
    private:
        ///
        wxGenericDirCtrl*   pDirCtrl_;
        ///
        wxButton*           pButtonFiles_;

        /** the last (by right-click) selected item;
            is normaly set by OnItemMenu() */
        wxTreeItemId        lastItemId_;

        ///
        BFDirCtrl ();

    protected:

    public:
        /// constructor
        BFDirCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFDirCtrl ();

        ///
        void OnButton_DirCtrl (wxCommandEvent& rEvent);
        ///
        void OnAddAsDestination (wxCommandEvent& event);

        ///
        void OnBeginDrag (wxTreeEvent& event);
        ///
        void OnItemMenu (wxTreeEvent& event);

        DECLARE_EVENT_TABLE();
};    // class BFDirCtrl

#endif    // BFDIRCTRL_H
