/**
 * Name:        BFSettings.h
 * Purpose:     BFSettings class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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

#define BFSETTINGS_CURRENT_VERSION 1000

///
class BFSettings
{
    private:
        /** project-specific settings by default */
        BFProjectSettings   defaultPrj_;
        /** display backup-tree with replaced activate
            macros ("2000-01-01") or not ("<date>") */
        bool                bReplaceMacros_;
        /** display the filesystem with files
            or just with dirs and volumes */
        bool                bWithFiles_;
        /** max size of the application logfile in KB
            size does not matter if it is '0' */
        long                lMaxLogFileSize_;
        /** open the last opened project on start */
        bool                bOpenLastProject_;
        /** last opened projectes */
        wxArrayString       arrLastProjects_;
        /** verbose level for the application log file*/
        BFMessageType       verboseLog_;
        /** verbose level for application messages (message box) *
        BFMessageType       verboseMsg_;*/

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
        bool GetReplaceMacros ();
        ///
        void SetReplaceMacros (bool bReplace);
        ///
        bool GetWithFiles ();
        ///
        void SetWithFiles (bool bWithFiles);
        /// return the maximum size of a log file in kilobyte
        long GetMaxLogFileSize ();
        ///
        void SetMaxLogFileSize (long lSizeInKiloByte);
        ///
        const wxArrayString& GetLastProjects ();
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
        /**
        void SetVerboseLevelMsg (BFMessageType lvl);
        ///
        BFMessageType GetVerboseLevelMsg ();*/

        ///
        bool Serialize (jbSerialize& rA);
};    // class BFSettings

#endif    // BFSETTINGS_H
