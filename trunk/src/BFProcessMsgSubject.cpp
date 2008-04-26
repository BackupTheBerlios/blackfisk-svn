/**
 * Name:        BFProcessMsgSubject.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-28
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
 ***


#include "BFProcessMsgSubject.h"

//
BFProcessMsgSubject::BFProcessMsgSubject (const wxString& strCaption /*= wxEmptyString*,
                                          const wxString& strMsg /*= wxEmptyString*)
                   : strCaption_(strCaption),
                     strMsg_(strMsg)

{
}


//
/*virtual* BFProcessMsgSubject::~BFProcessMsgSubject ()
{
}

void BFProcessMsgSubject::SetCaption (const wxString& strCaption)
{
    strCaption_ = strCaption;
    broadcastObservers();
}

void BFProcessMsgSubject::SetMsg (const wxString& strMsg)
{
    strMsg_ = strMsg;
    broadcastObservers();
}

wxString BFProcessMsgSubject::GetCaption ()
{
    return strCaption_;
}

wxString BFProcessMsgSubject::GetMsg ()
{
    return strMsg_;
}

*/
