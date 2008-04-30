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

#include <../src/zlib/zlib.h>

#include "BFCore_traverser.h"
#include "BFApp.h"
#include "BFRootTaskApp.h"
#include "Progress.h"
#include "blackfisk.h"
#include "BFundef.h"


wxDEFINE_SCOPED_PTR_TYPE(wxZipEntry);


BFCore BFCore::sCore_;




BFCore& BFCore::Instance ()
{
    return sCore_;
}


BFCore::BFCore ()
      : bWhileBackup_(false)
{
    SetCurrentDateTime();
}


/*virtual*/ BFCore::~BFCore ()
{
    int i = 0;
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
    strCurrentTime_.Replace(_T(":"), _T("h"), false);
    strCurrentTime_.Replace(_T(":"), _T("m"), false);
    strCurrentTime_ << _T("s");
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
        if ( BFRootTaskApp::Instance().GetStopCurrentTask() == true
          || BFRootTaskApp::Instance().GetStopProject() == true)
          return true;

    return false;
}

wxArrayString& BFCore::GetDirListing (const wxString& strDir,
                                      wxArrayString& arr,
                                      wxArrayString* pExcludeListing /*= NULL*/,
                                      bool bRelativ /*= false*/)
{
    wxDir                   dirSource(strDir);
    wxString                strStartDir;

    if (bRelativ)
        strStartDir = strDir;

    BFDirListingTraverser   trav(arr, strStartDir, pExcludeListing);

    dirSource.Traverse(trav);

    return arr;
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

bool BFCore::VerifyFileAttributes (wxFileName& fn1,
                                   wxFileName& fn2,
                                   bool bIgnoreArchiveBit /*= false*/)
{
    // PPP
    long lAttr1 = GetFileAttributes(fn1.GetFullPath());
    long lAttr2 = GetFileAttributes(fn2.GetFullPath());

    // remove archive bit if there is one
    if (bIgnoreArchiveBit)
    {
        if (lAttr1 & FILE_ATTRIBUTE_ARCHIVE)
            lAttr1 = lAttr1 & ~FILE_ATTRIBUTE_ARCHIVE;

        if (lAttr2 & FILE_ATTRIBUTE_ARCHIVE)
            lAttr2 = lAttr2 & ~FILE_ATTRIBUTE_ARCHIVE;
    }

    // compare attributes
    return lAttr1 == lAttr2;
}


bool BFCore::CreateZipFromDir (const wxString& strZipName,
                               const wxString& strSourceDir,
                               wxArrayString* pExcludeListing,
                               bool bVerify,
                               ProgressWithMessage* pProgress /*= NULL*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // get all files and directories to zip
    wxArrayString fileArray;
    GetDirListing(strSourceDir, fileArray, pExcludeListing);

    // Progress
    if (pProgress != NULL)
    {
        long lRange;
        if (bVerify)
            lRange = fileArray.Count() * 2;
        else
            lRange = fileArray.Count();

        pProgress->SetLabel(wxString::Format(_("compressing in %s"), strZipName));
        pProgress->SetActual(0);
        pProgress->SetRange(lRange);
    }

    // init zip streams
    wxFFileOutputStream out(strZipName);
    wxZipOutputStream   zip(out);
    wxFFileInputStream* pIn             = NULL;
    wxZipEntry*         pEntry          = NULL;
    wxFileName          fileName;
    long                lLen            = wxString(strSourceDir).Length();

    // check if streams are ready
    if ( !(out.Ok()) || !(zip.IsOk()) )
        return false;

    // iterate on files and dirs
    for (int i = 0; i < fileArray.GetCount(); ++i)
    {
        // stop ?
        if ( BFCore::IsStop() )
            return false;

        fileName.Assign(fileArray[i]);

        // is it a file ?
        if ( fileName.FileExists() )
        {   // create zip-entry for a file

            pEntry = new wxZipEntry(fileName.GetFullPath().Mid(lLen), fileName.GetModificationTime());
            pIn = new wxFFileInputStream(fileName.GetFullPath(), _T("rb"));
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
                wxDir dir(fileArray[i]);

                // if it is an empty directory create it explicit as a zip-entry
                if ( !(dir.HasFiles() || dir.HasSubDirs()) )
                    zip.PutNextDirEntry(fileName.GetFullPath().Mid(lLen)); // TODO , fileName.GetModificationTime());
            }
            else
            {
                BFSystem::Fatal(wxString::Format(_("no file AND no dir: %s"), fileArray[i].c_str()), _T("BFCore::CreateZipFromDir()"));

                if (pProgress != NULL)
                    pProgress->DoEnd();

                return false;
            }
        }

        // count the item for progress
        if (pProgress != NULL)
            pProgress->IncrementActualWithMessage(fileArray[i]);

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
        BFSystem::Backup(wxString::Format(_("%s compressed to %s"), strSourceDir, strZipName));

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // verify the zip-file
    if (bVerify)
    {
        if (pProgress != NULL)
            pProgress->SetLabel(wxString::Format(_("verify %s by CRC"), strZipName));

        return VerifyZip (strZipName, fileArray, pProgress);
    }

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
                                _T("BFCore::VerifyZip()"));

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
        BFSystem::Error(wxString::Format(_("could not open file %s in BFCore::GetFileCrc"), strFilename), _T("BFCore::GetFileCrc()"));
        return 0;
    }

    // read data
    size_t bufsize = file.Length();
    Bytef buffer[bufsize];
    file.Read(&buffer, bufsize);

    // get crc for data
    return crc32(0, buffer, bufsize);

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


bool BFCore::CopyFile (const wxString& strSource,
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
                            _T("BFCore::CopyFile"));
            return false;
        }
    }

    // stop ?
    if ( BFCore::IsStop() )
        return false;

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
        for (int i = 0; i < arrSource.Count(); ++i)
        {
            // stop ?
            if ( BFCore::IsStop() )
                return false;

            if ( !(::wxCopyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH), bOverwrite)) )
                rc = false;
        }

        // verify the files
        if (bVerify && rc)
            for (int i = 0; i < arrSource.Count(); ++i)
            {
                // stop ?
                if ( BFCore::IsStop() )
                    return true;

                if ( !(VerifyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH), bVerifyContent)) )
                    rc = false;
            }
    }

    if (rc && bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("%s copyied to %s"), strSource, strDestination));

    return rc;
}


bool BFCore::DeleteFile (const wxString& strFile, bool bIgnoreWriteProtection /*= DEFAULT_OVERWRITE*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    if ( IsWriteProtected(strFile) )
    {
        if (bIgnoreWriteProtection)
        {
            SetWriteProtected(strFile, false);
        }
        else
        {
            BFSystem::Error(wxString::Format(_("file %s is write-protected"), strFile), _T("BFCore::DeleteFile()"));
            return false;
        }
    }

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("delete %s"), strFile));

    return ::wxRemoveFile(strFile);
}

wxArrayString& BFCore::GetSubDirectories (const wxString& strDir, wxArrayString& arr)
{
    // check parameters
    if (!(wxDir::Exists(strDir)) )
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::GetSubDirectories()"));
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
        BFSystem::Error(wxString::Format(_("directory %s is not open"), strUsedDir), _T("BFCore::GetSubDirectories()"));
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

bool BFCore::DeleteDir (const wxString& strDir,
                        bool bOnlyIfEmpty /*= false*/,
                        bool bIgnoreWriteprotection /*= false*/)
{
    // check parameters
    if (!(wxDir::Exists(strDir)) )
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::DeleteDir()"));
        return false;
    }

    // init
    wxDir dir(strDir);

    // check if open
    if ( !(dir.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("directory %s is not open"), strDir), _T("BFCore::DeleteDir()"));
        return false;
    }

    // is empty
    if (bOnlyIfEmpty)
    {
        if (dir.HasFiles() || dir.HasSubDirs())
        {
            BFSystem::Error(wxString::Format(_("directory %s is not empty"), strDir), _T("BFCore::DeleteDir()"));
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
    for (int i = 0;
         i < arrToDelete.GetCount();
         ++i)
    {
        // remove write protection
        if (bIgnoreWriteprotection)
            if (IsWriteProtected(arrToDelete[i]))
                SetWriteProtected(arrToDelete[i], false);

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
            BFSystem::Error(wxString::Format(_T("couldn't delete directory %s\nmaybe it is write-protected or not-empty"), arrToDelete[i]), _T("BFCore::DeleteDir()"));
            return false;
        }
    }

    return true;
}

