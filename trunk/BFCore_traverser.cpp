/**
 * Name:        BFCore_traverser.cpp
 * Purpose:     implementation of wxDirTraverser derived classes used by BFCore
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

#include "BFCore_traverser.h"

#include "Progress.h"
#include "blackfisk.h"

BFDirListingTraverser::BFDirListingTraverser (wxArrayString&    rList,
                                              wxString          strStartDir /*= wxEmptyString*/,
                                              wxArrayString*    pExcludeList /*=NULL*/,
                                              bool              bOnlyDirectories /*=false*/)
                     : rList_(rList),
                       pExcludeList_(pExcludeList),
                       strStartDir_(strStartDir),
                       bOnlyDirectories_(bOnlyDirectories)

{
}


/*virtual*/ wxDirTraverseResult BFDirListingTraverser::OnFile(const wxString& filename)
{
    if (bOnlyDirectories_)
        return wxDIR_CONTINUE;
    else
        return HandleDirAndFile(filename, wxDIR_CONTINUE);
}


/*virtual*/ wxDirTraverseResult BFDirListingTraverser::OnDir(const wxString& dirname)
{
    return HandleDirAndFile(dirname, wxDIR_IGNORE);
}

wxDirTraverseResult BFDirListingTraverser::HandleDirAndFile(const wxString& name,
                                                            wxDirTraverseResult resultNegative)
{
    // the string to handle with
    wxString strCur(name);

    // do we handle absolute or relative dirnames or filenames?
    if (name.StartsWith(strStartDir_))
        strCur = name.Mid(strStartDir_.Len());
    else
        strCur = name;

    // check for exclusion
    if ( pExcludeList_ != NULL )
    {
        if ( pExcludeList_->Index(strCur) != wxNOT_FOUND )
            return resultNegative;

        // handle placeholders
        for (int i = 0; i < pExcludeList_->Count(); ++i)
            if ( strCur.Matches((*pExcludeList_)[i]) )
                return resultNegative;
    }

    // add the dir to the listing
    rList_.Add(strCur);

    return wxDIR_CONTINUE;
}

const wxArrayString& BFDirListingTraverser::ListingArray ()
{
    return rList_;
}

BFSynchronizeDirTraverser::BFSynchronizeDirTraverser (const wxChar* pOriginalDirectory,
                                                      const wxChar* pToSynchronizeDirectory,
                                                      wxArrayString& rList,
                                                      bool bVerify /*= false*/,
                                                      ProgressWithMessage* pProgress /*= NULL*/)
                          : BFDirListingTraverser(rList, pOriginalDirectory),
                            strToSynchronize_(pToSynchronizeDirectory),
                            bVerify_(bVerify),
                            pProgress_(pProgress)
{
}

/*virtual*/ wxDirTraverseResult BFSynchronizeDirTraverser::OnFile(const wxString& filename)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // listing
    BFDirListingTraverser::OnFile(filename);

    // XXX
    //BFSystem::Fatal(wxString::Format(_T("currently verifing %s"), filename), _T("syncTraverser::OnFile()"));

    // target
    wxString strTarget = strToSynchronize_ + ListingArray().Last();

    // progress
    if (pProgress_ != NULL)
        pProgress_->IncrementActualWithMessage(filename);

    // copy
    if ( !(BFCore::Instance().VerifyFile(filename, strTarget)) )
    {
        // get write-protection from destination
        bool bWriteprotectionDestination = BFCore::Instance().IsWriteProtected(strTarget);

        //
        if (bWriteprotectionDestination)
        {
            // remove write-protection from destination file
            BFCore::Instance().SetWriteProtected(strTarget, false);

            // get write-protection from source
            bool bWriteprotectionSource = BFCore::Instance().IsWriteProtected(filename);

            // copy the file
            if ( BFCore::Instance().CopyFile(filename, strTarget, true, false) == false )
            {
                BFSystem::Error(wxString::Format(_("not able to synchronize %s with %s (unknown reason)"), strTarget, filename),
                                _T("BFSynchronizeDirTraverser::OnFile() - copy without verify"));
            }

            // reset write-protection on the destination if there was one on the source
            if (bWriteprotectionSource)
            {
                BFCore::Instance().SetWriteProtected(strTarget, bWriteprotectionSource);

                if (bVerify_)
                {
                    if (BFCore::Instance().VerifyFile(filename, strTarget) == false)
                    {
                        BFSystem::Error(wxString::Format(_("files %s and %s are not verify (unknown reason)"), strTarget, filename),
                                        _T("BFSynchronizeDirTraverser::OnFile() - VerifyFile explicite"));
                    }
                }
            }
        }
        else
        {
            // copy the file
            if ( BFCore::Instance().CopyFile(filename, strTarget, true, bVerify_) == false )
            {
                BFSystem::Error(wxString::Format(_("not able to synchronize %s with %s (unknown reason)"), strTarget, filename),
                                _T("BFSynchronizeDirTraverser::OnFile() - copy with 'bVerify_'"));
            }
        }
    }

    return wxDIR_CONTINUE;
}

/*virtual*/ wxDirTraverseResult BFSynchronizeDirTraverser::OnDir(const wxString& dirname)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // listing
    BFDirListingTraverser::OnDir(dirname);

    // target
    wxString strTarget = strToSynchronize_ + ListingArray().Last();

    // does it exist
    if ( !(wxDir::Exists(strTarget)) )
        BFCore::Instance().CopyDir(dirname, strTarget, bVerify_);

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


BFCopyDirTraverser::BFCopyDirTraverser (const wxChar* pDestinationDirectory,
                                        MapStringPair* pRememberToVerify /*=NULL*/,
                                        ProgressWithMessage* pProgress /*= NULL*/)
                  : strDestination_(pDestinationDirectory),
                    pRememberToVerify_(pRememberToVerify),
                    pProgress_(pProgress)
{
}


/*virtual*/ wxDirTraverseResult BFCopyDirTraverser::OnDir(const wxString& dirname)
{
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // init
    wxDir dir(dirname);
    wxString strSubDest = strDestination_ + dirname.Right(dirname.Length() - dirname.Find(wxFILE_SEP_PATH, true));

    // create sub-dir
    if ( BFCore::Instance().CreateDir(strSubDest) == false )
        return wxDIR_IGNORE;

    // set attributes for sub-dir like the source-dir
    BFCore::Instance().CopyDirAttributes(dirname, strSubDest);

    // update progress
    if (pProgress_ != NULL)
        pProgress_->IncrementActualWithMessage(dirname);

    // copy
    BFCopyDirTraverser trav(strSubDest, pRememberToVerify_, pProgress_);
    dir.Traverse(trav);

    return wxDIR_IGNORE;
}


/*virtual*/ wxDirTraverseResult BFCopyDirTraverser::OnFile(const wxString& fileName)
{
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // init
    wxFileName  wxFn(fileName);
    wxString    strFrom(wxFn.GetFullPath());
    wxString    strTo(strDestination_ + wxFILE_SEP_PATH + wxFn.GetFullName());

    // copy
    if (pProgress_ != NULL)
        pProgress_->IncrementActualWithMessage(fileName);

    BFCore::Instance().CopyFile(strFrom, strTo);

    // remember for a later verify
    if (pRememberToVerify_ != NULL)
        pRememberToVerify_->push_back(PairString(strFrom, strTo));

    return wxDIR_CONTINUE;
}

