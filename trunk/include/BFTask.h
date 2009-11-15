/**
 * Name:        BFTask.h
 * Purpose:     BFTask class definition
 * Author:      Christian Buhtz
 * Created:     2006-04-28
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

#ifndef BFTASK_H
#define BFTASK_H

#include <vector>
#include "BFTaskBase.h"
#include "Progress.h"
#include "jbSerialize.h"


/** Represent a backup task in the data-layer. */
class BFTask
{
    private:
        /// unique object identifier
        BFoid                       oid_;
        /// task type
        BFTaskType                  type_;
        /// source
        wxString                    strSource_;
        /// destination path
        wxString                    strDestination_;
        /// destination name
        wxString                    strName_;
        ///
        bool                        bVerify_;
        ///
        bool                        bVerifyContent_;
        /** Specify the format of the archive if
            'type_' == TaskARCHIVE.
            If it is CompressNOTUSED it is not in use. */
        BFArchiveFormat             archiveFormat_;
        /// Files and sub-dirs to exclude from the backup.
        wxArrayString               arrExclude_;
        /** For synchronisation tasks. Delete files in
            destination directory if they don't exisit in source directory. */
        bool                        bRealSync_;

		/** Indicates that a call of BFTask::SetDestination() is running.
			It is needed for BFProject::HandleNewDestination().
			This member is not serialized! */
		bool						bCallSetDestination_;

        ///
        bool SetOID (BFoid oid);

    public:
        ///
        BFTask (BFTaskType type,
                const wxString& strSource,
                const wxString& strDestination,
                const wxString& strName,
                bool bVerify,
                bool bVerifyContent,
                BFArchiveFormat archive,
                wxArrayString& arrExclude,
                bool bRealSync);

        ///
        BFTask ();

        ///
        virtual ~BFTask ();

        /** Check if the object and it's data is valid.
            But it doesn't check if the directories are realy there.
            It just validate it to store it without creating inconsistence
            with other data. */
        bool IsValid ();

        /** Add all available task types to the vector
            with the current task type as first item. */
        void GetAvailableTypes (BFTypeVector& rVecType);
        ///
        wxString GetArchiveExtension();
        ///
        wxString GetTypeDescription ();

        ///
        BFoid GetOID () const;
        ///
        BFTaskType GetType () const;
        ///
        const wxString& GetSource () const;
        ///
        const wxString& GetDestination () const;
        ///
        const wxString& GetName () const;
        ///
        bool Verify () const;
        ///
        bool VerifyContent () const;
        ///
        BFArchiveFormat GetArchiveFormat () const;
        ///
        const wxArrayString& GetExclude () const;
        ///
        bool GetRealSync () const;

        ///
        void SetTaskType (BFTaskType type);
        ///
        void SetSource (const wxString& source);
        ///
        bool SetDestination (const wxString& dest);
        ///
        void SetName (const wxString& name);
        ///
        void SetVerify (bool verify);
        ///
        void SetVerifyContent (bool verify_content);
        ///
        void SetArchiveFormat (BFArchiveFormat archive);
        ///
        void SetExclude (const wxArrayString& exclude);
        ///
        void SetRealSync (bool bRealSync);

        /** The object will be serialized(write) TO 'rA'
            or
            a the object will be serialized(read) from 'rA'. */
        bool Serialize (jbSerialize& rA);

		/** Please see the member 'bCallSetDestination_' for more details. */
		bool IsWhileSetDestinationCall ();
};


#endif    // BFTASK_H
