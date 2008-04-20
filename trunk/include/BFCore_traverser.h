/**
 * Name:        BFCore_traverser.h
 * Purpose:     definition of wxDirTraverser derived classes used by BFCore
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

#ifndef BFCORE_TRAVERSER_H
#define BFCORE_TRAVERSER_H

#include <wx/dir.h>

#include "BFCore.h"

/** create a directory-listing with empty directories included */
class BFDirListingTraverser : public wxDirTraverser
{
    private:
        ///
        wxArrayString&      rList_;
        ///
        wxArrayString*      pExcludeList_;
        ///
        wxString            strStartDir_;
        ///
        bool                bOnlyDirectories_;

        /** handling for files and dirs is the same */
        wxDirTraverseResult HandleDirAndFile(const wxString& name, wxDirTraverseResult resultNegative);

    protected:
        ///
        const wxArrayString& ListingArray ();

    public:
        /** default ctor
            the listing is stored in 'rList'
            if you need relative pathes set 'strStartDir'
            set 'pExcludeList' to filter something (placeholders allowed) */
        BFDirListingTraverser (wxArrayString&   rList,
                               wxString         strStartDir = wxEmptyString,
                               wxArrayString*   pExcludeList = NULL,
                               bool             bOnlyDirectories = false);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

};  // class BFDirListingTraverser


///
class BFCountDirTraverser : public wxDirTraverser
{
    private:
        ///
        long lDirCount_;
        ///
        long lFileCount_;

    public:
        /// default ctor
        BFCountDirTraverser ();

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

        ///
        long GetCount ()
        { return lDirCount_ + lFileCount_; }
        ///
        long GetDirCount ()
        { return lDirCount_; }
        ///
        long GetFileCount ()
        { return lFileCount_; }
};  // class BFCountDirTraverser


/// to copy a dir with all files and sub-dirs in it
class BFCopyDirTraverser : public wxDirTraverser
{
    protected:
        ///
        wxString                strDestination_;
        ///
        ProgressWithMessage*    pProgress_;
        ///
        MapStringPair*          pRememberToVerify_;

    public:
        /// default ctor
        BFCopyDirTraverser (const wxChar* pDestinationDirectory,
                            MapStringPair* pRememberToVerify = NULL,
                            ProgressWithMessage* pProgress = NULL);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);

};  // class BFCopyDirTraverser

class BFSynchroniseDirTraverser : public BFDirListingTraverser
{
    private:
        /// directory to synchronise with the original
        wxString                strToSynchronise_;
        ///
        ProgressWithMessage*    pProgress_;
        ///
        bool                    bVerify_;
        ///
        bool                    bVerifyContent_;

    public:
        /// ctor
        BFSynchroniseDirTraverser (const wxChar* pOriginalDirectory,
                                   const wxChar* pToSynchroniseDirectory,
                                   wxArrayString& rList,
                                   bool bVerify,
                                   bool bVerifyContent,
                                   ProgressWithMessage* pProgress = NULL);

        ///
        virtual wxDirTraverseResult OnDir(const wxString& dirname);
        ///
        virtual wxDirTraverseResult OnFile(const wxString& filename);
};  // class BFSynchroniseDirTraverser

#endif // BFCORE_TRAVERSER_H
