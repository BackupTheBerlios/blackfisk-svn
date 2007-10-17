/**
 * Name:        BFAboutDlg.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-08-19
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


#ifndef BFABOUTDLG_H
#define BFABOUTDLG_H

#include <wx/dialog.h>

///
class BFAboutDlg : wxDialog
{
    private:
        ///
        wxSizer* CreateThirdPartySizer (const wxChar* strUrl,
                                        const wxChar* strUrlLabel,
                                        const wxChar* strAuthor,
                                        const wxChar* strDesc);

    protected:
        /// proteced members

    public:
        /// ctor
        BFAboutDlg ();

        /// virtual dtor
        virtual ~BFAboutDlg ();

        ///
        void OnViewLicense (wxCommandEvent& rEvent);
        ///
        void OnOk (wxCommandEvent& rEvent);
        ///
        void OnClose (wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};

#endif    // BFABOUTDLG_H
