/**
 * Name:        BFTipWindow.cpp
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


#include "BFTipWindow.h"

#include "ids.h"

#include <wx/settings.h>

// XXX
#include "BFSound.h"

#define BF_TIPWINDOW_ID_TIMER_AUTOPOP		1 + BF_TIPWINDOW_ID_HIGHEST
/*
BEGIN_EVENT_TABLE(BFTipWindow, wxTipWindow)
	EVT_MOTION	(						BFTipWindow::OnMouseMotion)
END_EVENT_TABLE()*/

/*static*/ BFTipWindow* BFTipWindow::psInstance_ = NULL;

BFTipWindow::BFTipWindow (wxWindow* pParent, const wxString& strTip)
		   : wxTipWindow (pParent, strTip),
		     timer_(this, BF_TIPWINDOW_ID_TIMER_AUTOPOP)
{
	// timer event
    Connect
    (
        BF_TIPWINDOW_ID_TIMER_AUTOPOP,
        wxEVT_TIMER,
        wxTimerEventHandler(BFTipWindow::OnTimer_Autopop),
        NULL,
        this
    );

    timer_.Start ( GetAutopopTime(), wxTIMER_ONE_SHOT );

	if ( HasCapture() )
	{
		BFSound::Fatal();
		//ReleaseMouse();
	}
	//CaptureMouse();
}

BFTipWindow::BFTipWindow ()
		   : wxTipWindow (NULL, wxEmptyString)
{
	//ReleaseMouse();
}


/*virtual*/ BFTipWindow::~BFTipWindow ()
{
	psInstance_ = NULL;
}

void BFTipWindow::OnTimer_Autopop (wxTimerEvent& rEvent)
{
	BFSound::Error();
	DeleteTipWindow();
}

/*void BFTipWindow::OnMouseMotion (wxMouseEvent& rEvent)
{
	BFSound::Fatal();
	DeleteTipWindow();
}*/

/*static*/ void BFTipWindow::CreateTipWindow (wxWindow* pParent, const wxString& strTip)
{
	DeleteTipWindow();

	psInstance_ = new BFTipWindow(pParent, strTip);
}

/*static*/ void BFTipWindow::DeleteTipWindow ()
{
	if ( psInstance_ )
		psInstance_->Destroy();
}


/*static*/ int BFTipWindow::GetInitialTime ()
{
	return wxSystemSettings::GetMetric (wxSYS_DCLICK_MSEC);
}

/*static*/ int BFTipWindow::GetAutopopTime ()
{
	return GetInitialTime() * 10;
}
