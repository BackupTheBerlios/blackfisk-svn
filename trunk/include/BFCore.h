/**
 * Name:        BFCore.h
 * Purpose:     BFCore class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-02-17
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

#include "BFundef.h"


typedef std::pair<wxString, wxString>   PairString;
typedef std::vector<PairString>         MapStringPair;

/// forwarde declarations
class wxZipEntry;
class Progress;
class ProgressWithMessage;

#define BF_DEFAULT_OVERWRITE        true
#define BF_VERIFY_CONTENT_DEFAULT   false

/** implement standardroutins for the application
 for example: file handling */
class BFCore
{
    private:
        /** we need a static because if a backup process
            works over midnight */
        wxString    strCurrentDate_;
        /** we need a static because if a backup process
            works over midnight */
        wxString    strCurrentTime_;

        /// Please see SetCurrentDateTime() for more detailes.
        wxString    strCurrentDate_Old_;
        /// Please see SetCurrentDateTime() for more detailes.
        wxString    strCurrentTime_Old_;

        /** indicates that a backup is running
            so that backup messages will be created */
        static bool bWhileBackup_;

        /** get attributes from a file ('rFn') and set them to a ZipEntry ('pEntry')
            this methode should be to encapsulate plattformdependend code! */
        bool SetZipEntryFileAttributes (wxFileName& rFn, wxZipEntry* pEntry);

        /// set 'strCurrentDate_' and 'strCurrentTime_'
        void SetCurrentDateTime ();

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

        ///
        void BackupStarted ();
        ///
        void BackupEnded ();
        ///
        bool IsWhileBackup ();
        /** if bWhileBackup_ == true and the current running task or project
            should be stoped because of an error it returns true */
        static bool IsStop ();

        /** Open a file specified by 'strFilename', search for a line specified by 'strOld'
            and replace it with a line specified by 'strNew'.
            It return 'true' if a replace was done and 'false' if no replace was done
            or an error occured. */
        bool ReplaceLineInFile (const wxString& strFilename,
                                const wxString& strOld,
                                const wxString& strNew);

        /** Open a file specified by 'strFilename', search for a line specified by 'strLine'
            and delete the line.
            It return 'true' if a the line was found and deleted and 'false' if the
            line wasn't found or an error occured. */
        bool DeleteLineInFile (const wxString& strFilename,
                               const wxString& strLine);

        /** Open a file specified by 'strFilename', search for a line specified by 'strLine'.
            It return 'true' if a the line was found and 'false' if the
            line wasn't found or an error occured. */
        bool IsLineInFile (const wxString& strFilename,
                           const wxString& strLine);

        /** move a file */
        bool MoveFile (const wxString &strSource, const wxString& Destination, bool bOverwrite = BF_DEFAULT_OVERWRITE);
        /** copy a file
            'pSource' can have placeholders
            'pDestination' can be a concret file or only a directory
            return false if there are one or more errors */
        static bool CopyFile (const wxString& Source,
                              const wxString& Destination,
                              bool bOverwrite = BF_DEFAULT_OVERWRITE,
                              bool bVerify = false,
                              bool bVerifyContent = BF_VERIFY_CONTENT_DEFAULT);

        /* synchroinze two directories *
        bool Synchronise (const wxString& strOriginal,
                          const wxString& strToSynchronise,
                          bool bVerify,
                          bool bVerifyContent,
                          bool bRealSync = true,
                          ProgressWithMessage* pProgress = NULL);*/

        ///
        static bool HasFileAttribute_ReadOnly (const wxString& strFilename);
        ///
        static bool HasFileAttribute_Archive (const wxString& strFilename);
        ///
        static bool HasFileAttribute_Hidden (const wxString& strFilename);

        ///
        static bool SetFileAttribute_ReadOnly (const wxString& strFilename, bool bReadOnly);
        ///
        static bool SetFileAttribute_Archive (const wxString& strFilename, bool bArchive);
        ///
        static bool SetFileAttribute_Hidden (const wxString& strFilename, bool bHidden);

        /// return number of files and directories in a directory and its subdirectories (return -1 on error)
        long GetDirFileCount(const wxString& Dir, long* pDirCount = NULL, long* pFileCount = NULL);

        /** get all files and subdirectories in 'dir' and store them to 'arr'
            it is possible to store them as relative pathes
            files and directories that should not be in the listing can be specified in 'pExcludeListing' */
        static wxArrayString& GetDirListing (const wxString& strDir,
                                             wxArrayString& arr,
                                             wxArrayString* pExcludeListing = NULL,
                                             bool bRelativ = false);

        /** Check each entry in 'arrListing' if it is a file or a diretorie
            and store it in 'arrResultDirectories' or 'arrResultFiles'.
            The result arrays are not cleared before. */
        static void SeparateListingInDirectoriesAndFiles (wxArrayString& arrListing,
                                                   wxArrayString& arrResultDirectories,
                                                   wxArrayString& arrResultFiles);

