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

        /** Used to identify and remember the original crontabline. */
        wxString            strOrgCrontabline_;
        /** Indicate if the project is in "retry mode". It means that
            the last automaticly run of the backup caused in an error
            and will no be retried periodicly.
            If this is 'true' means that the current run is a retry-run.
            If it succeded the crontab-line have to be restored to
            its original one.*/
        bool                bInRetryMode_;
        /** Used to identify and remember the retry crontabline. */
        wxString            strRetryCrontabline_;
        /// next retry: hours (please see 'lRetryMinutes_', too)
        long                lRetryHours_;
        /// next retry: minutes (please see 'lRetryHours_', too)
        long                lRetryMinutes_;

        /** it delete all task objects in the task-vector
            ATTENTION: it does not check if the project is saved !*/
        void ClearTaskVector ();

		/** Return the position of 'pTask' in the vector 'vecTasks_'.
			Return '-1' if nothing was found. */
		long BFProject::GetVectorPosition (const BFTask* pTask);

		/** Move the position of 'pTask' in the vector 'vecTasks_' between
			'idxFrom' and 'idxTo'. */
		void MoveTaskInVector (BFTask* pTask, long idxFrom, long idxTo);

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
        void SetOriginalCrontabLine (const wxString& strOrgLine);
        ///
        const wxString& GetOriginalCrontabLine ();

        ///
        void SetInRetryMode (bool bInRetry);
        ///
        bool IsInRetryMode ();

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
            If the task is not plausible or just exists in the project
            'OBInvalidOID' will be returns.
			The caller of that methode need to take care of 'rTaks' - maybe
			delete it. */
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

		/** This methode check the dependencies of a hypothetic backup
			task in the case of its source 'strSrc' and destination
			'strDest' directories.
			For example if the source directory ('strSrc') is the
			destination directory of another task, the other task
			should be executed before.
			Or if the destination directory ('strDest') is the source
			directory of another task, the other task should be executed
			after.
			If such dependencies cause in a paradoxon the methode will
			return 'false'. Otherwise it will return 'true'.
			The parameter 'rIdxAfter' is the position (in the task vector)
			after that such a task should be added.
			The parameter 'rIdxBefore' is the position (in the task vector)
			before that such a task should be added. */
		bool HasNoDependencyParadoxons (const wxString& strSrc,
										const wxString& strDest,
										long& rIdxAfter,
										long& rIdxBefore);

		/** This methode check if it is possible to modify the current destination
			of the task 'pTask' to 'strNewDestination'.
			If it is not possible 'false' is returned.
			If it is possible the position of the task in the global task vectore
			is modified if this is (because of task dependencies) needed and
			'true' is returned.
			This methode is called by BFTask::SetDestination(). */
		bool HandleNewDestination ( BFTask* pTask, wxString& strNewDestination );

        ///
        bool IsModified ();
        ///
        void SetModified (bool bModified = true);


        /// Create unique oid's
        BFoid CreateOID ();

        /// Return the destinations of each task as a string array.
        wxArrayString GetAllDestinations ();

		/// Return the sources of each task as a string array.
        wxArrayString GetAllSources ();

		/** Modify the destination of all tasks with the destination
			'strOldDestination' to 'strNewDestination'.
			Return 'true' if it was successfull.
			Return 'false' if it wasn't possible (e.g. because
			of dependencies paradoxons). */
        bool ModifyDestination (const wxString& strOldDestination,
                                const wxString& strNewDestination);

        /** Return the position in 'vecTasks_' of the last BFTask element
            which destination is the same or a parent of 'strDestination'.
            Return -1 if nothing is found. */
        long FindLastTaskWithDestination(const wxString& strDestination);

        /** Return the position in 'vecTasks_' of the last BFTask element
            which source is the same or a parent of 'strSource'.
            Return -1 if nothing is found. */
        long FindLastTaskWithSource(const wxString& strSource);

        /** Find all tasks with placeholders in its string-members */
        BFTaskVector& FindAllTasksWithPlaceholders (BFTaskVector& rVec);

        ///
        static wxString GetTypeDescription (BFTaskType type,
                                            BFArchiveFormat format = CompressNOTUSED);
        ///
        static wxString GetArchiveExtension(BFArchiveFormat format);

		///
		wxString GetDefaultProjectLocation ();
};

#endif    // BFPROJECT_H
