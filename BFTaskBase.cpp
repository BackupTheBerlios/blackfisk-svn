/**
 * Name:        BFTaskBase.cpp
 * Purpose:     BFTaskBase class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFTaskBase.h"
#include <wx/wx.h>

BFTaskBase::BFTaskBase ()
{
}


/*virtual*/ BFTaskBase::~BFTaskBase ()
{
}


/*static*/ wxString& BFTaskBase::FillBlackfiskPlaceholders (wxString& rStr)
{
    // BFTASK_PLACEHOLDER_DATE
    rStr.Replace(BFTASK_PLACEHOLDER_DATE, Core().GetDateString());

    // BFTASK_PLACEHOLDER_TIME
    rStr.Replace(BFTASK_PLACEHOLDER_TIME, Core().GetTimeString());

    return rStr;
}
