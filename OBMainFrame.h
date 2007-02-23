/**
 * OBMainFrame.h
 * definition of class OBMainFrame
 * the main window of OctopusBackup
 * 2006-04-05
 ***/

#ifndef OBMAINFRAME_H
#define OBMAINFRAME_H

class Progress;
class ProgressWithMessage;
class OBBackupCtrl;
class OBDirCtrl;

#include <wx/wx.h>

#include "OBApp.h"
#include "OBMessageDlg.h"

///
enum
{
    ID_Quit = 1,
    ID_About,
    ID_Test,
    ID_Backup,
    ID_OpenProject,
    ID_SaveProject
};


///
class OBMainFrame : public wxFrame
{
    private:
        /** points to the application object
            it is the main link down to the application layer */
        static OBApp*       spApp_;

        /// control to display the backup-structure
        OBBackupCtrl*       pBackupCtrl_;

        /// to display the real data-structure on HDD volumes
        OBDirCtrl*          pDirCtrl_;

        /** display messages from the application;
            it is not needed to touch it directly!
            just use OBSystem to display messages to the user */
        OBMessageDlg        msgDlg_;

    protected:
        /// proteced members

    public:
        /// constructor
        OBMainFrame (OBApp& rApp);
        /// virtual destructor
        virtual ~OBMainFrame ();

        ///
        static OBMainFrame* Instance()
        { return spApp_->MainFrame(); }
        ///
        void OnQuit (wxCommandEvent& event);
        ///
        void OnAbout (wxCommandEvent& event);
        ///
        void OnTest (wxCommandEvent& event);
        ///
        void OnBackup (wxCommandEvent& event);
        ///
        void OnOpenProject (wxCommandEvent& event);
        ///
        void OnSaveProject (wxCommandEvent& event);

        ///
        bool SaveProject ();

        /** Create a progress-object and a ProgressDialog as an observer
            for it.
            On default you do not have to care about life time of the
            progress-object or the dialog. If the progress is on 100% the
            observer will destroy itself and that will destroy the progress
            object, too. *
        ProgressWithMessage* StartProgress ();*/

        /// displays a modal message-box and return the pressed button
        //int Message (OBMessageType type, wxString strMessage, bool bLog = false);
        /// displays a modal info-message-box without login the message
        //int Message (wxString strMessage);
        /// displays a modal message-box, ask a YES-NO-question and return the answer as bool
        bool QuestionYesNo (const wxChar* strQuestion);
        /// displays a modal message-box, ask a YES-NO-CANCEL-question and return the answer as button ID
        int QuestionYesNoCancel (const wxChar* strQuestion);


    DECLARE_EVENT_TABLE();
};    // class OBMainFrame

#endif    // OBMAINFRAME_H
