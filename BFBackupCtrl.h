/**
 * BFBackupCtrl.h
 * definition of class BFBackupCtrl
 * <comment>
 * 2006-06-12
 ***/

#ifndef BFBACKUPCTRL_H
#define BFBACKUPCTRL_H

class BFBackupTree;

#include <wx/panel.h>

/// contains the treeCtrl, toolbar, etc on the left side of the splitter
class BFBackupCtrl : public wxPanel
{
    private:
        ///
        BFBackupTree*   pBackupTree_;

    protected:

    public:
        /// constructor
        BFBackupCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupCtrl ();

        ///
        BFBackupTree* BackupTree ();
};    // class BFBackupCtrl

#endif    // BFBACKUPCTRL_H
