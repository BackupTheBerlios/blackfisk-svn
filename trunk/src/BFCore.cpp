/**
 * Name:        BFCore.cpp
 * Purpose:     BFCore class implementation
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

#include "BFCore.h"

#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/tokenzr.h>
#include <wx/ptr_scpd.h>
#include <wx/textfile.h>
#include <wx/file.h>

#include <../src/zlib/zlib.h>

#include "BFCore_traverser.h"
#include "BFApp.h"
#include "BFBackup.h"
#include "Progress.h"
#include "BFSystem.h"
#include "blackfisk.h"
#include "BFundef.h"


wxDEFINE_SCOPED_PTR_TYPE(wxZipEntry);

/*static*/ bool BFCore::bWhileBackup_ = false;

BFCore BFCore::sCore_;




BFCore& BFCore::Instance ()
{
    return sCore_;
}


BFCore::BFCore ()
{
    bWhileBackup_ = false;
    SetCurrentDateTime();
}


/*virtual*/ BFCore::~BFCore ()
{
}

void BFCore::SetCurrentDateTime ()
{
    // remember the current set strings
    strCurrentDate_Old_ = strCurrentDate_;
    strCurrentTime_Old_ = strCurrentTime_;

    // remember current date in a string
    strCurrentDate_ = wxDateTime::Now().FormatISODate();

    // remember current time in a string
    strCurrentTime_ = wxDateTime::Now().FormatISOTime();
    strCurrentTime_.Replace(":", "h", false);
    strCurrentTime_.Replace(":", "m", false);
    strCurrentTime_ << "s";
}

void BFCore::BackupStarted ()
{
    bWhileBackup_ = true;

    SetCurrentDateTime();
}

void BFCore::BackupEnded ()
{
    bWhileBackup_ = false;
}

bool BFCore::IsWhileBackup ()
{
    return bWhileBackup_;
}

/*static*/ bool BFCore::IsStop ()
{
    if (Instance().IsWhileBackup())
        if ( BFBackup::Instance().GetStopCurrentOperation() == true
          || BFBackup::Instance().GetStopBackup() == true)
          return true;

    return false;
}


bool BFCore::ReplaceLineInFile (const wxString& strFilename,
                                const wxString& strOld,
                                const wxString& strNew,
                                bool bAppendIfNotFound /*= false*/)
{
    wxTextFile file(strFilename);
    bool rc = false;

    // open the old file
    if ( file.Open(strFilename) == false )
        return rc;

    if ( strOld.IsEmpty() )
    {
        // add the new line
        file.AddLine(strNew);
    }
    else
    {
        // iterate on old file
        for (wxString str = file.GetFirstLine();
             !(file.Eof());
             str = file.GetNextLine())
        {
            if (str == strOld)
            {
                file.RemoveLine(file.GetCurrentLine());
                file.InsertLine(strNew, file.GetCurrentLine());
                rc = true;
            }
        }

        // there is nothing found to replace
        if ( rc == false && bAppendIfNotFound == true)
        {
            file.AddLine(strNew);
            rc = true;
        }
    }

    file.Write();
    file.Close();

    return rc;
}


bool BFCore::DeleteLineInFile (const wxString& strFilename,
                               const wxString& strLine)
{
    wxTextFile file(strFilename);
    bool rc = false;

    // open the file
    if ( file.Open(strFilename) == false )
        return rc;

    // iterate on file
    for (wxString str = file.GetFirstLine();
         !(file.Eof());
         str = file.GetNextLine())
    {
        if (str == strLine)
        {
            file.RemoveLine(file.GetCurrentLine());
            rc = true;
            break;
        }
    }

    file.Write();
    file.Close();

    return rc;
}

