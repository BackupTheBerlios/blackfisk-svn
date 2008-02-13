/**
 * Name:        BFTask.h
 * Purpose:     BFTask and BFTaskData class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-28
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

#ifndef BFTASK_H
#define BFTASK_H

#include <vector>
#include "BFTaskBase.h"
#include "Progress.h"
#include "jbSerialize.h"

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
typedef std::vector<BFTaskType> BFTypeVector;


/// base class for a "BFTask"; represent the data-layer
class BFTaskData
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
        ///
        bool                        bVerifyContent_;
        /** specify the format of the archive if
            'type_' == TaskARCHIVE
            it is CompressNOTUSED if it is not in use */
        BFArchiveFormat             archiveFormat_;
        /// files and sub-dirs to exclude from the backup
        wxArrayString               arrExclude_;

    protected:
        ///
        bool SetOID (BFoid oid);

    public:
        ///
        BFTaskData (BFTaskType type,
                    const wxChar* strSource,
                    const wxChar* strDestination,
                    const wxChar* strName,
                    bool bVerify,
                    bool bVerifyContent,
                    BFArchiveFormat archive,
                    wxArrayString& arrExclude);

        ///
        BFTaskData ();

        ///
        virtual ~BFTaskData ();

        /** check if the object and its data in it is valid
            but it doesn't check if the directories are realy there
            it just validate it to store it without creating inconsistence
            with other data */
        bool IsValid ();

        /** add all available task types to the vector
            with the current task type as first item */
        void GetAvailableTypes (BFTypeVector& rVecType);

        ///
        BFoid GetOID () const;
        ///
        BFTaskType GetType () const;
        ///
        const wxChar* GetSource () const;
        ///
        const wxChar* GetDestination () const;
        ///
        const wxChar* GetName () const;
        ///
        bool Verify () const;
        ///
        bool VerifyContent () const;
        ///
        BFArchiveFormat GetArchiveFormat () const;
        ///
        const wxArrayString& GetExclude () const;

        ///
        void SetTaskType (BFTaskType type);
        ///
        void SetSource (const wxChar* source);
        ///
        void SetDestination (const wxChar* dest);
        ///
        void SetName (const wxChar* name);
        ///
        void SetVerify (bool verify);
        ///
        void SetVerifyContent (bool verify_content);
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
                bool bVerifyContent,
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
        ///
        static wxString GetTypeDescription (BFTaskType type, BFArchiveFormat format = CompressNOTUSED);
        /// return the iconId of the task
        int GetTypeIconId ();
        ///
        static int GetTypeIconId (BFTaskType type);
        ///
        wxString GetArchiveExtension();
        ///
        static wxString GetArchiveExtension(BFArchiveFormat format);

        /** the object will be serialized(write) TO 'rA'
            or
            a the object will be serialized(read) from 'rA' */
        bool Serialize (jbSerialize& rA);

        /** search for blackfisk-specific placeholders in 'rStr'
            and replace them with the needed value */
        static wxString& FillBlackfiskPlaceholders (wxString& rStr);
};    // class BFTask


#endif    // BFTASK_H
