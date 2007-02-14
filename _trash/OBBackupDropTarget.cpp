/**
 * implementation of class OBBackupDropTarget
 * 2006-06-20
 ***/

#include "OBBackupDropTarget.h"
#include "OBMainFrame.h"
#include "OBApp.h"
#include "OBBackupTree.h"

OBBackupDropTarget::OBBackupDropTarget (OBBackupTree* pBackupTree)
            : pBackupTree_(pBackupTree)
{
}

/*virtual*/ OBBackupDropTarget::~OBBackupDropTarget ()
{
}

/*virtual*/ wxDragResult OBBackupDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    /*if (pBackupTree_ != NULL)
        pBackupTree_->SelectItem( pBackupTree_->HitTest(wxPoint(x, y)) );*/

    return def;
}

/*virtual*/ bool OBBackupDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    if (pBackupTree_ == NULL)
        return false;

    return pBackupTree_->OnDropFiles(x, y, filenames);
}