bool BFCore::IsLineInFile (const wxString& strFilename,
                           const wxString& strLine)
{
    wxTextFile file(strFilename);
    bool rc = false;

    // open the file
    if ( file.Open(strFilename) == false )
        return rc;

    // iterate on file
    for (wxString str = file.GetFirstLine();
         !(file.Eof());
         str = file.GetNextLine())
    {
        if (str == strLine)
        {
            rc = true;
            break;
        }
    }

    file.Close();

    return rc;
}

/*static*/ wxArrayString& BFCore::GetDirListing (const wxString& strDir,
                                                 wxArrayString& arr,
                                                 wxArrayString* pExcludeListing /*= NULL*/,
                                                 bool bRelativ /*= false*/)
{
    wxDir                   dirSource(strDir);
    wxString                strStartDir;

    if (bRelativ)
        strStartDir = strDir;

    BFDirListingTraverser trav(arr, strStartDir, pExcludeListing);

    dirSource.Traverse(trav);

    return arr;
}


/*static*/ void BFCore::SeparateListingInDirectoriesAndFiles (wxArrayString& arrListing,
                                                   wxArrayString& arrResultDirectories,
                                                   wxArrayString& arrResultFiles)
{
    for (size_t i = 0; i < arrListing.GetCount(); i++)
    {
        // is directory?
        if ( wxDirExists(arrListing[i]) )
            arrResultDirectories.Add(arrListing[i]);
        else
        // or a file?
            arrResultFiles.Add(arrListing[i]);
    }
}


bool BFCore::SetZipEntryFileAttributes (wxFileName& rFn, wxZipEntry* pEntry)
{
    if (pEntry == NULL)
        return false;

    // read file attributes
    wxUint32    wxAttr  = pEntry->GetExternalAttributes();
    // PPP
    long        lAttr   = GetFileAttributes(rFn.GetFullPath());

    // process READ-ONLY
    if (lAttr & FILE_ATTRIBUTE_READONLY)
        wxAttr = wxAttr | wxZIP_A_RDONLY;
    else
        wxAttr = wxAttr & ~wxZIP_A_RDONLY;

    // process ARCHIVE
    if (lAttr & FILE_ATTRIBUTE_ARCHIVE)
        wxAttr = wxAttr | wxZIP_A_ARCH;
    else
        wxAttr = wxAttr & ~wxZIP_A_ARCH;

    // process HIDDEN
    if (lAttr & FILE_ATTRIBUTE_HIDDEN)
        wxAttr = wxAttr | wxZIP_A_HIDDEN;
    else
        wxAttr = wxAttr & ~wxZIP_A_HIDDEN;

    // process SYSTEM
    if (lAttr & FILE_ATTRIBUTE_SYSTEM)
        wxAttr = wxAttr | wxZIP_A_SYSTEM;
    else
        wxAttr = wxAttr & ~wxZIP_A_SYSTEM;

    // set the attributes
    pEntry->SetExternalAttributes(wxAttr);

    return true;
}

/*static*/ bool BFCore::VerifyFileAttributes (wxFileName& fn1,
                                   wxFileName& fn2)
{
    /* handle only this attributes
       FILE_ATTRIBUTE_READONLY
       FILE_ATTRIBUTE_HIDDEN
       FILE_ATTRIBUTE_SYSTEM */

    // get attributes
    long lAttr1 = GetFileAttributes(fn1.GetFullPath());
    long lAttr2 = GetFileAttributes(fn2.GetFullPath());

    // FILE_ATTRIBUTE_READONLY
    if ( (lAttr1 & FILE_ATTRIBUTE_READONLY) != (lAttr2 & FILE_ATTRIBUTE_READONLY) )
        return false;

    // FILE_ATTRIBUTE_HIDDEN
    if ( (lAttr1 & FILE_ATTRIBUTE_HIDDEN) != (lAttr2 & FILE_ATTRIBUTE_HIDDEN) )
        return false;

    // FILE_ATTRIBUTE_SYSTEM
    if ( (lAttr1 & FILE_ATTRIBUTE_SYSTEM) != (lAttr2 & FILE_ATTRIBUTE_SYSTEM) )
        return false;

    return true;
}

