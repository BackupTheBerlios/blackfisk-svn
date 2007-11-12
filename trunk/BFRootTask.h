/**
 * Name:        BFRootTask.h
 * Purpose:     BFRootTask and BFRootTaskData class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
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

#ifndef BFROOTTASK_H
#define BFROOTTASK_H

#include <wx/wx.h>
#include <wx/listbox.h>

#include "BFTaskBase.h"
#include "BFTask.h"
#include "ObserverPattern.h"
//#include "BFBackupTree.h"
#include "BFProjectSettings.h"

class BFBackupLog;
class ProgressTotal;
class ProgressWithMessage;

/// data layer
class BFRootTaskData : public BFTaskBase, public Subject
{
    private:
        ///
        BFRootTaskData (BFRootTaskData&);

        /// name of the RootTask; only for identification by the user
        wxString            strName_;
        /// vector with all backup-tasks
        BFTaskVector        vecTasks_;
        ///
        BFProjectSettings   projectSettings_;

        /** indicates that the object or child of it has
            data that was not stored this time */
        bool                bModified_;

    protected:
        ///
        BFRootTaskData ();
        ///
        virtual ~BFRootTaskData ();

        ///
        bool Serialize (jbSerialize& rA);

        /** it delete all task objects in the task-vector
            ATTENTION: it does not check if the project is saved !*/
        void ClearTaskVector ();

        /** return the position in the task-vector of the specified BFTask
            return -1 if nothing is found */
        long FindTask (BFTask* pTask);

    public:
        ///
        BFTaskVector& TaskVector ()
        { return vecTasks_; }

        ///
        bool Has (BFProjectSettings* pPrjSet);

        ///
        long GetTaskCount ();

        ///
        void SetName (const wxChar* strName);
        ///
        const wxChar* GetName ();
        ///
        BFProjectSettings& GetSettings ();

        ///
        void ModifyDestination (const wxString& strOldDestination, const wxString& strNewDestination);

        /** create a new task and return its oid
            if an error occures it returns OBInvalidOID
            this methode does not broadcast the observers! */
        BFoid AppendTask (BFTaskType type,
                          const wxChar* strSource,
                          const wxChar* strDestination,
                          const wxChar* strName,
                          bool bVerify,
                          wxArrayString* pArrExclude = NULL,
                          BFArchiveFormat archive = CompressNOTUSED);
        /** add an existing task to root and return its oid
            if the object is not plausible or just exists in root
            OBInvalidOID returns */
        BFoid AppendTask (BFTask& rTask);
        ///
        bool HasTask(BFoid oid);
        ///
        BFTask* GetTask(BFoid oid);
        ///
        bool DeleteTask (BFoid oid);

        /** search in 'vecTasks_' for 'pTask'
            and return the BFTask pointer behind it
            if 'pTask' is not found in the vector
            or 'pTask' is the last element
            the first BFTask pointer is returned */
        BFTask* GetNextTask (BFTask* pTask);
        ///
        BFTask* GetLastTask ();

        ///
        bool IsModified ();
        ///
        void SetModified (bool bModified = true);
};  // class BFRootTaskData

/// application layer
class BFRootTask : public BFRootTaskData
{
    private:
        /** array of log files from the last backup process
            start with the project log */
        wxArrayString       arrLastLogFiles_;
        /** the complete filename of the current open project
            if nothing is open it is empty */
        wxString            strCurrentFilename_;

        ///
        BFRootTask ();
        ///
        BFRootTask (BFRootTask&);

        /// the last created oid
        BFoid               oidLast_;
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

        ///
        static BFRootTask sRootTask_;

    public:
        ///
        static BFRootTask& Instance ();
        /// virtual dtor
        virtual ~BFRootTask();

        /// create unique oid's for
        BFoid CreateOID ();

        /// return the destinations of each task as a string array
        wxArrayString GetDestinations ();

        ///
        const wxString& GetCurrentFilename ();

        ///
        BFCore& Core ();

        ///
        void Close ();

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

        // void InitThat (wxListBox& rListBox);
        // void InitThat (BFBackupTree& rBackupTree);

        ///
        bool StoreToFile (const wxChar* strFilename);
        ///
        bool ReadFromFile (const wxChar* strFilename);

        /** clear 'arrLastLogFiles_' */
        void ClearLastLogFiles ();
        /** insert 'strFilename' as the the project
            log file as the first element of 'arrLastLogFiles_' */
        void SetProjectLogFile (wxString& strFilename);
        /** add 'strFilename' as a task log file to 'arrLastLogFiles_' */
        void AddTaskLogFile (wxString& strFilename);
        ///
        const wxArrayString& GetLastLogFiles ();
};  // class BFRootTask

#endif    // BFROOTTASK_H
