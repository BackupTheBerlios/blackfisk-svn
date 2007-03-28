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
    ID_SaveProject
};

/// menuIDs for BFDirCtrl and BFBackupTree
#define BFBACKUPCTRL_ID_ADDDESTINATION      1 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_COPY_DIR            2 + wxID_HIGHEST
#define BFDIRCTRL_ID_BACKUP_DIRCOPY         BFBACKUPCTRL_ID_COPY_DIR
#define BFBACKUPCTRL_ID_COPY_FILE           3 + wxID_HIGHEST
#define BFDIRCTRL_ID_BACKUP_FILECOPY        BFBACKUPCTRL_ID_COPY_FILE
#define BFDIRCTRL_ID_FILEBUTTON             4 + wxID_HIGHEST
#define BFDIRCTRL_ID_ADDDESTINATION         5 + wxID_HIGHEST
#define BFDIRCTRL_ID_CREATEDESTINATION      6 + wxID_HIGHEST

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
        //int Message (BFMessageType type, wxString strMessage, bool bLog = false);
        /// displays a modal info-message-box without login the message
        //int Message (wxString strMessage);
        /// displays a modal message-box, ask a YES-NO-question and return the answer as bool
        bool QuestionYesNo (const wxChar* strQuestion);
        /// displays a modal message-box, ask a YES-NO-CANCEL-question and return the answer as button ID
        int QuestionYesNoCancel (const wxChar* strQuestion);

    DECLARE_EVENT_TABLE();
};    // class BFMainFrame

#endif    // BFMAINFRAME_H
