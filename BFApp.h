/**
 * BFApp.h
 * definition of class BFApp
 * <comment>
 * 2006-04-05
 ***/

#ifndef BFAPP_H
#define BFAPP_H

// forwarde declarations
class BFMainFrame;

// includes
#include <wx/wx.h>
#include <wx/intl.h>
#include "BFCore.h"


///
class BFApp : public wxApp
{
    private:
        /// for internationalization
        wxLocale                locale_;

        /// the main window
        static BFMainFrame*     spMainFrame_;

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

        /** sum a array of strings to one string and return it */
        static wxString SumStrings (const wxArrayString& rStrings, const wxChar cSeperator = _T('\n'));
        /** prepend a string on each element of a string array
            and return the reference of the array */
        static wxArrayString& PrependString (wxArrayString& rStrings, const wxChar* strToPrepend);
        /** search for community on all strings and return the community
            example:
            string1 = "ABCDEFG" and string2 = "ABCwdFG
            return value is "ABC" */
        static wxString ExtractCommunity (wxArrayString& rStrings);

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
        void Test ();
        ///
        void Backup ();

        ///
        bool IsProjectModified ();
};    // class BFApp

#endif    // BFAPP_H