bool BFCore::Delete (wxArrayString& arrDelete, bool bOnlyIfEmpty /*= false*/, bool bIgnoreWriteprotection /*= false*/)
{
    for (int i = 0; i < arrDelete.GetCount(); ++i)
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

bool BFCore::Synchronise (const wxString& strOriginal,
                          const wxString& strToSynchronise,
                          bool bVerify,
                          bool bVerifyContent,
                          bool bRealSync /*= true*/,
                          ProgressWithMessage* pProgress /*= NULL*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // create destination dir
    if ( CreatePath(strToSynchronise) == false )
        return false;

    CopyDirAttributes(strOriginal, strToSynchronise);

    // init copy progress
    if (pProgress != NULL)
    {
        long lCountFile, lCountDir;

        pProgress->SetLabel ( wxString::Format(_("synchronise from %s to %s\ncounting directories and files"), strOriginal, strToSynchronise) );
        pProgress->SetActual( 0 );
        pProgress->SetRange ( GetDirFileCount(strOriginal, &lCountDir, &lCountFile) );
        pProgress->SetLabel ( wxString::Format(_("synchronise from %s to %s"), strOriginal, strToSynchronise) );
    }

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("synchronise %s with %s (copy-step)"), strToSynchronise, strOriginal));

    // synchronise the dir
    wxDir dir(strOriginal);
    wxArrayString arrOriginalListing;
    BFSynchroniseDirTraverser trav(strOriginal,
                                   strToSynchronise,
                                   arrOriginalListing,
                                   bVerify,
                                   bVerifyContent,
                                   pProgress);
    dir.Traverse(trav);

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // check for deletable files ...
    if (bRealSync)
    {
        if (bWhileBackup_)
            BFSystem::Backup(wxString::Format(_("synchronise %s with %s (delete-step)"), strToSynchronise, strOriginal));

        if (pProgress != NULL)
            pProgress->SetLabel ( _("check for deletable files and directories") );

        wxArrayString arrToSyncListing;

        GetDirListing(strToSynchronise, arrToSyncListing, &arrOriginalListing, true);

        BFApp::PrependString(arrToSyncListing, strToSynchronise);

        // ... and sort them in the right order
        arrToSyncListing.Sort(true);

        // init delete progress
        if (pProgress != NULL)
            pProgress->SetLabel ( _("delete unexisting files and directories<") );

        // delete
        Delete(arrToSyncListing, false, true);
    }

    return true;
}

long BFCore::GetDirFileCount(const wxString& strDir,
                             long* pDirCount /*= NULL*/,
                             long* pFileCount /*= NULL*/)
{
    if ( !(wxDir::Exists(strDir)) )
    {
        BFSystem::Error(wxString::Format(_("Can not count the files in %s.\nDirectory doesn't exists!"), strDir), _T("BFCore::GetDirFileCount()"));
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

bool BFCore::CreateDir (const wxString& strNewDir)
{
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("create dir %s"), strNewDir));

    return ::wxMkdir(strNewDir);
}


bool BFCore::CreatePath (const wxString& strPath)
{
    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("create path %s"), strPath));

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


