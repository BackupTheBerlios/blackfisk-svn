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
        long                lMaxLogFileSize_;
        /** open the last opened project on start */
        bool                bOpenLastProject_;
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
        /** order of the BFBackupCtrl and the BFDirCtrl in the main-window */
        bool                bSwitchMainCtrls_;
        /** position of the sash in the splitter window of the main-window */
        int                 iSashPositionInMainWindow_;
        /** show hidden files in the directory tree */
        bool                bShowHiddenFiles_;

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
        long GetMaxLogFileSize ();
        ///
        void SetMaxLogFileSize (long lSizeInKiloByte);
        ///
        const wxString& GetLastProject ();
        ///
        void SetLastProject (const wxChar* strFilename);
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
        bool Serialize (jbSerialize& rA);
};    // class BFSettings

#endif    // BFSETTINGS_H
