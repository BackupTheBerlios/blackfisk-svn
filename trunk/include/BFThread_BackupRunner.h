/**
 * Name:        BFThread_BackupRunner.h
 * Purpose:
 * Author:      Christian Buhtz
 * Created:     2007-09-28
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


#ifndef BFTHREAD_PROJECTRUNNER_H
#define BFTHREAD_PROJECTRUNNER_H

#include <wx/thread.h>
#include <wx/event.h>

#include "BFProjectSettings.h"

#ifdef __MINGW32__
    extern const wxEventType BF_EVENT_THREAD_END;
#else
    BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(BF_EVENT_THREAD_END, -1)
    END_DECLARE_EVENT_TYPES()
#endif

// forwarde declarations
class BFOperation;

///
class BFThread_BackupRunner : public wxThread
{
    private:
        ///
        BFOperation*    pOperation_;
        ///
        wxCondition*    pCondition_;
        ///
        BF_StopLevel    stopAnswer_;

        ///
        static BFThread_BackupRunner* sp_project_runner_;

        /// ctor
        BFThread_BackupRunner (BFOperation* pOperation);

        /// default ctor
        BFThread_BackupRunner () {};

    public:
        ///
        static BFThread_BackupRunner* CurrentlyRunning ()
        { return sp_project_runner_; }

        ///
        static bool Run (BFOperation* pOperation);

        /// virtual dtor
        virtual ~BFThread_BackupRunner ();

        /// thread execution starts here
        virtual void *Entry();

        /** If an error occures while backup and the user
            need to be asked what is to do,
            The backup-worker-thread can not ask the user
            himself because he is not alowed to call the GUI.
            The backup-worker-thread send an event to the
            main-thread and the main-thread ask the user
            and store the answer with this methode */
        void SetUsersStopAnswer (BF_StopLevel stop);
        ///
        BF_StopLevel GetUsersStopAnswer ();

};
#endif    // BFTHREAD_PROJECTRUNNER_H
