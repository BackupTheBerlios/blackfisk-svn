/**
 * Name:        BFBackup.h
 * Purpose:     application layer
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-22
 *              (based on BFRootTaskApp)
 * Copyright:   (c) 2009 Christian Buhtz <exsudat@gmx.de>
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


#ifndef BFBACKUP_H
#define BFBACKUP_H

#include <wx/wx.h>

#include "BFTaskBase.h"
#include "BFSystemBase.h"
#include "BFOperation.h"

// forwarde declarations
class BFProject;
class BFBackupLog;
class Progress;
class ProgressTotal;
class ProgressWithMessage;
class wxListBox;
class BFBackupTree;
class BFTaskListCtrl;

/** This class represent a complete backup process.
    It is the representation of the backup project in the application layer. */
class BFBackup
{
    private:
        ///
        BFProject* pProject_;

        ///
        BFOperationVector           vecOperations_;
        ///
        long                        lRunningOperation_;

        ///
        static BFBackup sInstance_;

        /// ctor
        BFBackup (BFProject* pProject);

        /** Array of log files from the last backup process
            start with the project log. */
        wxArrayString       arrLastLogFiles_;
        /** The complete filename of the current open project
            if nothing is open it is empty. */
        wxString            strCurrentFilename_;

        /// current running backup operation
        BFOperation*        pRunningOperation_;

        ///
        bool                bStopBackup_;
        ///
        bool                bStopOperation_;
        ///
        BFBackupLog*        pBackupLog_;
        ///
        ProgressTotal*          pProgressTotal_;
        ///
        ProgressWithMessage*    pProgressTask_;

        ///
        void ClearOperationVector ();

    public:
        ///
        static BFBackup& Instance()
        {
            return sInstance_;
        }

        /// virtual dtor
        virtual ~BFBackup ();

        ///
        long GetTaskPosition (BFOperation* pOperation);
        ///
        long GetTaskCount ();

        ///
        const wxString& GetProjectName();
        ///
        const wxString& GetCurrentFilename ();
        ///
        const wxString& GetBackupLogLocation ();

        ///
        BF_VerboseLevel GetVerboseLevel ();
        ///
        static BF_StopLevel GetStopLevel (BFMessageType type);

        ///
        static wxString GetStopLevelString (BF_StopLevel stopLevel);

        ///
        bool IsProjectModified ();

        /// run all tasks
        bool Run_Start ();
        ///
        bool Run_NextOperation ();
        ///
        bool Run_Finished ();


        ///
        Progress* GetProgressTotal ();
        ///
        ProgressWithMessage* GetProgressTask ();

        ///
        void StopCurrentOperation ();
        ///
        void StopBackup ();
        ///
        bool GetStopCurrentOperation ();
        ///
        bool GetStopBackup ();

        /** clear 'arrLastLogFiles_' */
        void ClearLastLogFiles ();

        /** insert 'strFilename' as the the project
            log file as the first element of 'arrLastLogFiles_' */
        void SetProjectLogFile (wxString& strFilename);

        /** add 'strFilename' as a task log file to 'arrLastLogFiles_' */
        void AddTaskLogFile (wxString& strFilename);

        ///
        const wxArrayString& GetLastLogFiles ();

        ///
        void InitThat (BFTaskListCtrl* pTaskList);
        ///
        void InitThat (BFBackupTree* pBackupTree);

        ///
        bool StoreToFile (const wxString& strFilename);
        ///
        bool ReadFromFile (const wxString& strFilename);

        ///
        void Reset ();

        ///
        void ModifyDestination (const wxString& strOldDestination,
                                const wxString& strNewDestination);

        /// check if all sources and backup-volumeas are there
        bool PreBackupCheck ();

        /** Searches for the crontab-line related to the currently open
            project and return the first match for it. */
        wxString GetCrontabline ();

        /** Separate each part of a crontab-line and store it as an
            element of 'arr'. The string-array will cleared at the beginning
            of this methode!
            It means that the strings for minute, hour, day-of-month, month,
            day-of-week and the command are extracted.
            If there are not enough (<5) parameters in the line, 'arr' is empty. */
        static wxArrayString& ParseCrontabline (const wxString& strLine, wxArrayString& arr);

        /** search for blackfisk-specific placeholders in 'rStr'
            and replace them with the needed value */
        static wxString& FillBlackfiskPlaceholders (wxString& rStr);
};

#endif    // BFBACKUP_H
