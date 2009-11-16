/**
 * Name:        BFIconTable.h
 * Purpose:     BFIconTable class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-27
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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
class BFIconTable
{
    private:
        ///
        static BFIconTable sIconTable_;

        /// filesystem icons for BFBackupTree
        wxImageList     imgFilesystem_;

        /// button icons
        wxImageList     imgButtons_;

        /// message type icons
        wxImageList     imgMessage_;

        ///
        BFIconTable ();
        ///
        virtual ~BFIconTable ();
        ///
        void Init ();

    public:
        ///
        static BFIconTable& Instance ();

        ///
        enum BFFilesystemIconId
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
            task_sync,
            _filesystem_unused_ // to check for invalid values
        };

        ///
        enum BFButtonIconId
        {
            yes,
            no,
            ok,
            cancel,
            stop_prj,
            stop_task,
            ignore,
            _button_unused_ // to check for invalid values
        };

        ///
        enum BFMessageIconId
        {
            question,
            info,
            warning,
            error,
            fatal,
            _message_unused_ // to check for invalid values
        };

        /** Return a wxIcon object based on an ico-file
            with multible bitmaps in it. */
        static wxIcon GetMainFrameIcon ();

        ///
        wxImageList* GetFilesystemImageList ();

        ///
        static wxBitmap GetBitmap (BFButtonIconId id);
        ///
        static wxBitmap GetBitmap (BFMessageIconId id);
        ///
        static wxBitmap GetBitmap (BFFilesystemIconId id);
        ///
        static wxIcon GetIcon (BFButtonIconId id);
        ///
        static wxIcon GetIcon (BFMessageIconId id);
        ///
        static wxIcon GetIcon (BFFilesystemIconId id);

};

#endif    // BFICONTABLE_H
