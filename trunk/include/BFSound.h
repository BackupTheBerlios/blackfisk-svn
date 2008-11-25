/**
 * Name:        BFSound.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-11-25
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#ifndef BFSOUND_H
#define BFSOUND_H

///
class BFSound
{
    private:
        /// ctor
        BFSound ();

    public:
        /// virtual dtor
        virtual ~BFSound ();

        ///
        static void Info ();
        ///
        static void Warning ();
        ///
        static void Error ();
        ///
        static void Fatal ();
        ///
        static void Finish ();
};

#endif
