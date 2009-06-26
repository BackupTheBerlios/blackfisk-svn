/**
 * Name:        BFIsWorkingCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-11
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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


#ifndef BFISWORKINGCTRL_H
#define BFISWORKINGCTRL_H

#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/timer.h>
#include <wx/stattext.h>
#include <vector>

/** A animated control that is just for the eyes and
    should suggest the user that something is going on. */
class BFIsWorkingCtrl : public wxPanel
{
    private:
        ///
        std::vector<wxStaticText*>              vecItems_;
        ///
        std::vector<wxStaticText*>::iterator    itCurrPos_;
        ///
        wxGridBagSizer*                     pGBSizer_;
        ///
        wxTimer*                            pTimer_;

        ///
        void CreatePositions (long lLength);
        ///
        void HighlightItem (long lFromCurrentPos, const wxString& str);

    protected:
        /// proteced members

    public:
        /// ctor
        BFIsWorkingCtrl (wxWindow* pParent, long lLength);

        /// virtual dtor
        virtual ~BFIsWorkingCtrl ();

        ///
        void OnTimer(wxTimerEvent& rEvent);
};

#endif    // BFISWORKINGCTRL_H