/*static*/ bool BFCore::VerifyFileAttributes (wxString& strFile1,
                                   wxString& strFile2)
{
    wxFileName fn1(strFile1);
    wxFileName fn2(strFile2);

    return VerifyFileAttributes (fn1, fn2);
}


bool BFCore::CreateArchive (const wxString&			strArchiveName,
							const wxArrayString&	arrEntries,
							ProgressWithMessage*	pProgress /*= NULL*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // backup message
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("Create archive %s..."), strArchiveName));

	// get source dir
	wxString strSource = BFApp::ExtractCommunity(arrEntries);

    // init zip streams
    wxFFileOutputStream out(strArchiveName);
    wxZipOutputStream   zip(out);
    wxFFileInputStream* pIn             = NULL;
    wxZipEntry*         pEntry          = NULL;
    wxFileName          fileName;
    long                lLen            = wxString(strSource).Length();

    // check if streams are ready
    if ( !(out.Ok()) || !(zip.IsOk()) )
        return false;

    // iterate on files and dirs
    for (size_t i = 0; i < arrEntries.GetCount(); ++i)
    {
        // stop ?
        if ( BFCore::IsStop() )
            return false;

        fileName.Assign(arrEntries[i]);

		// backup message
		if (bWhileBackup_)
			BFSystem::Backup(wxString::Format(_("Compress %s..."), fileName.GetFullPath()));
		
        // is it a file ?
        if ( fileName.FileExists() )
        {   // create zip-entry for a file

            pEntry = new wxZipEntry(fileName.GetFullPath().Mid(lLen), fileName.GetModificationTime());
            pIn = new wxFFileInputStream(fileName.GetFullPath(), "rb");
            // set file attributes for this entry
            SetZipEntryFileAttributes(fileName, pEntry);
            zip.PutNextEntry(pEntry);
            // write it in the zip-file
            zip.Write(*pIn);
        }
        else
        {
            // is a directory
            if ( fileName.DirExists() )
            {
                wxDir dir(arrEntries[i]);

                // if it is an empty directory create it explicit as a zip-entry
                if ( !(dir.HasFiles() || dir.HasSubDirs()) )
                    zip.PutNextDirEntry(fileName.GetFullPath().Mid(lLen)); // TODO , fileName.GetModificationTime());
            }
            else
            {
                BFSystem::Fatal(wxString::Format(_("no file AND no dir: %s"), arrEntries[i]), "BFCore::CreateArchive()");

                if (pProgress != NULL)
                    pProgress->DoEnd();

                return false;
            }
        }

        // progress
        if (pProgress != NULL)
            pProgress->IncrementActualWithMessage(arrEntries[i]);

        if (pIn != NULL)
        {
            delete pIn;
            pIn = NULL;
        }
    }   // for()

    // close the zip-file
    zip.Close();
    out.Close();

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("%s compressed to %s"), strSource, strArchiveName));

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    return true;
}


bool BFCore::VerifyZip (const wxString& strZipFileName, wxArrayString& arrFiles, ProgressWithMessage* pProgress /*= NULL*/)
{
    // init
    wxZipEntryPtr       entry;    // 'smart pointer'
    wxFFileInputStream  in(strZipFileName);
    wxZipInputStream    zip(in);
    int                 i = 0;

    while ( entry.reset(zip.GetNextEntry()), entry.get() != NULL )
    {
        // stop ?
        if ( BFCore::IsStop() )
            return true;

        // we do not need to check directory-entries
        if (entry->IsDir())
        {
            // progress
            if (pProgress != NULL)
                pProgress->IncrementActual();

            continue;
        }

        // check that the zipentry corospond to the filelist-entry
        if ( arrFiles[i].EndsWith(entry->GetName(), NULL) )
        {
            // progress
            if (pProgress != NULL)
                pProgress->IncrementActualWithMessage(wxString::Format(_("verify checksum for %s"), entry->GetName()));

            // compare crc's
            if ( entry->GetCrc() != GetFileCrc(arrFiles[i]) )
            {
                BFSystem::Error(wxString::Format(_("zip-file %s is corrupt\nbecause of the entry %s\n(crc compared with %s)"),
                                                 strZipFileName,
                                                 entry->GetName(),
                                                 arrFiles[i]),
                                "BFCore::VerifyZip()");

                return false;
            }
        }
        else
        {
            // progress
            if (pProgress != NULL)
                pProgress->IncrementActual();
        }

        ++i;
    }   // while

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("%s verifyied"), strZipFileName));

    return true;
}


