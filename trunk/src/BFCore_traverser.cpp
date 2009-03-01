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

#include "BFSystem.h"
#include "BFundef.h"
#include "Progress.h"
#include "blackfisk.h"

BFDirListingTraverser::BFDirListingTraverser (wxArrayString&    rList,
                                              wxString          strStartDir /*= wxEmptyString*/,
                                              wxArrayString*    pExcludeList /*= NULL*/,
                                              bool              bOnlyDirectories /*= false*/)
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
    if ( pExcludeList_ )
    {
        // exact fit
        if ( pExcludeList_->Index(strCur) != wxNOT_FOUND )
            return wxDIR_CONTINUE;

        for (size_t i = 0; i < pExcludeList_->Count(); ++i)
        {
            // handle placeholders
            if ( strCur.Matches((*pExcludeList_)[i]) )
                return wxDIR_CONTINUE;

            // begin with
            if ( strCur.StartsWith((*pExcludeList_)[i] + wxFILE_SEP_PATH) )
                return wxDIR_CONTINUE;
        }
    }

    // add the dir to the listing
    rList_.Add(strCur);

    return wxDIR_CONTINUE;
}

const wxArrayString& BFDirListingTraverser::ListingArray ()
{
    return rList_;
}
/*
BFSynchroniseDirTraverser::BFSynchroniseDirTraverser (const wxString& strOriginalDirectory,
                                                      const wxString& strToSynchroniseDirectory,
                                                      wxArrayString& rList,
                                                      bool bVerify,
                                                      bool bVerifyContent,
                                                      ProgressWithMessage* pProgress /*= NULL*)
                          : BFDirListingTraverser(rList, strOriginalDirectory),
                            strToSynchronise_(strToSynchroniseDirectory),
                            pProgress_(pProgress),
                            bVerify_(bVerify),
                            bVerifyContent_(bVerifyContent)
{
}

/*virtual* wxDirTraverseResult BFSynchroniseDirTraverser::OnFile(const wxString& filename)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // listing
    BFDirListingTraverser::OnFile(filename);

    // target
    wxString strTarget = strToSynchronise_ + ListingArray().Last();

    // progress
    if (pProgress_ != NULL)
        pProgress_->IncrementActualWithMessage(filename);

    // copy
    if ( !(BFCore::Instance().VerifyFile(filename, strTarget, bVerifyContent_)) )
    {
        // get write-protection from destination
        bool bWriteprotectionDestination = BFCore::HasFileAttribute_ReadOnly(strTarget);

        //
        if (bWriteprotectionDestination)
        {
            // remove write-protection from destination file
            BFCore::SetFileAttribute_ReadOnly(strTarget, false);

            // get write-protection from source
            bool bWriteprotectionSource = BFCore::HasFileAttribute_ReadOnly(filename);

            // copy the file
            if ( BFCore::Instance().CopyFile(filename, strTarget, true, false) == false )
            {
                BFSystem::Error(wxString::Format(_("not able to synchronise %s\nwith %s (unknown reason)"), strTarget, filename),
                                "BFSynchroniseDirTraverser::OnFile() - copy without verify");
            }

            // reset write-protection on the destination if there was one on the source
            if (bWriteprotectionSource)
            {
                BFCore::SetFileAttribute_ReadOnly(strTarget, bWriteprotectionSource);

                if (bVerify_)
                {
                    if (BFCore::Instance().VerifyFile(filename, strTarget, bVerifyContent_) == false)
                    {
                        BFSystem::Error(wxString::Format(_("verifing the files %s\nand %s failed (unknown reason)\nmaybe they are not identic"), strTarget, filename),
                                        "BFSynchroniseDirTraverser::OnFile() - VerifyFile explicite");
                    }
                }
            }
        }
        else
        {
            // copy the file
            if ( BFCore::Instance().CopyFile(filename, strTarget, true, bVerify_, bVerifyContent_) == false )
            {
                BFSystem::Error(wxString::Format(_("not able to synchronise %s\nwith %s (unknown reason)"), strTarget, filename),
                                "BFSynchroniseDirTraverser::OnFile() - copy with 'bVerify_'");
            }
        }
    }

    return wxDIR_CONTINUE;
}

/*virtual* wxDirTraverseResult BFSynchroniseDirTraverser::OnDir(const wxString& dirname)
{
    // stop ?
    if ( BFCore::IsStop() )
        return wxDIR_STOP;

    // progress
    if (pProgress_ != NULL)
    {
        pProgress_->IncrementActualWithMessage(dirname);
    }

    // listing
    BFDirListingTraverser::OnDir(dirname);

    // target
    wxString strTarget = strToSynchronise_ + ListingArray().Last();

    // does it exist
    if ( !(wxDir::Exists(strTarget)) )
    {
        wxString strLabel = pProgress_->GetLabel();
        pProgress_->SetDeadCountMode(true);
        pProgress_->Lock();
        BFCore::Instance().CopyDir(dirname, strTarget, false, false, pProgress_);
        pProgress_->Unlock();
        pProgress_->SetLabel(strLabel);
        pProgress_->SetDeadCountMode(false);
    }

    return wxDIR_CONTINUE;
}*/

BFCountDirTraverser::BFCountDirTraverser ()
                    : lDirCount_(0),
                      lFileCount_(0)
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


BFCopyDirTraverser::BFCopyDirTraverser (const wxString& strDestinationDirectory,
                                        MapStringPair* pRememberToVerify /*=NULL*/,
                                        ProgressWithMessage* pProgress /*= NULL*/)
                  : strDestination_(strDestinationDirectory),
                    pProgress_(pProgress),
                    pRememberToVerify_(pRememberToVerify)
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

