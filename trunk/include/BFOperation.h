/**
 * Name:        BFOperation.h
 * Purpose:     BFOperation class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-23
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

#ifndef BFOPERATION_H
#define BFOPERATION_H

#include <vector>
#include "Progress.h"
#include "BFTaskBase.h"
#include "jbSerialize.h"

// forwarde declarations
class BFTask;

/** Represent a backup task in the application-layer. */
class BFOperation
{
    private:
        ///
        BFTask*         pTask_;

        ///
        wxArrayString   arrSource_;
        ///
        wxArrayString   arrDestination_;
        
        ///
        bool RunForFileCopy(ProgressWithMessage& rProgress);
		///
        bool RunForArchive (ProgressWithMessage& rProgress);
		///
        bool RunForDirSync (ProgressWithMessage& rProgress);
        ///
        bool RunForDirCopy (ProgressWithMessage& rProgress);

        ///
        bool bStopOperation_;

    public:
        /// constructor
        BFOperation (BFTask* pTask);

        /// virtual destructor
        virtual ~BFOperation ();

        ///
        BFTask* Task ();

        /** create a new clean BFOperation object
            with a oid depending on the current OBRootTask instance
            (without storeing it to the task list in the current root task)
            and return a pointer to it *
        static BFOperation* CreateNew ();*/

        ///
        bool Run (ProgressWithMessage& rProgress);
        ///
        void StopOperation ();
};

///
typedef std::vector<BFOperation*>       BFOperationVector;
typedef BFOperationVector::iterator     BFOperationVectorIt;


#endif    // BFTASK_H