bool BFCore::CopyDir (const wxString&       strSourceDir,
                      const wxString&       strDestinationDir,
                      bool                  bVerify,
                      bool                  bVerifyContent /*= BF_VERIFY_CONTENT_DEFAULT*/,
                      ProgressWithMessage*  pProgress /*= NULL*/)
{
    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // create destination dir
    if ( CreatePath(strDestinationDir) == false )
        return false;

    CopyDirAttributes(strSourceDir, strDestinationDir);

    // init copy progress
    if (pProgress != NULL)
    {
        if ( !(pProgress->IsLocked()) )
        {
            long lCountFile, lCountDir, lRange;

            GetDirFileCount(strSourceDir, &lCountDir, &lCountFile);

            if (bVerify)
                lRange = lCountDir + lCountFile + lCountFile;
            else
                lRange = lCountDir + lCountFile;

            pProgress->SetActual( 0 );
            pProgress->SetRange ( lRange );
        }

        pProgress->SetLabel ( wxString::Format(_("copy to %s"), strDestinationDir) );
    }

    // init for verify
    MapStringPair   mapRememberToVerify;
    MapStringPair*  pMap = NULL;
    if (bVerify)
        pMap = &mapRememberToVerify;

    if (bWhileBackup_)
        BFSystem::Backup(wxString::Format(_("copy %s to %s"), strSourceDir, strDestinationDir));

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // copy the dir
    wxDir dir(strSourceDir);
    BFCopyDirTraverser trav(strDestinationDir, pMap, pProgress);
    dir.Traverse(trav);

    // stop ?
    if ( BFCore::IsStop() )
        return false;

    // verify
    if (bVerify)
    {
        if (pProgress != NULL)
            pProgress->SetLabel ( _("verify files") );

        if (bWhileBackup_)
            BFSystem::Backup(wxString::Format(_("verify %s"), strDestinationDir));

        return VerifyFiles (mapRememberToVerify, pProgress, bVerifyContent);
    }

    return true;
}