wxUint32 BFCore::GetFileCrc (const wxString& strFilename)
{
    // init file
    wxFile file(strFilename);
    if ( !(file.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("could not open file %s in BFCore::GetFileCrc"), strFilename), "BFCore::GetFileCrc()");
        return 0;
    }

    // read data
    size_t bufsize = file.Length();
    Bytef* pBuffer = new Bytef[bufsize];
    file.Read(pBuffer, bufsize);

    // get crc for data
    wxUint32 rc = crc32(0, pBuffer, bufsize);

	delete pBuffer;

	return rc;
    // DEBUG
    // for portability look at boost::crc
}


bool BFCore::MoveFile (const wxString& strSource,
                       const wxString& strDestination,
                       bool bOverwrite /*= DEFAULT_OVERWRITE*/)
{
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("move %s to %s"), strSource, strDestination));

    return ::wxRenameFile(strSource, strDestination, bOverwrite);
}


/*static*/ bool BFCore::CopyFile (const wxString& strSource,
                       const wxString& strDestination,
                       bool bOverwrite /*= DEFAULT_OVERWRITE*/,
                       bool bVerify /*= false*/,
                       bool bVerifyContent /*= BF_VERIFY_CONTENT_DEFAULT*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    wxString        strDest(strDestination);
    wxString        strSrc(strSource);
    wxArrayString   arrSource;
    bool            rc = true;

    // check source filename for placeholders
    if ( strSrc.Find('*') != -1 )
    {
        wxDir::GetAllFiles( strSrc.BeforeLast(wxFILE_SEP_PATH),
                            &arrSource,
                            strSrc.AfterLast(wxFILE_SEP_PATH) );
    }

    // check if destination is only a dir
    if ( ::wxDirExists(strDest) )
    {
        // append path seperator
        if ( strDest.Last() != wxFILE_SEP_PATH )
            strDest += wxFILE_SEP_PATH;

        // complete destination with fullname
        if ( arrSource.Count() == 0 )
            strDest += strSrc.AfterLast(wxFILE_SEP_PATH);
    }
    else
    {
        if ( arrSource.Count() > 0 )
        {
            BFSystem::Error(wxString::Format(_("can not copy %s\nto %s"),
                                              strSource,
                                              strDestination),
                            "BFCore::CopyFile");
            return false;
        }
    }

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // backup message
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("Copy %s to %s..."), strSource, strDestination));

    if ( arrSource.Count() == 0 )
    {   // one file to copy
        rc = ::wxCopyFile(strSrc, strDest, bOverwrite);

        // stop ?
        if ( BFCore::IsStop() )
            return false;

        // verify the file
        if (bVerify && rc)
            rc = VerifyFile(strSrc, strDest, bVerifyContent);
    }
    else
    {   // more then one files to copy because of placeholders
        for (size_t i = 0; i < arrSource.Count(); ++i)
        {
            // stop ?
            if ( BFCore::IsStop() )
                return false;

            if ( !(::wxCopyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH), bOverwrite)) )
                rc = false;
        }

        // verify the files
        if (bVerify && rc)
            for (size_t i = 0; i < arrSource.Count(); ++i)
            {
                // stop ?
                if ( BFCore::IsStop() )
                    return true;

                if ( !(VerifyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH), bVerifyContent)) )
                    rc = false;
            }
    }

    return rc;
}


/*static*/ bool BFCore::DeleteFile (const wxString& strFile, bool bIgnoreWriteProtection /*= DEFAULT_OVERWRITE*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    if ( HasFileAttribute_ReadOnly(strFile) )
    {
        if (bIgnoreWriteProtection)
        {
            SetFileAttribute_ReadOnly(strFile, false);
        }
        else
        {
            BFSystem::Error(wxString::Format(_("file %s is write-protected"), strFile), "BFCore::DeleteFile()");
            return false;
        }
    }

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("delete %s"), strFile));

    return ::wxRemoveFile(strFile);
}

/*static*/ wxArrayString& BFCore::GetSubDirectories (const wxString& strDir, wxArrayString& arr)
{
    // check parameters
    if (!(wxDir::Exists(strDir)) )
    {
        BFSystem::Fatal(_("wrong parameters"), "BFCore::GetSubDirectories()");
        return arr;
    }

    // path separator
    wxString strUsedDir = strDir;
    if ( !(strUsedDir.EndsWith(wxFILE_SEP_PATH)) )
        strUsedDir = strUsedDir + wxFILE_SEP_PATH;

    // init
    wxDir dir(strUsedDir);

    // check if open
    if ( !(dir.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("directory %s is not open"), strUsedDir), "BFCore::GetSubDirectories()");
        return arr;
    }

    // iterate
    wxString        strCurrent;

    for (bool bContinue = dir.GetFirst(&strCurrent, wxEmptyString, wxDIR_DIRS | wxDIR_HIDDEN);
         bContinue;
         bContinue = bContinue && dir.GetNext(&strCurrent))
    {
        const wxString strFullDir = strUsedDir + strCurrent;

        GetSubDirectories(strFullDir, arr).Add(strFullDir);
    }

    return arr;
}

/*static*/ bool BFCore::DeleteDir (const wxString& strDir,
                        bool bOnlyIfEmpty /*= false*/,
                        bool bIgnoreWriteprotection /*= false*/)
{
    // check parameters
    if (!(wxDir::Exists(strDir)) )
    {
        BFSystem::Fatal
        (
            wxString::Format(_("wrong parameter: %s"), strDir),
            "BFCore::DeleteDir()"
        );
        return false;
    }

    // init
    wxDir dir(strDir);

    // check if open
    if ( !(dir.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("directory %s is not open"), strDir), "BFCore::DeleteDir()");
        return false;
    }

    // is empty
    if (bOnlyIfEmpty)
    {
        if (dir.HasFiles() || dir.HasSubDirs())
        {
            BFSystem::Error(wxString::Format(_("directory %s is not empty"), strDir), "BFCore::DeleteDir()");
            return false;
        }
    }

    // get directories in the right (deletable) order
    wxArrayString   arrToDelete;
    GetSubDirectories(strDir, arrToDelete);
    arrToDelete.Add(strDir);

    // backup message
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("delete %s"), strDir));

    // delete subs
    for (size_t i = 0;
         i < arrToDelete.GetCount();
         ++i)
    {
        // remove write protection
        if ( bIgnoreWriteprotection )
            if ( HasFileAttribute_ReadOnly(arrToDelete[i]) )
                SetFileAttribute_ReadOnly(arrToDelete[i], false);

        // files in it?
        wxDir* pDirSub = new wxDir(arrToDelete[i]);
        if (pDirSub->HasFiles())
        {
            wxArrayString arrFilesToDelete;
            GetDirListing(arrToDelete[i], arrFilesToDelete);
            Delete(arrFilesToDelete, bOnlyIfEmpty, bIgnoreWriteprotection);
        }
        delete pDirSub;

        // delete
        if ( !(wxRmdir(arrToDelete[i])) )
        {
            BFSystem::Error(wxString::Format(_("couldn't delete directory %s\nmaybe it is write-protected or not-empty"), arrToDelete[i]), "BFCore::DeleteDir()");
            return false;
        }
    }

    return true;
}

