/**
 * Name:        BFCore_traverser.cpp
 * Purpose:     implementation of wxDirTraverser derived classes used by BFCore
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-02-17
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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
                                              wxArrayString*    pExcludeList /*=NULL*/)
                     : rList_(rList),
                       pExcludeList_(pExcludeList),
                       strStartDir_(strStartDir)

{
}


/*virtual*/ wxDirTraverseResult BFDirListingTraverser::OnFile(const wxString& filename)
{
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

        // handle wildcards
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

    // target
    wxString strTarget = strToSynchronize_ + ListingArray().Last();

    // verify
    if ( !(BFCore::Instance().VerifyFile(filename, strTarget)) )
    {
        BFSystem::Debug(wxString::Format(_T("%s is not sync -> copy it from %s"),
                                         strTarget.c_str(),
                                         filename.c_str()),
                        _T("SycnTrav::OnFile()"));
        BFCore::Instance().CopyFile(filename, strTarget, true, bVerify_);
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
    {
        BFSystem::Debug(wxString::Format(_T("%s is not sync -> copy it from %s"),
                                         strTarget.c_str(),
                                         dirname.c_str()),
                        _T("SycnTrav::OnDir()"));
        BFCore::Instance().CopyDir(dirname, strTarget, bVerify_);
    }

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


BFDeleteDirTraverser::BFDeleteDirTraverser (bool bIgnoreWriteprotection /*= false*/)
                    : bIgnoreWriteprotection_(bIgnoreWriteprotection)
{
}


/*virtual*/ wxDirTraverseResult BFDeleteDirTraverser::OnDir(const wxString& dirname)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // remove write protection
    if (bIgnoreWriteprotection_)
        if (BFCore::Instance().IsWriteProtected(dirname))
            BFCore::Instance().SetWriteProtected(dirname, false);

    wxDir dir(dirname);

    if (dir.HasFiles() || dir.HasSubDirs())
    {
        BFDeleteDirTraverser trav(bIgnoreWriteprotection_);
        dir.Traverse(trav);
    }

    ::wxRmdir(dirname);

    return wxDIR_IGNORE;
}


/*virtual*/ wxDirTraverseResult BFDeleteDirTraverser::OnFile(const wxString& filename)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // remove write protection
    if (bIgnoreWriteprotection_)
        if (BFCore::Instance().IsWriteProtected(filename))
            BFCore::Instance().SetWriteProtected(filename, false);

    if ( ::wxRemoveFile(filename) == false )
        BFSystem::Error(wxString::Format(_("error while removing %s"), filename.c_str()), _T("BFDeleteDirTraverser::OnFile()"));

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

