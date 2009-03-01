/**
 * Name:        BFPRoject.h
 * Purpose:     BFProject class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-22
 *              (based on BFRootTask)
 * Copyright:   (c) 2009 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFPROJECT_H
#define BFPROJECT_H

#include <wx/wx.h>

//#include "BFTaskBase.h"
#include "BFTask.h"
#include "ObserverPattern.h"
#include "BFProjectSettings.h"

/** This class represent a backup project in the data layer. */
class BFProject : public Subject
{
    private:
        ///
        BFProject (BFProject&);
        ///
        BFProject ();

        ///
        static BFProject sProject_;

        /** The name of the Project.
            It is only for identification by the user. */
        wxString            strName_;

        /// vector with all backup-tasks
        BFTaskVector        vecTasks_;
        ///
        BFProjectSettings   settings_;

        /** indicates that the object or child of it has
            data that was not stored this time */
        bool                bModified_;

        /// the last created oid
        BFoid               oidLast_;

        /** it delete all task objects in the task-vector
            ATTENTION: it does not check if the project is saved !*/
        void ClearTaskVector ();

    public:
        ///
        static BFProject& Instance ();
        /// virtual dtor
        virtual ~BFProject();

        /** add all elements (BFTask*) from 'vecTasks_' in
            'rVec' and return 'rVec'.
            The methode doesn't cleaning 'rVec' before
            filling it. */
        BFTaskVector& GetAllTasks (BFTaskVector& rVec);

        ///
        bool Serialize (jbSerialize& rA);
        ///
        void Reset ();

        ///
        long GetTaskCount ();

        ///
        void SetName (const wxString& strName);
        ///
        const wxString& GetName ();

        ///
        BFProjectSettings& GetSettings ();

        /** Create a new task and return its oid.
            If an error occures it returns OBInvalidOID.
            This methode does not broadcast the observers! */
        BFoid AppendTask (BFTaskType type,
                          const wxString& strSource,
                          const wxString& strDestination,
                          const wxString& strName,
                          bool bVerify,
                          bool bVerifyContent,
                          wxArrayString* pArrExclude = NULL,
                          BFArchiveFormat archive = CompressNOTUSED);

        /** Add an existing task to the project and return its oid.
            If the object is not plausible or just exists in the project
            OBInvalidOID returns. */
        BFoid AppendTask (BFTask& rTask);
        ///
        bool HasTask(BFoid oid);
        ///
        BFTask* GetTask(BFoid oid);
        ///
        bool DeleteTask (BFoid oid, bool bBroadcast = true);
        ///
        void DeleteTasks (BFTaskVector& vecTasks);

        /** Search in 'vecTasks_' for 'pTask'
            and return the BFTask pointer behind it.
            If 'pTask' is not found in the vector
            or 'pTask' is the last element
            the first BFTask pointer is returned. */
        BFTask* GetNextTask (BFTask* pTask);
        ///
        BFTask* GetLastTask ();
        /** Return the position in the task-vector of the specified BFTask.
            Return -1 if nothing is found. */
        long FindTask (BFTask* pTask);

        ///
        bool IsModified ();
        ///
        void SetModified (bool bModified = true);


        /// Create unique oid's
        BFoid CreateOID ();

        /// Return the destinations of each task as a string array.
        wxArrayString GetAllDestinations ();

        /** Return the position in the task-vector of the last BFTask
            with the specified destination.
            Return -1 if nothing is found. */
        long FindLastTaskWithDestination(const wxString& strDestination);

        /** Find all tasks with placeholders in its string-members */
        BFTaskVector& FindAllTasksWithPlaceholders (BFTaskVector& rVec);

        ///
        static wxString GetTypeDescription (BFTaskType type,
                                            BFArchiveFormat format = CompressNOTUSED);
        ///
        static wxString GetArchiveExtension(BFArchiveFormat format);

};

#endif    // BFPROJECT_H
