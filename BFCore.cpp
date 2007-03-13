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

#include "Progress.h"
#include "blackfisk.h"
#include "BFundef.h"

/*void BFCore::Init ()
{
    // set strCurrentDate_
    ::wxSnprintf(strCurrentDate_, 11, wxDateTime::Now().Format(_T("%Y-%m-%d")).c_str());
}*/

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


wxArrayString& BFCore::GetDirListing (const wxChar* dir, wxArrayString& arr, wxArrayString* pExcludeListing /*= NULL*/)
{
    if (dir == NULL)
        return arr;

    wxDir                   dirSource(dir);
    BFDirListingTraverser   trav(arr, pExcludeListing);

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
            {   // FATAL ERROR: it should not be possible to step in this code
                wxLogSysError(_("fatal error in BFCore::CreateZipFromDir(): no file AND no dir: %s"), fileArray[i].c_str());

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
                wxLogError
                (
                    _("zip-file %s is corrupt because of the entry %s (crc compared with %s)"),
                    pZipFileName,
                    entry->GetName().c_str(),
                    arrFiles[i].c_str()
                );

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
}


wxUint32 BFCore::GetFileCrc (const wxChar* pFilename)
{
    // check parameters
    if (pFilename == NULL)
    {
        wxLogSysError( _("wrong parameters in BFCore::GetFileCrc") );
        return 0;
    }

    // init file
    wxFile  file(pFilename);
    if ( !(file.IsOpened()) )
    {
        wxLogSysError( _("could not open file %s in BFCore::GetFileCrc"), pFilename );
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
        return false;

    return ::wxRenameFile(pSource, pDestination, bOverwrite);
}


bool BFCore::CopyFile (const wxChar* pSource, const wxChar* pDestination, bool bOverwrite /*= DEFAULT_OVERWRITE*/, bool bVerify /*= false*/)
{
    // check parameters
    if (pSource == NULL || pDestination == NULL)
        return false;

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
        {   // error because source has wildcards AND destination is only one specific file
            wxLogSysError ( _("error in BFCore::CopyFile(): can not copy %s to %s"),
                            pSource,
                            pDestination );
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

    return rc;
}


bool BFCore::DeleteFile (const wxChar* pFile, bool bIgnoreWriteProtection /*= DEFAULT_OVERWRITE*/)
{
    if (pFile == NULL)
        return false;

    if ( !bIgnoreWriteProtection && IsWriteProtected(pFile) )
        return false;

    return ::wxRemoveFile(pFile);
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
        return false;

    return ::wxMkdir(pNewDir);
}


bool BFCore::CreatePath (const wxChar* pPath)
{
    // check parameters
    if (pPath == NULL)
        return false;

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

    return true;
}

bool BFCore::DeleteDir (const wxChar* pDir, bool bOnlyIfEmpty /*= false*/, bool bIgnoreWriteprotection /*= false*/)
{
    if (pDir == NULL || !(wxDir::Exists(pDir)) )
    {
        wxLogSysError(_("Wrong parameters!"));
        return false;
    }

    wxDir dir(pDir);

    if (bOnlyIfEmpty)
    {
        if (dir.HasFiles() || dir.HasSubDirs())
        {
            wxLogSysError(_("directory is not empty"));
            return false;
        }
    }
    else
    {
        BFDeleteDirTraverser trav(*this, bIgnoreWriteprotection);
        dir.Traverse(trav);
    }

    return ::wxRmdir(pDir);
}


bool BFCore::CopyDir (const wxChar*         pSourceDir,
                      const wxChar*         pDestinationDir,
                      bool                  bVerify,
                      ProgressWithMessage*  pProgress /*= NULL*/)
{
    // check parameters
    if (pSourceDir == NULL || pDestinationDir == NULL)
    {
        wxLogSysError(_("Wrong parameters!"));
        return false;
    }

    // create destination dir
    if ( CreateDir(pDestinationDir) == false )
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

    // copy the dir
    wxDir dir(pSourceDir);
    BFCopyDirTraverser trav(*this, pDestinationDir, pMap, pProgress);
    dir.Traverse(trav);

    // verify
    if (bVerify)
    {
        if (pProgress != NULL)
            pProgress->SetLabel ( _("verify files") );

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
            wxLogError(_("the files %s and %s NOT identically"), rMap[i].first.c_str(), rMap[i].second.c_str());
            return false;
        }
    }

    return true;
}


bool BFCore::VerifyFile (const wxChar* pFile1, const wxChar* pFile2)
{
    #define BF_VERIFYFILE_ERR1 "file objects BFCore::VerifyFile() not open"

    // check parameters
    if (pFile1 == NULL || pFile2 == NULL)
    {
        wxLogSysError(_("wrong parameters in BFCore::VerifyFile()"));
        return false;
    }

    wxFileName  fn1(pFile1), fn2(pFile2);

    // check file-objects
    if ( !(fn1.IsOk()) || !(fn2.IsOk()) )
    {
        wxLogSysError(_(BF_VERIFYFILE_ERR1));
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
        wxLogSysError(_(BF_VERIFYFILE_ERR1));
        return false;
    }

    // verify byte-by-by
    return VerifyFileContents(f1, f2);
}


BFDirListingTraverser::BFDirListingTraverser (wxArrayString& rList, wxArrayString* pExcludeList /*=NULL*/)
                     : rList_(rList),
                       pExcludeList_(pExcludeList)

{
}


/*virtual*/ wxDirTraverseResult BFDirListingTraverser::OnFile(const wxString& filename)
{
    if ( pExcludeList_ != NULL )
    {
        if ( pExcludeList_->Index(filename) != wxNOT_FOUND )
            return wxDIR_CONTINUE;

        for (int i = 0; i < pExcludeList_->Count(); ++i)
            if ( filename.Matches((*pExcludeList_)[i]) )
                return wxDIR_CONTINUE;
    }

    rList_.Add(filename);

    return wxDIR_CONTINUE;
}


/*virtual*/ wxDirTraverseResult BFDirListingTraverser::OnDir(const wxString& dirname)
{
    if ( pExcludeList_ != NULL && pExcludeList_->Index(dirname) != wxNOT_FOUND )
        return wxDIR_IGNORE;

    rList_.Add(dirname);

    return wxDIR_CONTINUE;
}


BFCountDirTraverser::BFCountDirTraverser ()
                    : lFileCount_(0), lDirCount_(0)
{
}


/*virtual*/ wxDirTraverseResult BFCountDirTraverser::OnFile(const wxString& filename)
{
    ++lFileCount_;

    return wxDIR_CONTINUE;
}


/*virtual*/ wxDirTraverseResult BFCountDirTraverser::OnDir(const wxString& dirname)
{
    ++lDirCount_;

    return wxDIR_CONTINUE;
}


BFDeleteDirTraverser::BFDeleteDirTraverser (BFCore& rCore, bool bIgnoreWriteprotection /*= false*/)
                    : bIgnoreWriteprotection_(bIgnoreWriteprotection), core_(rCore)
{
}


/*virtual*/ wxDirTraverseResult BFDeleteDirTraverser::OnDir(const wxString& dirname)
{
    // remove write protection
    if (bIgnoreWriteprotection_)
        if (core_.IsWriteProtected(dirname))
            core_.SetWriteProtected(dirname, false);

    wxDir dir(dirname);

    if (dir.HasFiles() || dir.HasSubDirs())
    {
        BFDeleteDirTraverser trav(core_, bIgnoreWriteprotection_);
        dir.Traverse(trav);
    }

    ::wxRmdir(dirname);

    return wxDIR_IGNORE;
}


/*virtual*/ wxDirTraverseResult BFDeleteDirTraverser::OnFile(const wxString& filename)
{
    // remove write protection
    if (bIgnoreWriteprotection_)
        if (core_.IsWriteProtected(filename))
            core_.SetWriteProtected(filename, false);

    if ( ::wxRemoveFile(filename) == false )
        wxLogWarning(_("removeFile ERR"));

    return wxDIR_CONTINUE;
}


BFCopyDirTraverser::BFCopyDirTraverser (BFCore& rCore,
                                        const wxChar* pDestinationDirectory,
                                        MapStringPair* pRememberToVerify /*=NULL*/,
                                        ProgressWithMessage* pProgress /*= NULL*/)
                  : core_(rCore),
                    strDestination_(pDestinationDirectory),
                    pRememberToVerify_(pRememberToVerify),
                    pProgress_(pProgress)
{
}


/*virtual*/ wxDirTraverseResult BFCopyDirTraverser::OnDir(const wxString& dirname)
{
    // init
    wxDir dir(dirname);
    wxString strSubDest = strDestination_ + dirname.Right(dirname.Length() - dirname.Find(wxFILE_SEP_PATH, true));

    // create sub-dir
    if ( core_.CreateDir(strSubDest) == false )
        return wxDIR_IGNORE;

    // set attributes for sub-dir like the source-dir
    core_.CopyDirAttributes(dirname, strSubDest);

    // update progress
    pProgress_->IncrementActualWithMessage(dirname);

    // copy
    BFCopyDirTraverser trav(core_, strSubDest, pRememberToVerify_, pProgress_);
    dir.Traverse(trav);

    return wxDIR_IGNORE;
}


/*virtual*/ wxDirTraverseResult BFCopyDirTraverser::OnFile(const wxString& fileName)
{
    // init
    wxFileName  wxFn(fileName);
    wxString    strFrom(wxFn.GetFullPath());
    wxString    strTo(strDestination_ + wxFILE_SEP_PATH + wxFn.GetFullName());

    // copy
    pProgress_->IncrementActualWithMessage(fileName);
    core_.CopyFile(strFrom, strTo);

    // remember for a later verify
    if (pRememberToVerify_ != NULL)
        pRememberToVerify_->push_back(PairString(strFrom, strTo));

    return wxDIR_CONTINUE;
}
