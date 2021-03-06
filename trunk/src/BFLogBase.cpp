/**
 * Name:        BFLogBase.cpp
 * Purpose:     BFLogBase class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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

#include "BFLogBase.h"
#include "BFSystem.h"
#include "BFCore.h"

BFLogBase::BFLogBase (const wxString& strLogFileName, long lMaxSizeInBytes /*= 0*/ )
     : Observer(&(BFSystem::Instance())),
       lMaxSizeInB_(lMaxSizeInBytes),
       strLogFileName_(strLogFileName),
       fileLog_(strLogFileName, wxFile::write_append)
{
}

/*virtual*/ BFLogBase::~BFLogBase ()
{
}

long BFLogBase::GetMaxSizeInB ()
{
    return lMaxSizeInB_;
}

void BFLogBase::SetMaxSizeInB (long lMaxSizeInBytes)
{
    lMaxSizeInB_ = lMaxSizeInBytes;
}

void BFLogBase::CareSize ()
{
    // the size does not matter
    if (GetMaxSizeInB() <= 0)
        return;

    if ( !(fileLog_.IsOpened()) )
        return;

    // check log-file size
    if (fileLog_.Length() > GetMaxSizeInB())
    {
        // close the file
        fileLog_.Close();
        // backup the file
        BFCore::Instance().MoveFile(strLogFileName_, strLogFileName_ + ".bak", true);
        // create a new empty file
        fileLog_.Open(strLogFileName_, wxFile::write_append);
    }
}
