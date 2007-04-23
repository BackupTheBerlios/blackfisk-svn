/**
 * implementation of class BFCore
 * 2006-02-17
 ***/

#include "BFCore.h"

#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/tokenzr.h>
#include <wx/ptr_scpd.h>

#include <../../src/zlib/zlib.h>

#include "BFCore_traverser.h"
#include "BFApp.h"
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
{
    // remember current date in a string
    ::wxSnprintf(strCurrentDate_, 11, wxDateTime::Now().Format(_T("%Y-%m-%d")).c_str());
}


/*virtual*/ BFCore::~BFCore ()
{
}


wxArrayString& BFCore::GetDirListing (const wxChar* dir,
                                      wxArrayString& arr,
                                      wxArrayString* pExcludeListing /*= NULL*/,
                                      bool bRelativ /*= false*/)
{
    if (dir == NULL)
        return arr;

    wxDir                   dirSource(dir);
    wxString                strStartDir;

    if (bRelativ)
        strStartDir = dir;

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

bool BFCore::VerifyFileAttributes (wxFileName& fn1, wxFileName& fn2)
{
    long lAttr1 = GetFileAttributes(fn1.GetFullPath());
    long lAttr2 = GetFileAttributes(fn2.GetFullPath());

    return lAttr1 == lAttr2;
}


bool BFCore::CreateZipFromDir (const wxChar* pstrZipName,
                               const wxChar* pstrSourceDir,
                               wxArrayString* pExcludeListing,
                               bool bVerify,
                               ProgressWithMessage* pProgress /*= NULL*/)
{
    // check parameters
    if (pstrZipName == NULL || pstrSourceDir == NULL)
        return false;

    // get all files and directories to zip
    wxArrayString fileArray;
    GetDirListing(pstrSourceDir, fileArray, pExcludeListing);

    // Progress
    if (pProgress != NULL)
    {
        long lRange;
        if (bVerify)
            lRange = fileArray.Count() * 2;
        else
            lRange = fileArray.Count();

        pProgress->SetLabel(wxString::Format(_("compressing in %s"), pstrZipName));
        pProgress->SetActual(0);
        pProgress->SetRange(lRange);
    }

    // init zip streams
    wxFFileOutputStream out(pstrZipName);
    wxZipOutputStream   zip(out);
    wxFFileInputStream* pIn             = NULL;
    wxZipEntry*         pEntry          = NULL;
    wxFileName          fileName;
    long                lLen            = wxString(pstrSourceDir).Length();

    // check if streams are ready
    if ( !(out.Ok()) || !(zip.IsOk()) )
        return false;

    // iterate on files and dirs
    for (int i = 0; i < fileArray.GetCount(); ++i)
    {
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

    BFSystem::Backup(wxString::Format(_("%s compressed to %s"), pstrSourceDir, pstrZipName));

    // verify the zip-file
    if (bVerify)
    {
        if (pProgress != NULL)
            pProgress->SetLabel(wxString::Format(_("verify %s by CRC"), pstrZipName));

        return VerifyZip (pstrZipName, fileArray, pProgress);
    }

    return true;
}


bool BFCore::VerifyZip (const wxChar* pZipFileName, wxArrayString& arrFiles, ProgressWithMessage* pProgress /*= NULL*/)
{
    // check parameters
    if (pZipFileName == NULL)
        return false;

    // init
    wxZipEntryPtr       entry;    // 'smart pointer'
    wxFFileInputStream  in(pZipFileName);
    wxZipInputStream    zip(in);
    int                 i = 0;

    while ( entry.reset(zip.GetNextEntry()), entry.get() != NULL )
    {
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
                pProgress->IncrementActualWithMessage(wxString::Format(_("verify checksum for %s"), entry->GetName().c_str()).c_str());

            // compare crc's
            if ( entry->GetCrc() != GetFileCrc(arrFiles[i]) )
            {
                BFSystem::Error(wxString::Format(_("zip-file %s is corrupt because of the entry %s (crc compared with %s)"),
                                                 pZipFileName,
                                                 entry->GetName().c_str(),
                                                 arrFiles[i].c_str()),
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

    BFSystem::Backup(wxString::Format(_("%s verifyied"), pZipFileName));

    return true;
}


wxUint32 BFCore::GetFileCrc (const wxChar* pFilename)
{
    // check parameters
    if (pFilename == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::GetFileCrc") );
        return 0;
    }

    // init file
    wxFile file(pFilename);
    if ( !(file.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("could not open file %s in BFCore::GetFileCrc"), pFilename), _T("BFCore::GetFileCrc()"));
        return 0;
    }

    // read data
    size_t bufsize = file.Length();
    Bytef buffer[bufsize];
    file.Read(&buffer, bufsize);

    // get crc for data
    return crc32(0, buffer, bufsize);
}


bool BFCore::MoveFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite /*= DEFAULT_OVERWRITE*/)
{
    if (pSource == NULL || pDestination == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::MoveFile") );
        return 0;
    }

    BFSystem::Backup(wxString::Format(_("move %s to %s"), pSource, pDestination));

    return ::wxRenameFile(pSource, pDestination, bOverwrite);
}


bool BFCore::CopyFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite /*= DEFAULT_OVERWRITE*/, bool bVerify /*= false*/)
{
    // check parameters
    if (pSource == NULL || pDestination == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::CopyFile") );
        return false;
    }

    wxString        strDest(pDestination);
    wxString        strSource(pSource);
    wxArrayString   arrSource;
    bool            rc = true;

    // check source filename for wildcards
    if ( strSource.Find('*') != -1 )
    {
        wxDir::GetAllFiles( strSource.BeforeLast(wxFILE_SEP_PATH),
                            &arrSource,
                            strSource.AfterLast(wxFILE_SEP_PATH) );
    }

    // check if destination is only a dir
    if ( ::wxDirExists(strDest) )
    {
        // append path seperator
        if ( strDest.Last() != wxFILE_SEP_PATH )
            strDest += wxFILE_SEP_PATH;

        // complete destination with fullname
        if ( arrSource.Count() == 0 )
            strDest += strSource.AfterLast(wxFILE_SEP_PATH);
    }
    else
    {
        if ( arrSource.Count() > 0 )
        {
            BFSystem::Error(wxString::Format(_("can not copy %s to %s"),
                                              pSource,
                                              pDestination),
                            _T("BFCore::CopyFile"));
            return false;
        }
    }

    if ( arrSource.Count() == 0 )
    {   // one file to copy
        rc = ::wxCopyFile(strSource, strDest, bOverwrite);

        // verify the file
        if (bVerify && rc)
            rc = VerifyFile(strSource, strDest);
    }
    else
    {   // more then one files to copy because of wildcards
        for (int i = 0; i < arrSource.Count(); ++i)
            if ( !(::wxCopyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH), bOverwrite)) )
                rc = false;

        // verify the files
        if (bVerify && rc)
            for (int i = 0; i < arrSource.Count(); ++i)
                if ( !(VerifyFile(arrSource[i], strDest + arrSource[i].AfterLast(wxFILE_SEP_PATH))) )
                    rc = false;
    }

    if (rc)
        BFSystem::Backup(wxString::Format(_("%s copyied to %s"), pSource, pDestination));

    return rc;
}


