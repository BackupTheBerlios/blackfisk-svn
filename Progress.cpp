/**
 * Name:        Progress.cpp
 * Purpose:     Progress, ProgressWithMessage, ProgressTotal class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2002-05-28
 * Copyright:   (c) 2002 Christian Buhtz <exsudat@gmx.de>
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
          actual_(0)
{
}


/*virtual*/ Progress::~Progress()
{
}


void Progress::SetRange (long end)
{
    end_ = end;

	//SetActual(lBEGIN_);
}


bool Progress::SetActual (long actual)
{
	if (actual > end_ || actual < lBEGIN_)
        return false;

	actual_ = actual;
    broadcastObservers();

	return true;
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


bool ProgressWithMessage::IncrementActualWithMessage (const wxChar* message)
{
    if (message != NULL)
        strMessage_ = message;

    return Progress::IncrementActual();
}


const wxChar* ProgressWithMessage::GetMessage ()
{
    return strMessage_;
}


const wxChar* ProgressWithMessage::GetLabel ()
{
    return strLabel_;
}


void ProgressWithMessage::SetLabel (const wxChar* label)
{
    if (label == NULL)
        return;

    strLabel_ = label;
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


/*virtual*/ bool ProgressTotal::IncrementActual ()
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


