/**
 * OBBackupCtrl.h
 * definition of class OBBackupCtrl
 * <comment>
 * 2006-06-12
 ***/

#ifndef OBBACKUPCTRL_H
#define OBBACKUPCTRL_H

#include <wx/panel.h>

/// contains the treeCtrl, toolbar, etc on the left side of the splitter
class OBBackupCtrl : public wxPanel
{
    private:

    protected:

    public:
        /// constructor
        OBBackupCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~OBBackupCtrl ();

};    // class OBBackupCtrl

#endif    // OBBACKUPCTRL_H
