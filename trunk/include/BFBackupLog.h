/**
 * Name:        BFBackupLog.h
 * Purpose:     BFBackupLog class definition
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


#ifndef BFBACKUPLOG_H
#define BFBACKUPLOG_H

#include "ObserverPattern.h"
#include <wx/datetime.h>
#include <wx/arrstr.h>
#include <vector>
#include "BFTask.h"
#include "BFSystem.h"

/** handle messages while the backup process and
    store them to the related backup-log-files
    what is loged and what not depends on the project
    settings */
class BFTaskLog
{
    private:
        /// task start
        wxDateTime          timeStart_;
        /// task end
        wxDateTime          timeEnd_;
        ///
        wxArrayString       arrLog_;
        ///
        long                lCountWarnings_;
        ///
        long                lCountErrors_;
        ///
        long                lCountFatal_;
        ///
        bool                bNormalEnd_;
        ///
        BFTask&             rTask_;

    public:
        ///
        BFTaskLog (BFTask& rTask);
        ///
        virtual ~BFTaskLog ();

        ///
        void Started ();
        ///
        void Finished (bool bNormalEnd = true);

        ///
        void Message (BFMessageType type,
                      const wxDateTime& timestamp,
                      const wxChar* strMessage,
                      const wxChar* strLocation);
        ///
        bool Write ();

        ///
        wxString GetSummary ();
        ///
        wxString GetFileName ();
};

///
typedef std::vector<BFTaskLog*> BFTaskLogVector;

///
class BFBackupLog : public Observer
{
    private:
        /// backup start
        wxDateTime      timeStart_;
        /// backup end
        wxDateTime      timeEnd_;
        ///
        BFTaskLogVector vecTaskLogs_;
        ///
        bool            bIsSaved_;

        ///
        bool WhileBackup ();
        /// write all informations about the backup and its task to the log-files
        bool Write ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFBackupLog ();
        /// virtual destructor
        virtual ~BFBackupLog ();

        /// mark the beginning of a backup process
        void BackupStarted ();
        /// mark the end of a backup process
        void BackupFinished ();

        ///
        BFTaskLog* GetCurrentTaskLog ();

        ///
        void TaskStarted (BFTask& rTask);
        ///
        void TaskFinished ();
        ///
        void TaskStoped ();

        ///
        virtual void ValueChanged (Subject* pSender);
};    // class BFBackupLog

#endif    // BFBACKUPLOG_H
