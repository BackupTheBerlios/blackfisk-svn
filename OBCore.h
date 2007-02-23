/**
 * OBCore.h
 * definition of class OBCore
 * standardroutins for the applications
 * for example: file handling
 * 2006-02-17
 ***/

#ifndef OBCORE_H
#define OBCORE_H

#include <cstdio>
#include <vector>

#include <wx/filesys.h>
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/imaglist.h>

#include "OBLog.h"
#include "OBundef.h"


typedef std::pair<wxString, wxString>   PairString;
typedef std::vector<PairString>         MapStringPair;

/// forwarde declarations
class wxZipEntry;
class Progress;
class ProgressWithMessage;

///
class OBCore
{
    private:
        /** we need a static because if a backup process
            works over midnight */
        wxChar      strCurrentDate_[11];

        /** it log messages recieving from OBSystem;
            there is no need to touch OBLog directly
            just use OBSystem to create messages */
        OBLog       log_;

        ///
        //void Init ();

        /** get attributes from a file ('rFn') and set them to a ZipEntry ('pEntry')
            this methode should be to encapsulate plattformdependend code! */
        bool SetZipEntryFileAttributes (wxFileName& rFn, wxZipEntry* pEntry);

        /// compare the file-attributes of two files
        bool VerifyFileAttributes (wxFileName& fn1, wxFileName& fn2);
        /// compare the content of to files
        bool VerifyFileContents (wxFile& f1, wxFile& f2);

        ///
        static OBCore sCore_;

        /// constructor
        OBCore ();

    protected:
        /// proteced members

    public:
        ///
        static OBCore& Instance ();

        /// virtual destructor
        virtual ~OBCore ();

        /** move a file */
        bool MoveFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite = OB_DEFAULT_OVERWRITE);
        /** copy a file
            'pSource' can have wildcards
            'pDestination' can be a concret file or only a directory
            return false if there are one or more errors */
        bool CopyFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite = OB_DEFAULT_OVERWRITE, bool bVerify = false);
        /** delete a file */
        bool DeleteFile (const wxChar* pFile, bool bIgnoreWriteProtection = OB_DEFAULT_OVERWRITE);

        /// to patch for wxWidgets
        bool IsWriteProtected (const wxChar* pFilename);
        /// to patch for wxWidgets
        bool SetWriteProtected (const wxChar* pFilename, bool bWriteProtected);

        /// return number of files and directories in a directory and its subdirectories
        long GetDirFileCount(const wxChar* pDir, long* pDirCount = NULL, long* pFileCount = NULL);
        /* return number of files and directories in a list of files and directories
        long GetDirFileCount(wxArrayString& rListing, long* pDirCount = NULL, long* pFileCount = NULL);*/

        /** get all files and subdirectories in 'dir' and store them to 'arr'
            files and directories that should not be in the listing can be specified in 'pExcludeListing' */
        wxArrayString& GetDirListing (const wxChar* dir, wxArrayString& arr, wxArrayString* pExcludeListing = NULL);

        /** read the attributes (like file-attributes) from a dir ('pSourceDir') and
            copy them to another dir ('pDestinationDir') */
        bool CopyDirAttributes (const wxChar* pSourceDir, const wxChar* pDestinationDir);

        /** create a directory */
        bool CreateDir (const wxChar* pNewDir);
        /** create a path
            there could be more than one new directories in the path */
        bool CreatePath (const wxChar* pPath);

        /** copy a directory */
        bool CopyDir (const wxChar* pSourceDir, const wxChar* pDestinationDir, bool bVerify, ProgressWithMessage* pProgress = NULL);

        /** delete a directory
            if bOnlyIfEmpty = false it does not care about if the directory is empty or not
            if bOnlyIfEmpty = true it delete the directory only if there are no files or dirs in it
            if bIgnoreWriteprotection = false it does not delete files or directories with writeprotection
            if bIgnoreWriteprotection = true it remove writeprotection from all files and dirs and delete them */
        bool DeleteDir (const wxChar* pDir, bool bOnlyIfEmpty = false, bool bIgnoreWriteprotection = false);

        /** */
        bool VerifyFile (const wxChar* pFile1, const wxChar* pFile2);
        /** compare a pair-list (map) of files
            break on the first uncompare filepair */
        bool VerifyFiles (MapStringPair& rMap, ProgressWithMessage* pProgress = NULL);
        /** compare the zip entries by checksum (CRC) with the original files */
        bool VerifyZip (const wxChar* pZipFileName, wxArrayString& arrFiles, ProgressWithMessage* pProgress = NULL);

        /*
        bool CreateZipFromListing (const wxChar* pstrZipName, wxArrayString& rListing, ProgressWithMessage* pProgress = NULL);*/
        /** create a zipfile ('pstrZipName') from a directory ('pstrSourceDir')
            files and directories that should not be zipped can be specified in 'pExcludeListing'
            'pExcludeListing' and 'pProgress' can be NULL*/
        bool CreateZipFromDir (const wxChar* pstrZipName, const wxChar* pstrSourceDir, wxArrayString* pExcludeListing, bool bVerify, ProgressWithMessage* pProgress = NULL);

        /** */
        const wxChar* GetDateString ();

        ///
        wxUint32 GetFileCrc (const wxChar* pFilename);

};    // class OBCore


/** create a directory-listing with empty directories included */
class OBDirListingTraverser : public wxDirTraverser
{
    private:
        ///
        wxArrayString&      rList_;
        ///
        wxArrayString*      pExcludeList_;

    public:
        /// default ctor
        OBDirListingTraverser (wxArrayString& rList, wxArrayString* pExcludeList = NULL);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

};  // class OBDirListingTraverser


///
class OBCountDirTraverser : public wxDirTraverser
{
    private:
        ///
        long lDirCount_;
        ///
        long lFileCount_;

    public:
        /// default ctor
        OBCountDirTraverser ();

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

        ///
        long GetCount ()
        { return lDirCount_ + lFileCount_; }
        ///
        long GetDirCount ()
        { return lDirCount_; }
        ///
        long GetFileCount ()
        { return lFileCount_; }
};  // class OBCountDirTraverser


///
class OBDeleteDirTraverser : public wxDirTraverser
{
    private:
        /** if bIgnoreWriteprotection = false it does not delete files or directories with writeprotection
            if bIgnoreWriteprotection = true it remove writeprotection from all files and dirs and delete them */
        bool bIgnoreWriteprotection_;
        ///
        OBCore&     core_;

    public:
        /// default ctor
        OBDeleteDirTraverser (OBCore& rCore, bool bIgnoreWriteprotection = false);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

};  // class OBDeleteDirTraverser


///
class OBCopyDirTraverser : public wxDirTraverser
{
    private:
        ///
        wxString                strDestination_;
        ///
        ProgressWithMessage*    pProgress_;
        ///
        OBCore&                 core_;
        ///
        MapStringPair*          pRememberToVerify_;

    public:
        /// default ctor
        OBCopyDirTraverser (OBCore& rCore,
                            const wxChar* pDestinationDirectory,
                            MapStringPair* pRememberToVerify = NULL,
                            ProgressWithMessage* pProgress = NULL);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

};  // class OBCopyDirTraverser


#endif    // OBCORE_H
