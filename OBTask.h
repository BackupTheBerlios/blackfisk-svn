/**
 * OBTask.h
 * definition of class OBTask
 * <comment>
 * 2006-04-28
 ***/

#ifndef OBTASK_H
#define OBTASK_H

#include <vector>
#include "OBTaskBase.h"

/** define the type of the task
    it says what the tasks exactly do */
typedef wxUint8 OBTaskType;
#define TaskINVALID     0
#define TaskARCHIVE     1       // comprase backup-source to an archive
#define TaskDIRCOPY     2       // copy a complete directory
#define TaskFILECOPY    3       // copy a file
//#define TaskDUMMY       4

/** define the compression types
    which compression format should be used for TaskARCHIVE */
typedef wxUint8 OBArchiveFormat;
#define CompressNOTUSED 0
#define CompressZIP     1
//#define Compress7Z    2
//#define CompressTAR   3
//#define ...

///
typedef wxUint32 OBoid;
#define OBInvalidOID (OBoid)-1


/// base class for a "OBTask"; represent the data-layer
class OBTaskData : public OBTaskBase
{
    private:
        /// unique object identifier
        OBoid                       oid_;
        /// task type
        OBTaskType                  type_;
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
        OBArchiveFormat             archiveFormat_;
        /// files and sub-dirs to exclude from the backup
        wxArrayString               arrExclude_;

    protected:
        ///
        bool SetOID (OBoid oid);
        ///
        void SetTaskType (OBTaskType type);

    public:
        ///
        OBTaskData (OBTaskType type,
                    const wxChar* strSource,
                    const wxChar* strDestination,
                    const wxChar* strName,
                    bool bVerify,
                    OBArchiveFormat archive,
                    wxArrayString& arrExclude);

        ///
        OBTaskData ();

        ///
        virtual ~OBTaskData ();

        /** check if the object and its date in it plausibel */
        bool IsPlausible ();

        ///
        OBoid GetOID ();
        ///
        OBTaskType GetType ();
        ///
        const wxChar* GetSource ();
        ///
        const wxChar* GetDestination ();
        ///
        const wxChar* GetName ();
        ///
        bool Verify ();
        ///
        OBArchiveFormat GetArchiveFormat ();
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
        void SetArchiveFormat (OBArchiveFormat archive);
        ///
        void SetExclude (const wxArrayString& exclude);

};  // class OBTaskData


/// represent a OBTask in the application-layer
class OBTask : public OBTaskData
{
    private:
        ///
        bool RunForArchive (ProgressWithMessage& rProgress);
        ///
        bool RunForDirCopy (ProgressWithMessage& rProgress);
        ///
        bool RunForFileCopy(ProgressWithMessage& rProgress);

    public:
        /// constructor
        OBTask (OBTaskType type,
                const wxChar* strSource,
                const wxChar* strDestination,
                const wxChar* strName,
                bool bVerify,
                OBArchiveFormat archive,
                wxArrayString& arrExclude);

        /// create a dummy task with an invalid oid
        OBTask ();

        /// virtual destructor
        virtual ~OBTask ();

        /** create a new clean OBTask object
            with a oid depending on the current OBRootTask instance
            (without storeing it to the task list in the current root task)
            and return a pointer to it *
        static OBTask* CreateNew ();*/

        ///
        bool Run (ProgressWithMessage& rProgress);

        /// describe the task in words
        wxString GetTypeDescription ();
        ///
        wxString GetArchiveExtension();

        /** the object will be serialized(write) TO 'rA'
            or
            a the object will be serialized(read) from 'rA' */
        bool Serialize (jbArchive& rA);
};    // class OBTask


///
typedef std::vector<OBTask*>    OBTaskVector;

#endif    // OBTASK_H
