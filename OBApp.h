/**
 * OBApp.h
 * definition of class OBApp
 * <comment>
 * 2006-04-05
 ***/

#ifndef OBAPP_H
#define OBAPP_H

// forwarde declarations
class OBMainFrame;

// includes
#include <wx/wx.h>
#include <wx/intl.h>
#include "OBCore.h"


///
class OBApp : public wxApp
{
    private:
        /// for internationalization
        wxLocale        locale_;

        /// the main window
        static OBMainFrame*    spMainFrame_;

    protected:
        /// reference to the OBCore instance
        OBCore& Core ()
        { return OBCore::Instance(); }

    public:
        /// constructor
        OBApp ();
        /// virtual destructor
        virtual ~OBApp ();

        /// start point like main()
        virtual bool OnInit();

        ///
        static OBMainFrame* MainFrame ();

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
};    // class OBApp

#endif    // OBAPP_H