/*static*/ bool BFCore::Delete (wxArrayString& arrDelete, bool bOnlyIfEmpty /*= false*/, bool bIgnoreWriteprotection /*= false*/)
{
    for (size_t i = 0; i < arrDelete.GetCount(); ++i)
    {
        // stop ?
        if ( BFCore::IsStop() )
            return false;

        if ( wxDir::Exists(arrDelete[i]) )
            DeleteDir(arrDelete[i], bOnlyIfEmpty, bIgnoreWriteprotection);
        else
            DeleteFile(arrDelete[i], bIgnoreWriteprotection);
    }

    return true;
}

long BFCore::GetDirFileCount(const wxString& strDir,
                             long* pDirCount /*= NULL*/,
                             long* pFileCount /*= NULL*/)
{
    if ( !(wxDir::Exists(strDir)) )
    {
        BFSystem::Error(wxString::Format(_("Can not count the files in %s.\nDirectory doesn't exists!"), strDir), "BFCore::GetDirFileCount()");
        return -1;
    }

    // count the files with a special traverser object
    wxDir dir(strDir);
    BFCountDirTraverser trav;
    dir.Traverse(trav);

    // set and return the counted values
    if (pDirCount != NULL)
        *pDirCount = trav.GetDirCount();

    if (pFileCount != NULL)
        *pFileCount = trav.GetFileCount();

    return trav.GetCount();
}

