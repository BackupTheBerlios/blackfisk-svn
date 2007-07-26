/**
 * Name:        BFDestinationDlg.h
 * Purpose:     BFDestinationDlg class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-28
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

#ifndef BFDESTINATIONDLG_H
#define BFDESTINATIONDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>

#define BFDESTINATIONDLG_ID_BUTTONOK        1 + wxID_HIGHEST
#define BFDESTINATIONDLG_ID_BUTTONCANCEL    2 + wxID_HIGHEST

class BFDestinationCtrl;

///
class BFDestinationDlg : public wxDialog
{
    private:
        ///
        BFDestinationCtrl*  pDestCtrl_;

        ///
        BFDestinationDlg ();

    public:
        /// constructor
        BFDestinationDlg (wxWindow* pParent, const wxChar* strPath = wxEmptyString);

        /// virtual destructor
        virtual ~BFDestinationDlg ();

        ///
        void OnClose (wxCloseEvent& event);
        ///
        void OnButton_Ok (wxCommandEvent& rEvent);
        ///
        void OnButton_Cancel (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};    // class BFDestinationDlg

#endif    // BFDESTINATIONDLG_H
