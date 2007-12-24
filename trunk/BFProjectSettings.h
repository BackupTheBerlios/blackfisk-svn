/**
 * Name:        BFProjectSettings.h
 * Purpose:     BFProjectSettings class definition
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

#ifndef BFPROJECTSETTINGS_H
#define BFPROJECTSETTINGS_H

#include <wx/string.h>
#include "BFSystem.h"
#include "BFTaskBase.h"
#include "jbSerialize.h"


///
class BFProjectSettings
{
    private:
        ///
        BF_VerboseLevel     iVerboseLevel_;
        /// what is to do on a fatal error
        BF_StopLevel        iStopLevelOnFatal_;
        /// what is to do on a normal error
        BF_StopLevel        iStopLevelOnError_;
        /// what is to do on a warning
        BF_StopLevel        iStopLevelOnWarning_;
        /// path to the location where the backup-process is logged
        wxString            strBackupLogLocation_;

        ///
        void SetStopLevel (BF_StopLevel& iTarget, BF_StopLevel iStopLevel);

    protected:
        /// proteced members

    public:
        /// constructor
        BFProjectSettings ();
        /// virtual destructor
        virtual ~BFProjectSettings ();

        /// set all values to default
        void SetDefault();

        ///
        void SetVerboseLevel (BF_VerboseLevel iLevel);
        ///
        BF_VerboseLevel GetVerboseLevel ();
        ///
        void SetStopLevelOnFatal (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnFatal ();
        ///
        void SetStopLevelOnError (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnError ();
        ///
        void SetStopLevelOnWarning (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnWarning ();
        ///
        const wxString& GetBackupLogLocation ();
        ///
        void SetBackupLogLocation (const wxChar* strFile);

        ///
        bool Serialize (jbSerialize& rA);
};    // class BFProjectSettings

#endif    // BFPROJECTSETTINGS_H
