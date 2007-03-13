/**
 * BFBackupCtrl.h
 * definition of class BFBackupCtrl
 * <comment>
 * 2006-06-12
 ***/

#ifndef BFBACKUPCTRL_H
#define BFBACKUPCTRL_H

#include <wx/panel.h>

/// contains the treeCtrl, toolbar, etc on the left side of the splitter
class BFBackupCtrl : public wxPanel
{
    private:

    protected:

    public:
        /// constructor
        BFBackupCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupCtrl ();

};    // class BFBackupCtrl

#endif    // BFBACKUPCTRL_H
