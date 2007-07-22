/**
 * Name:        BFLogViewDlg.h
 * Purpose:     BFLogViewDlg class defination
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-21
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***/

#ifndef BFLOGVIEWDLG_H
#define BFLOGVIEWDLG_H

#include <wx/dialog.h>

///
class BFLogViewDlg : public wxDialog
{
    private:
        ///
        void Init ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFLogViewDlg (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFLogViewDlg ();

        ///
        void OnClose (wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};    // class BFLogViewDlg

#endif    // BFLOGVIEWDLG_H
