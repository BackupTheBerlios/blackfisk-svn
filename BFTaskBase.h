/**
 * Name:        BFTaskBase.h
 * Purpose:     BFTaskBase class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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

#ifndef BFTASKBASE_H
#define BFTASKBASE_H

#include "BFCore.h"


#define BFTASK_MACRO_DATE   _T("<date>")

/// lowest base class for all Task classes
class BFTaskBase
{
    private:

    protected:
        /// reference to the BFCore instance
        static BFCore& Core ()
        { return BFCore::Instance(); }

    public:
        /// ctor
        BFTaskBase ();
        /// virtual dtor
        virtual ~BFTaskBase ();

        /** search for macros (for example "<date>") in 'rStr'
            and replace them with the needed value */
        static wxString& ReplaceMacros (wxString& rStr);
};  // class BFTaskBase


#endif    // BFTASKBASE_H