/*static*/ bool BFCore::CreateDir (const wxString& strNewDir)
{
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("create dir %s"), strNewDir));

    return ::wxMkdir(strNewDir);
}


/*static*/ bool BFCore::CreatePath (const wxString& strPath)
{
    /*if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("create path %s"), strPath));*/

    // init
    wxString strSub;
    wxStringTokenizer tkz(strPath, wxFILE_SEP_PATH);

    while (tkz.HasMoreTokens())
    {
        strSub = strSub + tkz.GetNextToken();

        if ( !(wxDir::Exists(strSub)) )
            if ( !(CreateDir(strSub)) )
                return false;

        strSub = strSub + wxFILE_SEP_PATH;
    }

    return true;
}



/*static*/ bool BFCore::HasFileAttribute_ReadOnly (const wxString& strFilename)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::HasFileAttribute_ReadOnly()"
        );
        return false;
    }

    // check for concrete attribute
    if ( attr & FILE_ATTRIBUTE_READONLY )
        return true;

    return false;
}


/*static*/ bool BFCore::HasFileAttribute_Archive (const wxString& strFilename)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::HasFileAttribute_Archive()"
        );
        return false;
    }

    // check for concrete attribute
    if ( attr & FILE_ATTRIBUTE_ARCHIVE )
        return true;

    return false;
}


/*static*/ bool BFCore::HasFileAttribute_Hidden (const wxString& strFilename)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::HasFileAttribute_Hidden()"
        );
        return false;
    }

    // check for concrete attribute
    if ( attr & FILE_ATTRIBUTE_HIDDEN )
        return true;

    return false;
}


/*static*/ bool BFCore::SetFileAttribute_ReadOnly (const wxString& strFilename, bool bReadOnly)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_ReadOnly()"
        );
        return false;
    }

    if ( attr & FILE_ATTRIBUTE_READONLY )
    {
        if ( !bReadOnly )
            attr = attr & ~FILE_ATTRIBUTE_READONLY;
    }
    else
    {
        if ( bReadOnly )
            attr = attr | FILE_ATTRIBUTE_READONLY;
    }

    if ( SetFileAttributes(strFilename, attr) == 0 )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Not able to set file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_ReadOnly()"
        );
        return false;
    }

    return true;
}