bool BFCore::DeleteFile (const wxChar* pFile, bool bIgnoreWriteProtection /*= DEFAULT_OVERWRITE*/)
{
    if (pFile == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::DeleteFile()"));
        return false;
    }

    if ( !bIgnoreWriteProtection && IsWriteProtected(pFile) )
        return false;

    BFSystem::Backup(wxString::Format(_("delete %s"), pFile));

    return ::wxRemoveFile(pFile);
}

bool BFCore::DeleteDir (const wxChar* pDir, bool bOnlyIfEmpty /*= false*/, bool bIgnoreWriteprotection /*= false*/)
{
    if (pDir == NULL || !(wxDir::Exists(pDir)) )
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::DeleteDir()"));
        return false;
    }

    wxDir dir(pDir);

    if (bOnlyIfEmpty)
    {
        if (dir.HasFiles() || dir.HasSubDirs())
        {
            BFSystem::Error(wxString::Format(_("directory %s is not empty"), pDir), _T("BFCore::DeleteDir()"));
            return false;
        }
    }
    else
    {
        BFDeleteDirTraverser trav(bIgnoreWriteprotection);
        dir.Traverse(trav);
    }

    BFSystem::Backup(wxString::Format(_("delete %s"), pDir));

    return ::wxRmdir(pDir);
}

bool BFCore::Delete (wxArrayString& arrDelete)
{
    for (int i = 0; i < arrDelete.GetCount(); ++i)
    {
        BFSystem::Debug(wxString::Format(_T("current delete target: %s"), arrDelete[i].c_str()), _T("BFCore::Delete()"));

        if ( wxDir::Exists(arrDelete[i]) )
            DeleteDir(arrDelete[i], false, true);
        else
            DeleteFile(arrDelete[i], true);
    }

    return true;
}

