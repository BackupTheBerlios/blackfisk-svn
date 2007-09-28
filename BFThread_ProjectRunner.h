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

class BFTask;

///
class BFThread_ProjectRunner : public wxThread
{
    private:
        ///
        BFTask* pTask_;

    protected:
        /// proteced members

    public:
        /// ctor
        BFThread_ProjectRunner (BFTask* pTask);

        /// virtual dtor
        virtual ~BFThread_ProjectRunner ();

        /// thread execution starts here
        virtual void *Entry();
};
#endif    // BFTHREAD_PROJECTRUNNER_H
