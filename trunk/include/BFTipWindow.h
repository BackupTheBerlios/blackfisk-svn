/**
 * Name:        BFTipWindow.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-05-27
 * Copyright:   (c) 2009 Christian Buhtz <blackfisk@web.de>
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


#ifndef BFTIPWINDOW_H
#define BFTIPWINDOW_H

#include <wx/timer.h>
#include <wx/tipwin.h>

/// 
class BFTipWindow : public wxTipWindow
{
	private:
		///
		static BFTipWindow*	psInstance_;

		///
		wxTimer				timer_;

        /// ctor
        BFTipWindow ();

		///
		BFTipWindow (wxWindow* pParent, const wxString& strTip);

    public:
		///
		static void CreateTipWindow (wxWindow* pParent, const wxString& strTip);
		///
		static void DeleteTipWindow ();

        /// virtual dtor
        virtual ~BFTipWindow ();

		///
		void OnTimer_Autopop (wxTimerEvent& rEvent);
		///
		//void OnMouseMotion (wxMouseEvent& rEvent);

		/** The amount of time (in miliseconds) a pointer must remain stationary
			within a tool's bounding rectangle before the ToolTip
			window appears. */
		static int GetInitialTime ();
		/** The amount of time (in miliseconds) a ToolTip window remains visible
			if the pointer is stationary within a tool's bounding rectangle. */
		static int GetAutopopTime ();

	//DECLARE_EVENT_TABLE();
};

#endif
