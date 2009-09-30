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
#include <wx/splitter.h>
#include <vector>

#define BF_ID_MAINFRAME                 1 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_QUIT            2 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_ABOUT           3 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_BACKUP          4 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_OPENPRJ         5 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SAVEPRJ         6 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SAVEPRJAS       7 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_CLOSEPRJ        8 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_NEWPRJ          9 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_PRJSETTINGS     10 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SETTINGS        11 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SHOWLOG         12 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SHOWLICENSE     13 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SHOWHISTORY     14 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_OPENWEBSITE     15 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_SUBMITBUG       16 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_FEAUTERREQUEST  17 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_PRJPLANNER      18 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_MAIL            19 + BF_MAINFRAME_ID_HIGHEST
#define BF_ID_MAINFRAME_FEEDBACK        20 + BF_MAINFRAME_ID_HIGHEST
#ifdef _DEBUG
    #define BF_ID_MAINFRAME_TEST        21 + BF_MAINFRAME_ID_HIGHEST
#endif

///
typedef std::vector<wxThread*> BFThreadVector;

/// main window of the application
class BFMainFrame : public wxFrame
{
    private:
        /// control to display the backup-structure
        BFBackupCtrl*       pBackupCtrl_;

        /// to display the real data-structure on HDD volumes
        BFDirCtrl*          pDirCtrl_;

        /// the ctrl holding 'pBackupCtrl_' and 'pDirCtrl_'
        wxSplitterWindow*   pSplitterCtrl_;

        ///
        wxMenu*             menuProject_;

        ///
        BFThreadVector      vecThreads_;

        ///
        void DeleteRememberedThreads ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFMainFrame ();
        /// virtual destructor
        virtual ~BFMainFrame ();

        ///
        static BFMainFrame* Instance();

        ///
        void EnableMenuProjectPlanner (bool bEnable);

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
        void OnFeauterRequest (wxCommandEvent& event);
        ///
        void OnMail (wxCommandEvent& event);
        ///
        void OnFeedback (wxCommandEvent& event);
        ///
        void OnThreadEnd (wxCommandEvent& event);

        /** remember the thread in vecThreads_
            to delete them correct in OnThreadEnd() */
        void RememberThread (wxThread* pThread);

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
        static bool QuestionYesNo (const wxString& strQuestion);
        /// displays a modal message-box, ask a YES-NO-CANCEL-question and return the answer as button ID
        static int QuestionYesNoCancel (const wxString& strQuestion);

        ///
        void OpenProjectSettings ();
        ///
        void OpenProjectPlanner ();

        ///
        void RefreshTitle ();

        /** Wrap a string depending on the current device context
            to the width in pixel specified by 'iWidthInPixel'.
            It will insert new-line characters in the string. */
        wxString& Wrap (wxString& str, int iWidthInPixel);

		/** Calculate the size of the string 'strText' in pixel and return it.
			The calculated size depend on the used wxWindow specified by 'pWin'.
			If 'pWin' NULL then the current wxMainFramee is used. */
		static wxSize GetTextSize (const wxString& strText, wxWindow* pWin = NULL);
		/// Please see 'wxSize GetTextSize (const wxString& strText, wxWindow* pWin = NULL);' for more detailes.
		static int GetTextWidth (const wxString& strText, wxWindow* pWin = NULL);

		/** */
		static void SetFontBold (wxWindow* pWindow, bool bBold = true);

    DECLARE_EVENT_TABLE();
};    // class BFMainFrame

#endif    // BFMAINFRAME_H
