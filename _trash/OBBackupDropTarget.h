/**
 * OBBackupDropTarget.h
 * definition of class MyDropTarget
 * <comment>
 * 2006-06-20
 ***/

#ifndef OBBACKUPDROPTARGET_H
#define OBBACKUPDROPTARGET_H

#include <wx/dnd.h>

class OBBackupTree;

///
class OBBackupDropTarget : public wxFileDropTarget
{
    private:
        ///
        OBBackupTree* pBackupTree_;


    public:
        /// constructor
        OBBackupDropTarget (OBBackupTree* pBackupTree);

        /// virtual destructor
        virtual ~OBBackupDropTarget ();

        ///
        virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
        ///
        virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
};    // class OBBackupDropTarget

#endif    // OBBACKUPDROPTARGET_H
