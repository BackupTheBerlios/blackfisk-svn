/**
 * Name:        BFApp.h
 * Purpose:     BFApp class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-05
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

#ifndef BFAPP_H
#define BFAPP_H

// forwarde declarations
class BFMainFrame;

// includes
#include <wx/wx.h>
#include <wx/intl.h>
#include "BFCore.h"
#include "BFMsgObserver.h"


///
class BFApp : public wxApp
{
    /// XXX
    public:
        void DebugLogPointers ();

    private:
        /// for internationalization
        wxLocale                locale_;

        /// the main window
        static BFMainFrame*     spMainFrame_;

        /** handle messages from the application
            it is not needed to touch it directly!
            just use OBSystem to display messages to the user */
        BFMsgObserver           msgObserver_;

    protected:
        /// reference to the OBCore instance
        BFCore& Core ()
        { return BFCore::Instance(); }

    public:
        /// constructor
        BFApp ();
        /// virtual destructor
        virtual ~BFApp ();

        /// start point like main()
        virtual bool OnInit();
        ///
        virtual int OnExit();

        ///
        static BFMainFrame* MainFrame ();
        /** it sets the static member spMainFrame_ in BFApp;
            see the call in the BFMainFrame ctor for more details */
        static void SetMainFrame (BFMainFrame* pMainFrame);

        ///
        static wxString GetFullApplicationName ();

        /** prepend a string on each element of a string array
            and return the reference of the array */
        static wxArrayString& PrependString (wxArrayString& rStrings, const wxChar* strToPrepend);
        /** search for community on all strings and return the community
            example:
            string1 = "ABCDEFG" and string2 = "ABCwdFG
            return value is "ABC" */
        static wxString ExtractCommunity (const wxArrayString& rStrings);

        ///
        const wxString& GetCurrentProjectFilename ();
        ///
        wxString GetCurrentProjectName ();
        ///
        bool OpenProject (const wxChar* filename);
        ///
        bool SaveProject (const wxChar* filename);
        /// read the global settings from the settings file
        static bool ReadSettings ();
        /// save the global settings to the settings file
        static bool SaveSettings ();
        ///
        bool SaveCurrentProject ();
        /** it close the current project */
        bool CloseCurrentProject (bool bCheckForModifications = true);

        ///
        bool IsProjectModified ();

        ///
        void Sound_BackupFinished ();

#ifdef _DEBUG
        void Test ();
#endif

};    // class BFApp

///
DECLARE_APP(BFApp);

#endif    // BFAPP_H
