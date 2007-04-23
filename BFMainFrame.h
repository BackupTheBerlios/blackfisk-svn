/**
 * BFMainFrame.h
 * definition of class BFMainFrame
 * the main window of OctopusBackup
 * 2006-04-05
 ***/

#ifndef BFMAINFRAME_H
#define BFMAINFRAME_H

class Progress;
class ProgressWithMessage;
class BFBackupCtrl;
class BFBackupTree;
class BFDirCtrl;

#include <wx/wx.h>

#include "BFApp.h"
#include "BFMessageDlg.h"

///
enum
{
    ID_Quit = 1,
    ID_About,
    ID_Test,
    ID_Backup,
    ID_OpenProject,
    ID_SaveProject,
    ID_SaveProjectAs,
    ID_CloseProject,
    ID_NewProject,
    ID_ProjectSettings,
    ID_Settings
};

///
class BFMainFrame : public wxFrame
{
    private:
        /** points to the application object
            it is the main link down to the application layer */
        static BFApp*       spApp_;

        /// control to display the backup-structure
        BFBackupCtrl*       pBackupCtrl_;

        /// to display the real data-structure on HDD volumes
        BFDirCtrl*          pDirCtrl_;

        /** display messages from the application;
            it is not needed to touch it directly!
            just use OBSystem to display messages to the user */
        BFMessageDlg        msgDlg_;

    protected:
        /// proteced members

    public:
        /// constructor
        BFMainFrame (BFApp& rApp);
        /// virtual destructor
        virtual ~BFMainFrame ();

        ///
        static BFMainFrame* Instance()
        { return spApp_->MainFrame(); }

        ///
        BFBackupCtrl* BackupCtrl ();
        ///
        BFBackupTree* BackupTree ();

        ///
        void OnQuit (wxCommandEvent& event);
        ///
        void OnAbout (wxCommandEvent& event);
        ///
        void OnTest (wxCommandEvent& event);
        ///
        void OnBackup (wxCommandEvent& event);
        ///
        void OnProject (wxCommandEvent& event);
        ///
        void OnSettings (wxCommandEvent& event);

        /** ask for the project file to open and store it in 'strProject'
            it handle another open and modified project */
        bool AskOpenProject (wxString& strProject);
        /** ask for the project filename to save and store it in 'strProject' */
        bool AskSaveProject (wxString& strProject);
        /** handle a modified project, ask if it should be saved and does it
            it return 'false' if the user press the cancel button
            it return 'true' if the user press yes or no */
        bool AskModification ();

        /** Create a progress-object and a ProgressDialog as an observer
            for it.
            On default you do not have to care about life time of the
            progress-object or the dialog. If the progress is on 100% the
            observer will destroy itself and that will destroy the progress
            object, too. *
        ProgressWithMessage* StartProgress ();*/

        /// displays a modal message-box, ask a YES-NO-question and return the answer as bool
        bool QuestionYesNo (const wxChar* strQuestion);
        /// displays a modal message-box, ask a YES-NO-CANCEL-question and return the answer as button ID
        int QuestionYesNoCancel (const wxChar* strQuestion);

        ///
        void OpenProjectSettings ();

    DECLARE_EVENT_TABLE();
};    // class BFMainFrame

#endif    // BFMAINFRAME_H
