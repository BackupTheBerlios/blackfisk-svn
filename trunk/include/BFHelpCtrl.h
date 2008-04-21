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

//
class wxStaticText;
class wxHelpProvider;

/** DESCRIPTION:
    A control that display help text of a wxWindow
    that is under the mouse courser.
    BFHelpCtrl handle the mouse-events "EnterWindow" and "LeaveWindow"
    and set the wxSimpleHelpProvider in the ctor.
    HOWTO:
    Set a global wxHelpProvider on startup of the application. The easiest
    way is to call BFHelpCtrl::InitHelpProvider().
    Then create a BFHelpCtrl. Use one of the three available
    Connect()-methodes of to connect the wxWindow to the BFHelpCtrl.
    Based on which Connect()-methode was called you need to call
    wxWindow::SetHelpText() explicitly.
    Then the help text will be displayed on the help-ctrl
    if the mouse is over the wxWindow. */
class BFHelpCtrl : public wxPanel
{
    private:
        ///
        wxStaticText*   pHelpText_;
        /// the last window its help text was displayed
        wxWindow*       pLastWindow_;
        /// mark that a call of OnEnterWindow() is running
        bool            bOnEnter_;

        ///
        static wxHelpProvider*  spHelpProvider_;

    protected:
        /// proteced members

    public:
        /// ctor
        BFHelpCtrl (wxWindow* pParent,
                    int iMaxHelpLines = 4);

        /// virtual dtor
        virtual ~BFHelpCtrl ();

        /** This install a global wxSimpleHelpProvider.
            Call this before any other call of wxWindow::SetHelpText()! */
        static void InitHelpProvider ();

        ///
        void OnEnterWindow (wxMouseEvent& rEvent);
        ///
        void OnLeaveWindow (wxMouseEvent& rEvent);

        /// conect 'pWindow' to 'OnEnterWindow()' and 'OnLeaveWindow()'
        void Connect (wxWindow* pWindow);
        /** Did the same as Connect(wxWindow*)
            but set the help text for the wxWindow, too. */
        void Connect (wxWindow* pWindow, const wxString& strHelpText);
        /// Please look on Connect (wxWindow*, const wxString&) for more detailes.
        void Connect (wxWindow* pWindow1, wxWindow* pWindow2, const wxString& strHelpText);
        /// Please look on Connect (wxWindow*, const wxString&) for more detailes.
        void Connect (wxWindow* pWindow1, wxWindow* pWindow2, wxWindow* pWindow3, const wxString& strHelpText);
};

#endif    // BFHELPCTRL_H