/*static*/ bool BFCore::SetFileAttribute_Archive (const wxString& strFilename, bool bArchive)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_Archive()"
        );
        return false;
    }

    if ( attr & FILE_ATTRIBUTE_ARCHIVE )
    {
        if ( !bArchive )
            attr = attr & ~FILE_ATTRIBUTE_ARCHIVE;
    }
    else
    {
        if ( bArchive )
            attr = attr | FILE_ATTRIBUTE_ARCHIVE;
    }

    if ( SetFileAttributes(strFilename, attr) == 0 )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Not able to set file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_Archive()"
        );
        return false;
    }

    return true;
}


/*static*/ bool BFCore::SetFileAttribute_Hidden (const wxString& strFilename, bool bHidden)
{
    // get attributes
    DWORD attr = GetFileAttributes(strFilename);

    // attributes valid?
    if ( attr == INVALID_FILE_ATTRIBUTES )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Invalid file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_Hidden()"
        );
        return false;
    }

    if ( attr & FILE_ATTRIBUTE_HIDDEN )
    {
        if ( !bHidden )
            attr = attr & ~FILE_ATTRIBUTE_HIDDEN;
    }
    else
    {
        if ( bHidden )
            attr = attr | FILE_ATTRIBUTE_HIDDEN;
    }

    if ( SetFileAttributes(strFilename, attr) == 0 )
    {
        BFSystem::Fatal
        (
            wxString::Format
            (
                _("Not able to set file attributes for %s. Return code of GetLastError() is %d"),
                strFilename,
                GetLastError()
            ),
            "BFCore::SetFileAttribute_Hidden()"
        );
        return false;
    }

    return true;
}


const wxString& BFCore::GetDateString ()
{
    return strCurrentDate_;
}

const wxString& BFCore::GetTimeString ()
{
    return strCurrentTime_;
}

const wxString& BFCore::GetDateString_Old ()
{
    return strCurrentDate_Old_;
}

const wxString& BFCore::GetTimeString_Old ()
{
    return strCurrentTime_Old_;
}

/*static*/ bool BFCore::CopyDirAttributes (const wxString& strSourceDir,
                                           const wxString& strDestinationDir)
{
    if ( SetFileAttributes(strDestinationDir, GetFileAttributes(strSourceDir)) )
		return true;
	else
		return false;
}


/*static*/ bool BFCore::VerifyFileContents (wxFile& f1, wxFile& f2)
{
    const int       ciSize = 4096;
    int             iRead = 0;
    wxChar          buf1[ciSize];
    wxChar          buf2[ciSize];

    // jump to beginning of the files
    f1.Seek(0);
    f2.Seek(0);

    // read-compare-loop
    while ( iRead != wxInvalidOffset && !(f1.Eof()) && !(f2.Eof()) )
    {
        // read on file one
        iRead = f1.Read(&buf1, ciSize);

        // read on file two
        if ( iRead != f2.Read(&buf2, ciSize) )
            // take care that the same length of bytes is read from both files
            return false;

        // compare readed buffers
        if ( memcmp(buf1, buf2, iRead) != 0)
            return false;
    }

    // no difference between buffers detected -> the files have the same content
    return true;
}


bool BFCore::VerifyFiles(MapStringPair& rMap,
                         ProgressWithMessage* pProgress /*= NULL*/,
                         bool bVerifyContent /*=BF_VERIFY_CONTENT_DEFAULT*/)
{
    for (size_t i = 0; i < rMap.size(); ++i)
    {
        if (BFCore::IsStop())
            return true;

        if (pProgress != NULL)
        {
            wxString strMsg = wxString::Format(_("compare %s with %s"), rMap[i].first.c_str(), rMap[i].second.c_str());

            if (pProgress->IsLocked())
            {
                /* this could be called a workaround for the following situation:
                   a sync-task sometimes need to call BFCore::CopyDir() with bVerify == true
                   so it could be called a sub-task inside the sync-task.
                   But the sync-task doesn't calculate the verify-steps of the sub-copy-task
                   for its progress-object. Of course there is a need for verifying the copied files
                   but don't tell it the Progress object because it only count the files and dirs
                   they need to synchronized. */
                pProgress->SetMessage(strMsg);
                pProgress->broadcastObservers();
            }
            else
            {
                pProgress->IncrementActualWithMessage(strMsg);
            }
        }

        if ( !VerifyFile(rMap[i].first.c_str(), rMap[i].second.c_str(), bVerifyContent) )
        {
            if (IsWhileBackup())
                BFSystem::Backup(wxString::Format(_("the files %s and %s NOT identically"),
                                                  rMap[i].first.c_str(),
                                                  rMap[i].second.c_str()));
            return false;
        }
    }

    return true;
}


