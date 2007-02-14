/**
 * OBRootTask.h
 * definition of class OBRootTask
 * <comment>
 * 2006-05-29
 ***/

#ifndef OBROOTTASK_H
#define OBROOTTASK_H

#include <wx/wx.h>
#include <wx/listbox.h>

#include "OBTaskBase.h"
#include "OBTask.h"
#include "OBMainFrame.h"
#include "ObserverPattern.h"
#include "OBBackupTree.h"

/// data layer
class OBRootTaskData : public OBTaskBase, public Subject
{
    private:
        ///
        OBRootTaskData (OBRootTaskData&);

        /// name of the RootTask; only for identification by the user
        wxString            strName_;
        /// vector with all backup-tasks
        OBTaskVector        vecTasks_;

        /** indicates that the object or child of it has
            data that was not stored this time */
        bool                bModified_;

    protected:
        ///
        OBRootTaskData ();
        ///
        virtual ~OBRootTaskData ();

        ///
        OBTaskVector& TaskVector ()
        { return vecTasks_; }

        ///
        bool Serialize (jbArchive& rA);

    public:
        ///
        long GetTaskCount ();

        ///
        void SetName (const wxChar* strName);
        ///
        const wxChar* GetName ();

        /** create a new task and return its oid
            if an error occures it returns OBInvalidOID
            this methode does not broadcast the observers! */
        OBoid AppendTask (OBTaskType type,
                          const wxChar* strSource,
                          const wxChar* strDestination,
                          const wxChar* strName,
                          bool bVerify,
                          wxArrayString* pArrExclude = NULL,
                          OBArchiveFormat archive = CompressNOTUSED);
        /** add an existing task to root and return its oid
            if the object is not plausible or just exists in root
            OBInvalidOID returns */
        OBoid AppendTask (OBTask& rTask);

        ///
        bool HasTask(OBoid oid);
        ///
        OBTask* GetTask(OBoid oid);

        ///
        bool IsModified ();
        ///
        void SetModified (bool bModified = true);
        ///
        bool StoreToFile (const wxChar* strFilename);
        ///
        bool ReadFromFile (const wxChar* strFilename);
};  // class OBRootTaskData

/// application layer
class OBRootTask : public OBRootTaskData
{
    private:
        ///
        Progress*           pTotalProgress_;
        ///
        Progress*           pTaskProgress_;

        ///
        OBRootTask ();
        ///
        OBRootTask (OBRootTask&);

        /// the last created oid
        OBoid               oidLast_;

        ///
        static OBRootTask sRootTask_;

    protected:

    public:
        ///
        static OBRootTask& Instance ();
        /// virtual dtor
        virtual ~OBRootTask();

        /// create unique oid's for
        OBoid CreateOID ();

        ///
        OBCore& Core ();

        /** run all tasks*/
        bool Run (wxWindow* pParent);

        ///
        void InitThat (wxListBox& rListBox);
        ///
        void InitThat (OBBackupTree& rBackupTree);
};  // class OBRootTask

#endif    // OBROOTTASK_H
