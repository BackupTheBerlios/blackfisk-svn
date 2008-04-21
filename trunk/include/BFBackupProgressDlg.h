/**
 * Name:        BFBackupProgressDlg.cpp
 * Purpose:     BFBackupProgressDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-09
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

#ifndef BFTASKPROGRESSDLG_H
#define BFTASKPROGRESSDLG_H

#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/thread.h>

#include "ObserverPattern.h"
#include "ids.h"

#define BF_BACKUPPROGRESSDLG_QUESTION       1 + BF_BACKUPPROGRESSDLG_ID_HIGHEST

// forwarde declarations
class Progress;
class ProgressTotal;
class ProgressWithMessage;
class BFProgressTaskCtrl;
class BFProgressTotalCtrl;
class BFTaskListCtrl;
class BFTask;

///
class BFBackupProgressDlg : public wxDialog
{
    private:
        ///
        BFTaskListCtrl*             pCtrlTaskList_;
        ///
        BFProgressTotalCtrl*        pCtrlTotalProgress_;
        ///
        BFProgressTaskCtrl*         pCtrlTaskProgress_;

        ///
        wxMutex*                    pMutex_;
        ///
        wxCondition*                pCondition_;

        ///
        BFBackupProgressDlg();

        ///
        void Init ();

        ///
        static BFBackupProgressDlg*   sp_backup_progress_dlg_;

    protected:
        /// proteced members

    public:
        /// constructor
        BFBackupProgressDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupProgressDlg ();

        ///
        static BFBackupProgressDlg* Instance ()
        { return sp_backup_progress_dlg_; }

        ///
        wxMutex* GetMutex ();
        ///
        wxCondition* GetCondition ();

        ///
        void SetCurrentTask (BFTask* pTask);

        ///
        void OnClose (wxCloseEvent& rEvent);
        ///
        void OnStopTask (wxCommandEvent& rEvent);
        ///
        void OnStopProject (wxCommandEvent& rEvent);
        ///
        void OnQuestion (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFBackupProgressDlg

#endif    // BFTASKPROGRESSDLG_H
