/**
 * BFIconTable.h
 * definition of class BFIconTable
 * <comment>
 * 2007-03-27
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
            task_zip
        };
};    // class BFIconTable

#endif    // BFICONTABLE_H
