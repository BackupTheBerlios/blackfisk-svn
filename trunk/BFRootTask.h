/**
 * Name:        BFRootTask.h
 * Purpose:     BFRootTask class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
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

#ifndef BFROOTTASK_H
#define BFROOTTASK_H

#include <wx/wx.h>

#include "BFTaskBase.h"
#include "BFTask.h"
#include "ObserverPattern.h"
#include "BFProjectSettings.h"

/// data layer
class BFRootTask : public Subject
{
    private:
        ///
        BFRootTask (BFRootTask&);
        ///
        BFRootTask ();

        ///
        static BFRootTask sRootTask_;

        /// name of the RootTask; only for identification by the user
        wxString            strName_;
        /// vector with all backup-tasks
        BFTaskVector        vecTasks_;
        ///
        BFProjectSettings   projectSettings_;

        /** indicates that the object or child of it has
            data that was not stored this time */
        bool                bModified_;
        /// the last created oid
        BFoid               oidLast_;



        /** it delete all task objects in the task-vector
            ATTENTION: it does not check if the project is saved !*/
        void ClearTaskVector ();

        /** return the position in the task-vector of the specified BFTask
            return -1 if nothing is found */
        long FindTask (BFTask* pTask);

        /** return the position in the task-vector of the last BFTask
            with the destination
            return -1 if nothing is found */
        long FindLastTaskWithDestination(const wxChar* destination);

    public:
        ///
        static BFRootTask& Instance ();
        /// virtual dtor
        virtual ~BFRootTask();

        /** add all elements (BFTask*) from 'vecTasks_' in
            'rVec' and return 'rVec'.
            The methode doesn't cleaning 'rVec' before
            filling it. */
        BFTaskVector& GetAllTasks (BFTaskVector& rVec);

        ///
        bool Serialize (jbSerialize& rA);
        ///
        bool Has (BFProjectSettings* pPrjSet);

        ///
        long GetTaskCount ();

        ///
        void SetName (const wxChar* strName);
        ///
        const wxChar* GetName ();
        ///
        BFProjectSettings& GetSettings ();

        /** create a new task and return its oid
            if an error occures it returns OBInvalidOID
            this methode does not broadcast the observers! */
        BFoid AppendTask (BFTaskType type,
                          const wxChar* strSource,
                          const wxChar* strDestination,
                          const wxChar* strName,
                          bool bVerify,
                          wxArrayString* pArrExclude = NULL,
                          BFArchiveFormat archive = CompressNOTUSED);
        /** add an existing task to root and return its oid
            if the object is not plausible or just exists in root
            OBInvalidOID returns */
        BFoid AppendTask (BFTask& rTask);
        ///
        bool HasTask(BFoid oid);
        ///
        BFTask* GetTask(BFoid oid);
        ///
        bool DeleteTask (BFoid oid);

        /** search in 'vecTasks_' for 'pTask'
            and return the BFTask pointer behind it
            if 'pTask' is not found in the vector
            or 'pTask' is the last element
            the first BFTask pointer is returned */
        BFTask* GetNextTask (BFTask* pTask);
        ///
        BFTask* GetLastTask ();

        ///
        bool IsModified ();
        ///
        void SetModified (bool bModified = true);


        /// create unique oid's for
        BFoid CreateOID ();

        /// return the destinations of each task as a string array
        wxArrayString GetDestinations ();

        ///
        void Close ();
};  // class BFRootTask

#endif    // BFROOTTASK_H
