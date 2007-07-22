/**
 * BFRootTask.h
 * definition of class BFRootTask
 * <comment>
 * 2006-05-29
 ***/

#ifndef BFROOTTASK_H
#define BFROOTTASK_H

#include <wx/wx.h>
#include <wx/listbox.h>

#include "BFTaskBase.h"
#include "BFTask.h"
#include "BFMainFrame.h"
#include "ObserverPattern.h"
#include "BFBackupTree.h"
#include "BFProjectSettings.h"

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
        BFTaskVector& TaskVector ()
        { return vecTasks_; }

        ///
        bool Serialize (jbArchive& rA);

        /** it delete all task objects in the task-vector
            ATTENTION: it does not check if the project is saved !*/
        void ClearTaskVector ();

    public:
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

        /** run all tasks*/
        bool Run (wxWindow* pParent);
        ///
        void StopCurrentTask ();
        ///
        void StopProject ();

        ///
        void InitThat (wxListBox& rListBox);
        ///
        void InitThat (BFBackupTree& rBackupTree);

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
