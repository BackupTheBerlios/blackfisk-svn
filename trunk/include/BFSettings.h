/**
 * Name:        BFSettings.h
 * Purpose:     BFSettings class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFSETTINGS_H
#define BFSETTINGS_H

#include "BFProjectSettings.h"
#include "BFSystem.h"
#include "jbSerialize.h"

///
class BFSettings
{
    public:
        ///
        static const long langCount_ = 3;
        ///
        static const wxLanguage langIds_[3];
        ///
        static const wxString langNames_[3];

    private:
        /** project-specific settings by default */
        BFProjectSettings   defaultPrj_;
        /** display backup-tree with filled placeholders
            or the placeholders itself ("2000-01-01") */
        bool                bFillBlackfiskPlaceholders_;
        /** display the filesystem with files
            or just with dirs and volumes */
        bool                bWithFiles_;
        /** max size of the application logfile in KB
            size does not matter if it is '0' */
        wxUint32            lMaxLogFileSizeKB_;
        /** open the last opened project on start */
        bool                bOpenLastProject_;
		/** save the projects automaticly without asking */
		bool				bAutosaveProject_;
        /** last opened projecte */
        wxString            strLastProject_;
        /** verbose level for the application log file*/
        BFMessageType       verboseLog_;
        /** language for the application */
        wxLanguage          lang_;
        /** last size of the main-window */
        wxSize              sizeMainWindow_;
        /** last position of the main-window */
        wxPoint             pointMainWindow_;
        /** last maximized state of the main-window */
        bool                bMainWindowMaximized_;
        /** order of the BFBackupCtrl and the BFDirCtrl in the main-window */
        bool                bSwitchMainCtrls_;
        /** position of the sash in the splitter window of the main-window */
        int                 iSashPositionInMainWindow_;
        /** show hidden files in the directory tree */
        bool                bShowHiddenFiles_;
        /** time in days till the next check for a new blackfisk version
            It is 0 if there shouldn't be a check. */
        wxInt32             lDaysTillNextCheck_;
        /** last check for a new blackfisk version */
        wxDateTime          dateLastVersionCheck_;

        /** The Type of the scheduler.
            0 means no scheduler is used by blackfisk.
            1 means the in-build wxCron is used
            2 means a extern wxCron is used */
        wxInt32             lScheduler_;
        ///
        wxString            strCrontab_;

        /** Behaviour of sounds (wave-files).
            0 means sound is played.
            1 means only if autobackup (option --run) sound is played.
            2 means no sound is played. */
        int                 iSound_;
        /** Behaviour of beeps (pc-speaker).
            0 means beep is played.
            1 means only if autobackup (option --run) beep is played.
            2 means no beep is played. */
        int                 iBeep_;

        ///
        static BFSettings   sSettings_;

        /// private ctor
        BFSettings ();

    public:
        ///
        static BFSettings& Instance ()
        {
            return sSettings_;
        }

        /// virtual destructor
        virtual ~BFSettings ();

        ///
        void InitDefaultValues();

        ///
        BFProjectSettings& GetDefaultProjectSettings ();
        ///
        bool GetFillBlackfiskPlaceholders();
        ///
        void SetFillBlackfiskPlaceholders (bool bFill);
        ///
        bool GetWithFiles ();
        ///
        void SetWithFiles (bool bWithFiles);
        /// return the maximum size of a log file in kilobyte
        wxUint32 GetMaxLogFileSizeInKB ();
        ///
        void SetMaxLogFileSizeInKB (wxUint32 lSizeInKiloByte);
        ///
        const wxString& GetLastProject ();
        ///
        void SetLastProject (const wxString& strFilename);
		///
		bool GetAutosaveProjects ();
		///
		void SetAutosaveProjects (bool bAutosave);
        ///
        void SetOpenLastProject (bool bOpen);
        ///
        bool GetOpenLastProject ();
        ///
        void SetVerboseLevelLog (BFMessageType lvl);
        ///
        BFMessageType GetVerboseLevelLog ();
        ///
        void SetLanguage (wxLanguage lang);
        ///
        wxLanguage GetLanguage ();
        ///
        void SetMainWindowSize (const wxSize& size);
        ///
        wxSize GetMainWindowSize ();
        ///
        void SetMainWindowPosition (const wxPoint& point);
        ///
        wxPoint GetMainWindowPosition ();
        ///
        void SetMainWindowMaximized (bool bMax);
        ///
        bool GetMainWindowMaximized ();
        ///
        void SetSwitchMainCtrls (bool bSwitch);
        ///
        bool GetSwitchMainCtrls ();
        ///
        void SetSashPositionInMainWindow (int pos);
        ///
        int GetSashPositionInMainWindow ();
        ///
        void SetShowHiddenFiles (bool bShowHidden);
        ///
        bool GetShowHiddenFiles ();
        ///
        void SetDaysTillNextCheck (wxInt32 lDays);
        ///
        wxInt32 GetDaysTillNextCheck ();
        ///
        void SetLastVersionCheck (wxDateTime date);
        ///
        wxDateTime GetLastVersionCheck ();
        /** Return true if a new version check could be done. */
        bool CheckForNewVersion ();
        ///
        wxInt32 GetScheduler ();
        ///
        void SetScheduler (wxInt32 lScheduler);
        ///
        const wxString& GetCrontab ();
        ///
        void SetCrontab (const wxString& strCrontab);

        ///
        void SetSoundBehaviour (int iSound);
        ///
        int GetSoundBehaviour ();
        ///
        void SetBeepBehaviour (int iBeep);
        ///
        int GetBeepBehaviour ();

        ///
        bool Serialize (jbSerialize& rA);
};    // class BFSettings

#endif    // BFSETTINGS_H
