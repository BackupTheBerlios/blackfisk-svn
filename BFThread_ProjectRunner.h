/**
 * Name:        BFThread_ProjectRunner.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-28
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

class BFTask;

///
class BFThread_ProjectRunner : public wxThread
{
    private:
        ///
        BFTask*         pTask_;
        ///
        wxMutex*        pMutex_;
        ///
        wxCondition*    pCondition_;
        ///
        BF_StopLevel    stopAnswer_;

        ///
        static BFThread_ProjectRunner* sp_project_runner_;

        /// ctor
        BFThread_ProjectRunner (BFTask* pTask);

        /// default ctor
        BFThread_ProjectRunner () {};

    public:
        ///
        static BFThread_ProjectRunner* CurrentlyRunning ()
        { return sp_project_runner_; }

        ///
        static bool Run (BFTask* pTask);

        /// virtual dtor
        virtual ~BFThread_ProjectRunner ();

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