bool BFCore::Synchronize (const wxChar* pOriginal,
                          const wxChar* pToSynchronize,
                          bool bVerify,
                          ProgressWithMessage* pProgress /*= NULL*/)
{
    // check parameters
    if (pOriginal == NULL || pToSynchronize == NULL)
    {
        BFSystem::Fatal(_("Wrong parameters!"), _T("BFCore::Synchronize"));
        return false;
    }

    // create destination dir
    if ( CreatePath(pToSynchronize) == false )
        return false;

    CopyDirAttributes(pOriginal, pToSynchronize);

    // init copy progress
    if (pProgress != NULL)
    {
        long lCountFile, lCountDir;

        pProgress->SetActual( 0 );
        pProgress->SetRange ( GetDirFileCount(pOriginal, &lCountDir, &lCountFile) );
        pProgress->SetLabel ( wxString::Format(_("synchronize to %s"), pToSynchronize) );
    }

    BFSystem::Backup(wxString::Format(_("synchronize %s with %s (copy-step)"), pToSynchronize, pOriginal));

    // synchronize the dir
    wxDir dir(pOriginal);
    wxArrayString arrOriginalListing;
    BFSynchronizeDirTraverser trav(pOriginal, pToSynchronize, arrOriginalListing, bVerify, pProgress);
    dir.Traverse(trav);

    BFSystem::Backup(wxString::Format(_("synchronize %s with %s (delete-step)"), pToSynchronize, pOriginal));

    // check for deletable files
    wxArrayString arrToSyncListing;
    GetDirListing(pToSynchronize, arrToSyncListing, &arrOriginalListing, true);
    BFApp::PrependString(arrToSyncListing, pToSynchronize);
    BFSystem::Log(_T("files to delete..."));
    BFSystem::Log(BFApp::SumStrings(arrToSyncListing));
    Delete(arrToSyncListing);

    return true;
}

long BFCore::GetDirFileCount(const wxChar* pDir, long* pDirCount /*= NULL*/, long* pFileCount /*= NULL*/)
{
    // check parameters
    if (pDir == NULL)
        return -1;

    // count the files with a special traverser object
    wxDir dir(pDir);
    BFCountDirTraverser trav;
    dir.Traverse(trav);

    // set and return the counted values
    if (pDirCount != NULL)
        *pDirCount = trav.GetDirCount();

    if (pFileCount != NULL)
        *pFileCount = trav.GetFileCount();

    return trav.GetCount();
}

bool BFCore::CreateDir (const wxChar* pNewDir)
{
    if (pNewDir == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::CreateDir()"));
        return false;
    }

    BFSystem::Backup(wxString::Format(_("create dir %s"), pNewDir));

    return ::wxMkdir(pNewDir);
}


bool BFCore::CreatePath (const wxChar* pPath)
{
    // check parameters
    if (pPath == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::CreatePath()"));
        return false;
    }

    // init
    wxString strSub;
    wxStringTokenizer tkz(pPath, wxFILE_SEP_PATH);

    while (tkz.HasMoreTokens())
    {
        strSub = strSub + tkz.GetNextToken();

        if ( !(wxDir::Exists(strSub)) )
            if ( !(CreateDir(strSub)) )
                return false;

        strSub = strSub + wxFILE_SEP_PATH;
    }

    BFSystem::Backup(wxString::Format(_("create path %s"), pPath));

    return true;
}


