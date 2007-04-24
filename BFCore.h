/**
 * BFCore.h
 * definition of class BFCore
 * standardroutins for the applications
 * for example: file handling
 * 2006-02-17
 ***/

#ifndef BFCORE_H
#define BFCORE_H

#include <cstdio>
#include <vector>

#include <wx/filesys.h>
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/imaglist.h>

#include "BFLog.h"
#include "BFundef.h"


typedef std::pair<wxString, wxString>   PairString;
typedef std::vector<PairString>         MapStringPair;

/// forwarde declarations
class wxZipEntry;
class Progress;
class ProgressWithMessage;

#define BF_DEFAULT_OVERWRITE       true

///
class BFCore
{
    private:
        /** we need a static because if a backup process
            works over midnight */
        wxChar      strCurrentDate_[11];

        /** it log messages recieving from OBSystem;
            there is no need to touch OBLog directly
            just use OBSystem to create messages */
        BFLog       log_;

        /** get attributes from a file ('rFn') and set them to a ZipEntry ('pEntry')
            this methode should be to encapsulate plattformdependend code! */
        bool SetZipEntryFileAttributes (wxFileName& rFn, wxZipEntry* pEntry);

        ///
        static BFCore sCore_;

        /// constructor
        BFCore ();

    protected:
        /// proteced members

    public:
        ///
        static BFCore& Instance ();

        /// virtual destructor
        virtual ~BFCore ();

        /** move a file */
        bool MoveFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite = BF_DEFAULT_OVERWRITE);
        /** copy a file
            'pSource' can have wildcards
            'pDestination' can be a concret file or only a directory
            return false if there are one or more errors */
        bool CopyFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite = BF_DEFAULT_OVERWRITE, bool bVerify = false);

        /** synchroinze two directories */
        bool Synchronize (const wxChar* pOriginal, const wxChar* pToSynchronize, bool bVerify, ProgressWithMessage* pProgress = NULL);

        /// to patch for wxWidgets
        bool IsWriteProtected (const wxChar* pFilename);
        /// to patch for wxWidgets
        bool SetWriteProtected (const wxChar* pFilename, bool bWriteProtected);

        /// return number of files and directories in a directory and its subdirectories
        long GetDirFileCount(const wxChar* pDir, long* pDirCount = NULL, long* pFileCount = NULL);

        /** get all files and subdirectories in 'dir' and store them to 'arr'
            it is possible to store them as relative pathes
            files and directories that should not be in the listing can be specified in 'pExcludeListing' */
        wxArrayString& GetDirListing (const wxChar* dir, wxArrayString& arr, wxArrayString* pExcludeListing = NULL, bool bRelativ = false);

        /** read the attributes (like file-attributes) from a dir ('pSourceDir') and
            copy them to another dir ('pDestinationDir') */
        bool CopyDirAttributes (const wxChar* pSourceDir, const wxChar* pDestinationDir);

        /** create a directory
            there should be just one new directory to create
            look at CreatePath() to create more than one new directories */
        bool CreateDir (const wxChar* pNewDir);
        /** create a path
            there could be more than one new directories in the path */
        bool CreatePath (const wxChar* pPath);

        /** copy a directory */
        bool CopyDir (const wxChar* pSourceDir, const wxChar* pDestinationDir, bool bVerify, ProgressWithMessage* pProgress = NULL);


        /*
        bool CreateZipFromListing (const wxChar* pstrZipName, wxArrayString& rListing, ProgressWithMessage* pProgress = NULL);*/
        /** create a zipfile ('pstrZipName') from a directory ('pstrSourceDir')
            files and directories that should not be zipped can be specified in 'pExcludeListing'
            'pExcludeListing' and 'pProgress' can be NULL*/
        bool CreateZipFromDir (const wxChar* pstrZipName, const wxChar* pstrSourceDir, wxArrayString* pExcludeListing, bool bVerify, ProgressWithMessage* pProgress = NULL);

        /** */
        const wxChar* GetDateString ();

        // >>> DELETE methodes  <<<
    public:
        /** delete a directory
            if bOnlyIfEmpty = false it does not care about if the directory is empty or not
            if bOnlyIfEmpty = true it delete the directory only if there are no files or dirs in it
            if bIgnoreWriteprotection = false it does not delete files or directories with writeprotection
            if bIgnoreWriteprotection = true it remove writeprotection from all files and dirs and delete them */
        bool DeleteDir (const wxChar* pDir, bool bOnlyIfEmpty = false, bool bIgnoreWriteprotection = false);

        /** delete a file */
        bool DeleteFile (const wxChar* pFile, bool bIgnoreWriteProtection = BF_DEFAULT_OVERWRITE);

        /** delete files and directories specified by the array
            be carefull because it ignore write-protection and does not check if a directory is empty */
        bool Delete (wxArrayString& arrDelete);

        // >>> VERIFICATION methodes  <<<
    public:
        /** verify the file in this order:
            file-size, file-attributes, file-content*/
        bool VerifyFile (const wxChar* pFile1, const wxChar* pFile2);
        /** compare a pair-list (map) of files
            break on the first uncompare filepair */
        bool VerifyFiles (MapStringPair& rMap, ProgressWithMessage* pProgress = NULL);
        /** compare the zip entries by checksum (CRC) with the original files */
        bool VerifyZip (const wxChar* pZipFileName, wxArrayString& arrFiles, ProgressWithMessage* pProgress = NULL);
        /** create a CRC checksum of a file */
        wxUint32 GetFileCrc (const wxChar* pFilename);
    private:
        /// compare the file-attributes of two files
        bool VerifyFileAttributes (wxFileName& fn1, wxFileName& fn2);
        /// compare the content of to files
        bool VerifyFileContents (wxFile& f1, wxFile& f2);
};    // class BFCore

#endif    // BFCORE_H
