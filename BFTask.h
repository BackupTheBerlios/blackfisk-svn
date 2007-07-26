/**
 * Name:        BFTask.h
 * Purpose:     BFTask and BFTaskData class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-28
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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

#ifndef BFTASK_H
#define BFTASK_H

#include <vector>
#include "BFTaskBase.h"
#include "jbarchive.h"

/** define the type of the task
    it says what the tasks exactly do */
typedef wxUint8 BFTaskType;
#define TaskINVALID     0
#define TaskARCHIVE     1       // comprase backup-source to an archive
#define TaskDIRCOPY     2       // copy a complete directory
#define TaskFILECOPY    3       // copy a file
#define TaskSYNC        4       // synchronize directories

/** define the compression types
    which compression format should be used for TaskARCHIVE */
typedef wxUint8 BFArchiveFormat;
#define CompressNOTUSED 0
#define CompressZIP     1
//#define Compress7Z    2
//#define CompressTAR   3
//#define ...

///
typedef wxUint32 BFoid;
#define BFInvalidOID (BFoid)-1


/// base class for a "BFTask"; represent the data-layer
class BFTaskData : public BFTaskBase
{
    private:
        /// unique object identifier
        BFoid                       oid_;
        /// task type
        BFTaskType                  type_;
        /// source
        wxString                    strSource_;
        /// destination path
        wxString                    strDestination_;
        /// destination name
        wxString                    strName_;
        ///
        bool                        bVerify_;
        /** specify the format of the archive if
            'type_' == TaskARCHIVE
            it is CompressNOTUSED if it is not in use */
        BFArchiveFormat             archiveFormat_;
        /// files and sub-dirs to exclude from the backup
        wxArrayString               arrExclude_;

    protected:
        ///
        bool SetOID (BFoid oid);
        ///
        void SetTaskType (BFTaskType type);

    public:
        ///
        BFTaskData (BFTaskType type,
                    const wxChar* strSource,
                    const wxChar* strDestination,
                    const wxChar* strName,
                    bool bVerify,
                    BFArchiveFormat archive,
                    wxArrayString& arrExclude);

        ///
        BFTaskData ();

        ///
        virtual ~BFTaskData ();

        /** check if the object and its date in it plausibel */
        bool IsPlausible ();

        ///
        BFoid GetOID ();
        ///
        BFTaskType GetType ();
        ///
        const wxChar* GetSource ();
        ///
        const wxChar* GetDestination ();
        ///
        const wxChar* GetName ();
        ///
        bool Verify ();
        ///
        BFArchiveFormat GetArchiveFormat ();
        ///
        const wxArrayString& GetExclude ();

        ///
        void SetSource (const wxChar* source);
        ///
        void SetDestination (const wxChar* dest);
        ///
        void SetName (const wxChar* name);
        ///
        void SetVerify (bool verify);
        ///
        void SetArchiveFormat (BFArchiveFormat archive);
        ///
        void SetExclude (const wxArrayString& exclude);

};  // class BFTaskData


/// represent a BFTask in the application-layer
class BFTask : public BFTaskData
{
    private:
        ///
        bool RunForArchive (ProgressWithMessage& rProgress);
        ///
        bool RunForDirCopy (ProgressWithMessage& rProgress);
        ///
        bool RunForFileCopy(ProgressWithMessage& rProgress);
        ///
        bool RunForDirSync (ProgressWithMessage& rProgress);

        ///
        bool bStopTask_;

    public:
        /// constructor
        BFTask (BFTaskType type,
                const wxChar* strSource,
                const wxChar* strDestination,
                const wxChar* strName,
                bool bVerify,
                BFArchiveFormat archive,
                wxArrayString& arrExclude);

        /// create a dummy task with an invalid oid
        BFTask ();

        /// virtual destructor
        virtual ~BFTask ();

        /** create a new clean BFTask object
            with a oid depending on the current OBRootTask instance
            (without storeing it to the task list in the current root task)
            and return a pointer to it *
        static BFTask* CreateNew ();*/

        ///
        bool Run (ProgressWithMessage& rProgress);
        ///
        void StopTask ();

        /// return a describe of the task
        wxString GetTypeDescription ();
        /// return the iconId of the task
        int GetTypeIconId ();
        ///
        static int GetTypeIconId (BFTaskType type);
        ///
        wxString GetArchiveExtension();

        /** the object will be serialized(write) TO 'rA'
            or
            a the object will be serialized(read) from 'rA' */
        bool Serialize (jbArchive& rA);
};    // class BFTask


///
typedef std::vector<BFTask*>    BFTaskVector;

#endif    // BFTASK_H