bool BFCore::CopyDir (const wxChar*         pSourceDir,
                      const wxChar*         pDestinationDir,
                      bool                  bVerify,
                      ProgressWithMessage*  pProgress /*= NULL*/)
{
    // check parameters
    if (pSourceDir == NULL || pDestinationDir == NULL)
    {
        BFSystem::Fatal(_("Wrong parameters!"), _T("BFCore::CopyDir"));
        return false;
    }

    // create destination dir
    if ( CreatePath(pDestinationDir) == false )
        return false;

    CopyDirAttributes(pSourceDir, pDestinationDir);

    // init copy progress
    if (pProgress != NULL)
    {
        long lCountFile, lCountDir, lRange;

        GetDirFileCount(pSourceDir, &lCountDir, &lCountFile);

        if (bVerify)
            lRange = lCountDir + lCountFile + lCountFile;
        else
            lRange = lCountDir + lCountFile;

        pProgress->SetActual( 0 );
        pProgress->SetRange ( lRange );
        pProgress->SetLabel ( wxString::Format(_("copy to %s"), pDestinationDir) );
    }

    // init for verify
    MapStringPair   mapRememberToVerify;
    MapStringPair*  pMap = NULL;
    if (bVerify)
        pMap = &mapRememberToVerify;

    BFSystem::Backup(wxString::Format(_("copy %s to %s"), pSourceDir, pDestinationDir));

    // copy the dir
    wxDir dir(pSourceDir);
    BFCopyDirTraverser trav(pDestinationDir, pMap, pProgress);
    dir.Traverse(trav);

    // verify
    if (bVerify)
    {
        if (pProgress != NULL)
            pProgress->SetLabel ( _("verify files") );

        BFSystem::Backup(wxString::Format(_("verify %s"), pDestinationDir));

        return VerifyFiles (mapRememberToVerify, pProgress);
    }

    return true;
}


bool BFCore::IsWriteProtected (const wxChar* pFilename)
{
    wxStructStat strucStat;

    wxStat(pFilename, &strucStat);

    return !(strucStat.st_mode & _S_IWRITE);
}


bool BFCore::SetWriteProtected (const wxChar* pFilename, bool bWriteProtected)
{
    int iMode;

    if (bWriteProtected)
        iMode = _S_IREAD;
    else
        iMode = _S_IWRITE;

    // TODO wxChmod()
#ifdef _UNICODE
    return _wchmod(pFilename, iMode);
#else
    return _chmod(pFilename, iMode);
#endif
}


const wxChar* BFCore::GetDateString ()
{
    /* look at the member declaration for informations */
    return strCurrentDate_;
}


bool BFCore::CopyDirAttributes (const wxChar* pSourceDir, const wxChar* pDestinationDir)
{
    if (pSourceDir == NULL || pDestinationDir == NULL)
        return false;

    return SetFileAttributes(pDestinationDir, GetFileAttributes(pSourceDir));
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


bool BFCore::VerifyFiles(MapStringPair& rMap, ProgressWithMessage* pProgress /*= NULL*/)
{
    for (int i = 0; i < rMap.size(); ++i)
    {
        if (pProgress != NULL)
            pProgress->IncrementActualWithMessage(wxString::Format(_("compare %s with %s"), rMap[i].first.c_str(), rMap[i].second.c_str()));

        if ( !VerifyFile(rMap[i].first.c_str(), rMap[i].second.c_str()) )
        {
            BFSystem::Backup(wxString::Format(_("the files %s and %s NOT identically"),
                                              rMap[i].first.c_str(),
                                              rMap[i].second.c_str()));
            return false;
        }
    }

    return true;
}


bool BFCore::VerifyFile (const wxChar* pFile1, const wxChar* pFile2)
{
    // check parameters
    if (pFile1 == NULL || pFile2 == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFCore::VerifyFile()"));
        return false;
    }

    wxFileName  fn1(pFile1), fn2(pFile2);

    // check file-objects
    if ( !(fn1.IsOk()) || !(fn2.IsOk()) )
    {
        BFSystem::Error(wxString::Format(_("file objects (%s and %s) not open"),
                                         pFile1,
                                         pFile2),
                        _T("BFCore::VerifyFile() - wxFileName"));
        return false;
    }

    // verify file size
    wxStructStat statBuf1, statBuf2;
    wxStat(pFile1, &statBuf1);
    wxStat(pFile2, &statBuf2);
    if ( statBuf1.st_size != statBuf2.st_size )
    //if ( f1.Length() != f2.Length() )
        return false;

    /* verify file times
    wxDateTime dt1m;
    wxDateTime dt2m;
    fn1.GetTimes(NULL, &dt1m, NULL);
    fn2.GetTimes(NULL, &dt2m, NULL);

    if (dt1m != dt2m)
        return false;
*/
    // verify file attributes
    if ( !(VerifyFileAttributes(fn1, fn2)) )
        return false;

    // open the files
    wxFile f1(pFile1), f2(pFile2);

    // check open files
    if ( !(f1.IsOpened()) || !(f2.IsOpened()) )
    {
        BFSystem::Error(wxString::Format(_("file objects (%s and %s) not open"),
                                         pFile1,
                                         pFile2),
                        _T("BFCore::VerifyFile() - wxFile"));
        return false;
    }

    // verify byte-by-byte
    return VerifyFileContents(f1, f2);
}
