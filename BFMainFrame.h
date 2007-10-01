/**
 * Name:        BFMainFrame.cpp
 * Purpose:     BFMainFrame class definition
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
#ifdef _DEBUG
    ID_Test,
#endif
    ID_Backup,
    ID_OpenProject,
    ID_SaveProject,
    ID_SaveProjectAs,
    ID_CloseProject,
    ID_NewProject,
    ID_ProjectSettings,
    ID_LastProject1,
    ID_LastProject2,
    ID_LastProject3,
    ID_LastProject4,
    ID_Settings,
    ID_DisplayLog,
    ID_ShowLicense,
    ID_ShowHistory,
    ID_OpenWebSite,
    ID_SubmitBug
};

/// main window of the application
class BFMainFrame : public wxFrame
{
    private:
        /** points to the application object
            it is the main link down to the application layer */
        //static BFApp*       spApp_;

        /// control to display the backup-structure
        BFBackupCtrl*       pBackupCtrl_;

        /// to display the real data-structure on HDD volumes
        BFDirCtrl*          pDirCtrl_;

        ///
        wxMenu*             menuProject_;

        ///
        void CreateLastProjectMenu ();

#ifdef _DEBUG
    void Test ();
#endif

    protected:
        /// proteced members

    public:
        /// constructor
        BFMainFrame (BFApp& rApp);
        /// virtual destructor
        virtual ~BFMainFrame ();

        ///
        static BFMainFrame* Instance()
        { return wxGetApp().MainFrame(); }

        ///
        BFBackupCtrl* BackupCtrl ();
        ///
        BFBackupTree* BackupTree ();

        ///
        void OnClose (wxCloseEvent& event);
        ///
        void OnQuit (wxCommandEvent& event);
        ///
        void OnAbout (wxCommandEvent& event);
#ifdef _DEBUG
        ///
        void OnTest (wxCommandEvent& event);
#endif
        ///
        void OnBackup (wxCommandEvent& event);
        ///
        void OnProject (wxCommandEvent& event);
        ///
        void OnLastProject (wxCommandEvent& event);
        ///
        void OnSettings (wxCommandEvent& event);
        ///
        void OnDisplayLog (wxCommandEvent& event);
        ///
        void OnShowLicense (wxCommandEvent& event);
        ///
        void OnShowHistory (wxCommandEvent& event);
        ///
        void OnOpenWebSite (wxCommandEvent& event);
        ///
        void OnSubmitBug (wxCommandEvent& event);
        ///
        void OnThreadEnd (wxCommandEvent& event);

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

        ///
        void RefreshTitle ();

    DECLARE_EVENT_TABLE();
};    // class BFMainFrame

#endif    // BFMAINFRAME_H
