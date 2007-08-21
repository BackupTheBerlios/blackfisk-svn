/**
 * Name:        BFBackupQuestionDlg.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-29
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


#ifndef BFBACKUPQUESTIONDLG_H
#define BFBACKUPQUESTIONDLG_H

#include "BFSystem.h"
#include "BFProjectSettings.h"
#include <wx/dialog.h>

#define BF_BTNID_STOPTASK   1
#define BF_BTNID_STOPPRJ    2
#define BF_BTNID_IGNORE     3

/** a message dialog that display messages while a backup
    and aks (stop task, stop prj, ignore) what is to do */
class BFBackupQuestionDlg : public wxDialog
{
    private:
        /// ctor
        BFBackupQuestionDlg (const wxString& strMsg, BFMessageType type);

    protected:
        /// proteced members

    public:
        /// virtual dtor
        virtual ~BFBackupQuestionDlg ();

        ///
        void OnButton (wxCommandEvent& rEvent);

        ///
        static BF_StopLevel Ask (const wxString& strMsg, BFMessageType type);

    DECLARE_EVENT_TABLE();
};

#endif    // BFBACKUPQUESTIONDLG_H
