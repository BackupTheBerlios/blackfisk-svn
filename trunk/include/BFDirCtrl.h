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
#include <wx/checkbox.h>

#define BFDIRCTRL_ID_BACKUP_FILECOPY        BFBACKUPCTRL_ID_COPY_FILE
#define BFDIRCTRL_ID_BACKUP_DIRCOPY         BFBACKUPCTRL_ID_COPY_DIR
#define BFDIRCTRL_ID_BACKUP_SYNCDIR         BFBACKUPCTRL_ID_SYNC_DIR
#define BFDIRCTRL_ID_ARCHIVEDIR             BFBACKUPCTRL_ID_ARCHIVE_DIR
#define BFDIRCTRL_ID_CHECK_WITHFILES        1 + BF_DIRCTRL_ID_HIGHEST
#define BFDIRCTRL_ID_ADDDESTINATION         2 + BF_DIRCTRL_ID_HIGHEST
#define BFDIRCTRL_ID_CREATEDESTINATION      3 + BF_DIRCTRL_ID_HIGHEST
#define BFDIRCTRL_ID_CHECK_HIDDEN           4 + BF_DIRCTRL_ID_HIGHEST

/** it is the complete control to handle the filesystem */
class BFDirCtrl : public wxPanel
{
    private:
        ///
        wxGenericDirCtrl*   pDirCtrl_;
        ///
        wxCheckBox*         pCheckWithFiles_;
        ///
        wxCheckBox*         pCheckHidden_;

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
        void OnCheck_WithFiles (wxCommandEvent& rEvent);
        ///
        void OnCheck_Hidden (wxCommandEvent& rEvent);
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
