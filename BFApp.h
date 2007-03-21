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
        wxLocale        locale_;

        /// the main window
        static BFMainFrame*    spMainFrame_;

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
        static BFMainFrame* MainFrame ();
        /** it sets the static member spMainFrame_ in BFApp;
            see the call in the BFMainFrame ctor for more details */
        static void SetMainFrame (BFMainFrame* pMainFrame);

        ///
        bool OpenProject (const wxChar* filename);
        ///
        bool SaveProject (const wxChar* filename);
        ///
        void Test ();
        ///
        void Backup ();

        ///
        bool IsProjectModified ();
};    // class BFApp

#endif    // BFAPP_H
