/**
 * Name:        BFHelpCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-12
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


#ifndef BFHELPCTRL_H
#define BFHELPCTRL_H

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

/** a control that display help text related to another control
    that is under the mouse courser
    BFHelpCtrl handle the mouse-motion event and the displaying
    of the related help text */
class BFHelpCtrl : public wxPanel
{
    private:
        ///
        wxStaticText*   pHelpText_;
        /// the last window its help text was displayed
        wxWindow*       pLastWindow_;

    protected:
        /// proteced members

    public:
        /// ctor
        BFHelpCtrl (wxWindow* pParent, int iMaxHelpLines = 3);

        /// virtual dtor
        virtual ~BFHelpCtrl ();

        /// handle mouse-motion events
        void OnMotion (wxMouseEvent& rEvent);
        /// conect 'pWindow' to 'OnMotion' on mouse-motion-events
        void ConnectMotionEvent (wxWindow* pWindow);
};

#endif    // BFHELPCTRL_H