bool BFCore::IsWriteProtected (const wxString& strFilename)
{
    wxStructStat strucStat;

    wxStat(strFilename, &strucStat);

    return !(strucStat.st_mode & _S_IWRITE);
}


bool BFCore::SetWriteProtected (const wxString& strFilename, bool bWriteProtected)
{
    int iMode;

    if (bWriteProtected)
        iMode = _S_IREAD;
    else
        iMode = _S_IWRITE;

    // TODO wxChmod()
#ifdef _UNICODE
    return _wchmod(strFilename, iMode);
#else
    return _chmod(strFilename, iMode);
#endif
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

bool BFCore::CopyDirAttributes (const wxString& strSourceDir, const wxString& strDestinationDir)
{
    return SetFileAttributes(strDestinationDir, GetFileAttributes(strSourceDir));
}


bool BFCore::VerifyFileContents (wxFile& f1, wxFile& f2)
{
    const int       ciSize = 4096;
    size_t          iRead = 0;
    wxChar            buf1[ciSize];
    wxChar            buf2[ciSize];

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
    for (int i = 0; i < rMap.size(); ++i)
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


bool BFCore::VerifyFile (const wxString& strFile1,
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
                        _T("BFCore::VerifyFile() - wxFileName"));
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

    /* XXX */
    if (dt1m != dt2m)
    {
        // take care of the low-resolution of some filesystems (e.g. FAT32 2sec-steps)
        if (wxMax(dt1m.GetValue(), dt2m.GetValue()) % wxMin(dt1m.GetValue(), dt2m.GetValue()) > 2001)
        {
            return false;
        }
    }

    // verify file attributes but ignore archive-bit
    if ( !(VerifyFileAttributes(fn1, fn2, true)) )
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
                            _T("BFCore::VerifyFile() - wxFile"));
            return false;
        }

        return VerifyFileContents(f1, f2);
    }
    else
    {
        return true;
    }
}
