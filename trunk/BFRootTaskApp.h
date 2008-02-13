/**
 * Name:        BFRootTaskApp.h
 * Purpose:     application layer for BFRootTask
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-23
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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


#ifndef BFROOTTASKAPP_H
#define BFROOTTASKAPP_H

#include <wx/wx.h>

#include "BFTaskBase.h"
#include "BFSystemBase.h"

// forwarde declarations
class BFRootTask;
class BFTask;
class BFBackupLog;
class Progress;
class ProgressTotal;
class ProgressWithMessage;
class wxListBox;
class BFBackupTree;
class BFProcessMsgSubject;
class BFTaskListCtrl;

///
class BFRootTaskApp
{
    private:
        ///
        BFRootTask* pRootTask_;
        ///
        static BFRootTaskApp sInstance_;

        /// ctor
        BFRootTaskApp (BFRootTask* pRootTask);

        /** array of log files from the last backup process
            start with the project log */
        wxArrayString       arrLastLogFiles_;
        /** the complete filename of the current open project
            if nothing is open it is empty */
        wxString            strCurrentFilename_;
        /// current running task
        BFTask*             pRunningTask_;
        ///
        bool                bStopProject_;
        ///
        bool                bStopTask_;
        ///
        BFBackupLog*        pBackupLog_;
        ///
        ProgressTotal*          pProgressTotal_;
        ///
        ProgressWithMessage*    pProgressTask_;

    public:
        ///
        static BFRootTaskApp& Instance()
        {
            return sInstance_;
        }

        /// virtual dtor
        virtual ~BFRootTaskApp ();

        ///
        long GetTaskPosition (BFTask* pTask);
        ///
        long GetTaskCount ();

        ///
        const wxChar* GetProjectName();
        ///
        const wxString& GetCurrentFilename ();
        ///
        const wxString& GetBackupLogLocation ();

        ///
        BF_VerboseLevel GetVerboseLevel ();
        ///
        BF_StopLevel GetStopLevelOnFatal ();
        ///
        BF_StopLevel GetStopLevelOnError ();
        ///
        BF_StopLevel GetStopLevelOnWarning ();

        ///
        static wxString GetStopLevelString (BF_StopLevel stopLevel);

        ///
        bool IsProjectModified ();

        /// run all tasks
        bool Run_Start ();
        ///
        bool Run_NextTask ();
        ///
        bool Run_Finished ();


        ///
        Progress* GetProgressTotal ();
        ///
        ProgressWithMessage* GetProgressTask ();

        ///
        void StopCurrentTask ();
        ///
        void StopProject ();
        ///
        bool GetStopCurrentTask ();
        ///
        bool GetStopProject ();

        /** clear 'arrLastLogFiles_' */
        void ClearLastLogFiles ();
        /** insert 'strFilename' as the the project
            log file as the first element of 'arrLastLogFiles_' */
        void SetProjectLogFile (wxString& strFilename);
        /** add 'strFilename' as a task log file to 'arrLastLogFiles_' */
        void AddTaskLogFile (wxString& strFilename);
        ///
        const wxArrayString& GetLastLogFiles ();

        // oid InitThat (wxListBox* pListBox);
        ///
        void InitThat (BFTaskListCtrl* pTaskList);
        ///
        void InitThat (BFBackupTree* pBackupTree);

        ///
        bool StoreToFile (const wxString& strFilename);
        ///
        bool ReadFromFile (const wxString& strFilename);

        ///
        void Close ();

        ///
        void ModifyDestination (const wxString& strOldDestination,
                                const wxString& strNewDestination);

        /// check if all sources and backup-volumeas are there
        bool PreBackupCheck (BFProcessMsgSubject* pMsg);
};

#endif    // BFROOTTASKAPP_H