/*static*/ bool BFCore::VerifyFile (const wxString& strFile1,
                         const wxString& strFile2,
                         bool bVerifyContent /*= BF_VERIFY_CONTENT_DEFAULT*/)
{
    wxFileName  fn1(strFile1), fn2(strFile2);

    // check file-objects
    if ( !(fn1.IsOk()) || !(fn2.IsOk()) )
    {
        BFSystem::Error(wxString::Format(_("file objects (%s\nand %s) not open"),
                                         strFile1,
                                         strFile2),
                        "BFCore::VerifyFile() - wxFileName");
        return false;
    }

    // verify file size
    wxStructStat statBuf1, statBuf2;
    wxStat(strFile1, &statBuf1);
    wxStat(strFile2, &statBuf2);
    if ( statBuf1.st_size != statBuf2.st_size )
    //if ( f1.Length() != f2.Length() )
        return false;

    /* verify file times */
    wxDateTime dt1m;
    wxDateTime dt2m;
    fn1.GetTimes(NULL, &dt1m, NULL);
    fn2.GetTimes(NULL, &dt2m, NULL);

    /* The difference don't have to be an error.
       Some filesystems (e.g. FAT32) save there filetimes in a lower resolution
       then 1 seconds. We need to handle this cases here. */
    if (dt1m != dt2m)
    {
        // take care of the low-resolution of some filesystems (e.g. FAT32 2sec-steps)
        if (wxMax(dt1m.GetValue(), dt2m.GetValue()) % wxMin(dt1m.GetValue(), dt2m.GetValue()) > 2001)
        {
            return false;
        }
    }

    // verify some file attributes
    if ( !(VerifyFileAttributes(fn1, fn2)) )
        return false;

    // verify byte-by-byte
    if (bVerifyContent)
    {
        // open the files
        wxFile f1(strFile1), f2(strFile2);

        // check open files
        if ( !(f1.IsOpened()) || !(f2.IsOpened()) )
        {
            BFSystem::Error(wxString::Format(_("file objects (%s\nand %s) not open"),
                                             strFile1,
                                             strFile2),
                            "BFCore::VerifyFile() - wxFile");
            return false;
        }

        return VerifyFileContents(f1, f2);
    }
    else
    {
        return true;
    }
}


/*static*/ bool BFCore::IsPathValid (const wxString& strPath)
{
	#ifndef __WXMSW__
		#error "Improve this methode for the current operating system!"
	#endif

	// to short for a path
	if ( strPath.Len() < 2 )
		return false;

	// not between or the same as a/A and z/Z
	if ( !((strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z')) )
		return false;

	// wxFILE_SEP_DSK (':')
	if ( strPath[1] != wxFILE_SEP_DSK )
		return false;

	// wxFILE_SEP_PATH ('\\')
	if ( strPath.Len() == 3
		&& strPath[2] != wxFILE_SEP_PATH )
		return false;

	// wxFILE_SEP_PATH
	wxArrayString arrTkz = wxStringTokenize ( strPath, wxFILE_SEP_PATH );

	for ( size_t i = 0;
		  i < arrTkz.size();
		  ++i )
	{
		if ( arrTkz[i].IsEmpty() )
			return false;
	}

	return true;
}
