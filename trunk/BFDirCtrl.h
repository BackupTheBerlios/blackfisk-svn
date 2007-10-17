/**
 * Name:        BFDirCtrl.h
 * Purpose:     BFDirCtrl class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-24
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef BFDIRCTRL_H
#define BFDIRCTRL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/dirctrl.h>
#include <wx/button.h>
#include <wx/tglbtn.h>


/** it is the complete control to handle the filesystem */
class BFDirCtrl : public wxPanel
{
    private:
        ///
        wxGenericDirCtrl*   pDirCtrl_;
        ///
        wxToggleButton*     pButtonFiles_;

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
        void OnDestination (wxCommandEvent& event);
        ///
        void OnBackup (wxCommandEvent& event);

        ///
        void OnBeginDrag (wxTreeEvent& event);
        ///
        void OnItemMenu (wxTreeEvent& event);

        DECLARE_EVENT_TABLE();
};    // class BFDirCtrl

#endif    // BFDIRCTRL_H
