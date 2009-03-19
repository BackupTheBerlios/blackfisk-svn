/**
 * Name:        Progress.cpp
 * Purpose:     Progress, ProgressWithMessage, ProgressTotal class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2002-05-28
 * Copyright:   (c) 2002 Christian Buhtz <blackfisk@web.de>
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

#include <wx/wx.h>
#include <wx/wxchar.h>
#include "Progress.h"


//const long Progress::lBEGIN_ = 0;

Progress::Progress()
        : lBEGIN_(0),
          end_(100),
          actual_(0),
          bLocked_(false),
          bDeadCountMode_(false),
          deadcounts_(0)
{
}


/*virtual*/ Progress::~Progress()
{
}

void Progress::SetDeadCountMode (bool bOn)
{
    bDeadCountMode_ = bOn;
}

void Progress::Lock ()
{
    bLocked_ = true;
}

void Progress::Unlock ()
{
    bLocked_ = false;
}

bool Progress::IsLocked ()
{
    return bLocked_;
}

void Progress::SetRange (long end)
{
    if (IsLocked())
        return;

    end_ = end;
}


bool Progress::SetActual (long actual)
{
	if (IsLocked() || actual > end_ || actual < lBEGIN_)
        return false;

	actual_ = actual;
    broadcastObservers();

	return true;
}

/*virtual*/ bool Progress::IncrementActual ()
{
    if (bDeadCountMode_)
    /* DEAD COUNT MODE */
    {
        ++deadcounts_;
    }
    else
    /* NORMALE MODE */
    {
        /* don't increment the normale count before
           deadcounts_ is still 0 */
        if (deadcounts_ > 0)
        {
            // decrement the deadcounts
            --deadcounts_;
            /* we need to return false to show the caller
               that no observer is broadcasted */
            return false;
        }
    }

    // temporary unlock because SetActual()
    // won't work if the progress object is locked
    bool bLock = IsLocked();
    Unlock();

    // increment
    bool rc = SetActual(GetActual() + 1);

    // reset the lock-status
    if (bLock)
        Lock();

    return rc;
}

bool Progress::IsEnd ()
{
    if (GetActual() >= end_)
        return true;
    else
        return false;
}


void Progress::DoEnd ()
{
    if (IsEnd())
        return;

    SetActual (end_);
}


long Progress::GetActual (void)
{
	return actual_;
}


long Progress::GetRange ()
{
    return end_ - lBEGIN_;
}


unsigned short Progress::GetProgress ()
{
	double x;

	x = end_ - lBEGIN_;
	x = 100 / x;

	return ( (unsigned short) (x * actual_) );
}



ProgressWithMessage::ProgressWithMessage ()
{
}


/*virtual*/ ProgressWithMessage::~ProgressWithMessage ()
{
}


bool ProgressWithMessage::IncrementActualWithMessage (const wxString& strMessage)
{
    SetMessage(strMessage, false);

    if (Progress::IncrementActual())
    {
        return true;
    }
    else
    {
        broadcastObservers();
        return false;
    }
}


const wxString& ProgressWithMessage::GetMessage ()
{
    return strMessage_;
}


const wxString& ProgressWithMessage::GetLabel ()
{
    return strLabel_;
}


void ProgressWithMessage::SetLabel (const wxString& strLabel)
{
    strLabel_ = strLabel;

    broadcastObservers();
}

void ProgressWithMessage::SetMessage (const wxString& strMessage, bool bBroadcast /*= true*/)
{
    strMessage_ = strMessage;

    if (bBroadcast)
        broadcastObservers();
}

ProgressTotal::ProgressTotal(long lProgressCount,
                             Progress* pSubProgress)
             : Observer(pSubProgress),
               lProgressCount_(lProgressCount),
               lProgressCurrent_(1)
{
}


/*virtual*/ ProgressTotal::~ProgressTotal()
{
}


bool ProgressTotal::IncrementActual ()
{
    ++lProgressCurrent_;

    if (lProgressCurrent_ > lProgressCount_)
        DoEnd();

    return true;
}


/*virtual*/ void ProgressTotal::ValueChanged (Subject* pSender)
{
    // init
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    // calculate new range
    SetRange(pP->GetRange() * lProgressCount_);

    // set new position in that range
    SetActual
    (
        ( pP->GetRange() * (lProgressCurrent_-1) ) + pP->GetActual()
    );
}


