/**
 * Name:        BFLogViewDlg.h
 * Purpose:     BFLogViewDlg class defination
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-21
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
