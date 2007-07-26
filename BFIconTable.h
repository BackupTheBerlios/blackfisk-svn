/**
 * Name:        BFIconTable.h
 * Purpose:     BFIconTable class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-27
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

#ifndef BFICONTABLE_H
#define BFICONTABLE_H

#include <wx/wx.h>
#include <wx/imaglist.h>

///
class BFIconTable : public wxImageList
{
    private:
        ///
        static BFIconTable* psIconTable_;

        ///
        BFIconTable ();
        ///
        virtual ~BFIconTable ();
        ///
        void Init ();

    public:
        ///
        static BFIconTable* Instance ();

        ///
        enum BFIconId
        {
            logo,
            volume_harddisk,
            volume_floppy,
            volume_cdrom,
            volume_removable,
            folder,
            folder_open,
            folder_virtual,
            folder_virtual_open,
            task_dircopy,
            task_filecopy,
            task_zip,
            task_sync
        };
};    // class BFIconTable

#endif    // BFICONTABLE_H
