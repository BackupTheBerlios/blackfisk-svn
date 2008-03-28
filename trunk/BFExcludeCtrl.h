/**
 * Name:        BFExcludeCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-03-27
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#ifndef BFEXCLUDECTRL_H
#define BFEXCLUDECTRL_H

#include <wx/panel.h>

//
class wxTextCtrl;
class wxListBox;
class BFTask;

///
class BFExcludeCtrl : public wxPanel
{
    private:
        ///
        wxTextCtrl*     pPath_;
        ///
        wxListBox*      pListPaths_;
        /** the related BFTask-Object to recieve
            the affected source directory */
        BFTask*         pTask_;

    public:
        /// ctor
        BFExcludeCtrl (wxWindow* pParent, BFTask* pTask);

        /// virtual dtor
        virtual ~BFExcludeCtrl ();

        /** write from 'pListPaths_' to 'rExclude'
            and clear 'rExclude' before */
        void SetData (wxArrayString& rExclude);
        /** read 'rExclude' to 'pListPaths_'
            and clear 'pListPaths_' before */
        void GetData (const wxArrayString& rExclude);

        ///
        void OnButtonAdd (wxCommandEvent& rEvent);
        ///
        void OnDoubleClick (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};

#endif    // BFEXCLUDECTRL_H
