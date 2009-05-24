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