        /** read the attributes (like file-attributes) from a dir ('pSourceDir') and
            copy them to another dir ('pDestinationDir') */
        static bool CopyDirAttributes (const wxString& strSourceDir,
                                       const wxString& strDestinationDir);

        /** create a directory
            there should be just one new directory to create
            look at CreatePath() to create more than one new directories */
        static bool CreateDir (const wxString& strNewDir);
        /** create a path
            there could be more than one new directories in the path */
        static bool CreatePath (const wxString& strPath);

        /* copy a directory *
        bool CopyDir (const wxString& strSourceDir,
                      const wxString& strDestinationDir,
                      bool bVerify,
                      bool bVerifyContent = BF_VERIFY_CONTENT_DEFAULT,
                      ProgressWithMessage* pProgress = NULL);*/


        /** create a zipfile ('pstrZipName') from a directory ('pstrSourceDir')
            files and directories that should not be zipped can be specified in 'pExcludeListing'
            'pExcludeListing' and 'pProgress' can be NULL*
        bool CreateZipFromDir (const wxString& strZipName,
                               const wxString& strSourceDir,
                               wxArrayString* pExcludeListing,
                               bool bVerify,
                               ProgressWithMessage* pProgress = NULL);*/

		/** Create a ZIP-archive ('strArchiveName'). The entries are in 'arrEntries' and can be
			files and directories.
			If 'pProgress' is specified there would be a message and a increment operation
			for each entry in 'arrEntries'.
			The range of 'pProgress' is not modified.
			There is no verify for the created archive. Use VerifyZip() for that. */
        bool CreateArchive (const wxString&			strArchiveName,
							const wxArrayString&	arrEntries,
                            ProgressWithMessage*	pProgress = NULL);
        ///
        const wxString& GetDateString ();
        ///
        const wxString& GetTimeString ();
        ///
        const wxString& GetDateString_Old ();
        ///
        const wxString& GetTimeString_Old ();


        // --- DELETE methodes ---
    public:
        /** delete a directory
            if bOnlyIfEmpty = false it does not care about if the directory is empty or not
            if bOnlyIfEmpty = true it delete the directory only if there are no files or dirs in it
            if bIgnoreWriteprotection = false it does not delete files or directories with writeprotection
            if bIgnoreWriteprotection = true it remove writeprotection from all files and dirs and delete them */
        static bool DeleteDir (const wxString& strDir, bool bOnlyIfEmpty = false, bool bIgnoreWriteprotection = false);

        /** delete a file */
        static bool DeleteFile (const wxString& strFile, bool bIgnoreWriteProtection = BF_DEFAULT_OVERWRITE);

        /** delete files and directories specified by the array */
        static bool Delete (wxArrayString& arrDelete, bool bOnlyIfEmpty = false, bool bIgnoreWriteprotection = false);

        /** add all subdirectories of 'strDir' to 'arr' including subdirectories
            in all levels. the point is it add the deepest (last) subdirectory
            to 'arr' before the parent of the subdirectory
            e.g. X:\dir\sub1\sub1a\sub1a_
                 X:\dir\sub1\sub1a\
                 X:\dir\sub2\sub2x
                 X:\dir\sub2\
            it searches for hidden directories, too
            it is used by DeleteDir() */
        static wxArrayString& GetSubDirectories (const wxString& strDir, wxArrayString& arr);

        // >>> VERIFICATION methodes  <<<
    public:
        /** verify the file in this order:
            file-size, file-attributes, file-content*/
        static bool VerifyFile (const wxString& strFile1, const wxString& strFile2, bool bVerifyContent = BF_VERIFY_CONTENT_DEFAULT);

        /** compare a pair-list (map) of files and break on the first uncompare filepair
            if 'pProgress' is used it call IncrementActualWithMessage() on it with each compared file
            it only increment the progress but don't set the range on it
            if 'pProgress' IsLocked() it only call SetMessage() on it and don't IncrementActual()
            for more detailes about that look an how a sync-task handle its progress object
            sometimes a sync-task need to call BFCore::CopyDir() */
        bool VerifyFiles (MapStringPair& rMap, ProgressWithMessage* pProgress = NULL, bool bVerifyContent = BF_VERIFY_CONTENT_DEFAULT);

        /** compare the zip entries by checksum (CRC) with the original files */
        bool VerifyZip (const wxString& strZipFileName, wxArrayString& arrFiles, ProgressWithMessage* pProgress = NULL);

        /** create a CRC checksum of a file */
        wxUint32 GetFileCrc (const wxString& strFilename);

        /** Compare the some (NOT ALL!!!) file-attributes of two files.
            See the methode code for more details. */
        static bool VerifyFileAttributes (wxFileName& fn1, wxFileName& fn2);
        /** A wrapper for VerifyFileAttributes(wxFileName&, wxFileName&).
            See more detailes there. */
        static bool VerifyFileAttributes (wxString& strFile1, wxString& strFile2);

        /// compare the content of to files
        static bool VerifyFileContents (wxFile& f1, wxFile& f2);
};    // class BFCore

#endif    // BFCORE_H
