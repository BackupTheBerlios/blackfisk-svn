/**
 * Name:        BFSettings.cpp
 * Purpose:     BFSettings class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
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

#include "BFSettings.h"


/*static*/ BFSettings BFSettings::sSettings_;

//
BFSettings::BFSettings ()
          : bReplaceMacros_(false),
            bWithFiles_(false),
            bOpenLastProject_(false),
            lMaxLogFileSize_(1024),
            verboseLog_(MsgINFO)
{
}


//
/*virtual*/ BFSettings::~BFSettings ()
{
    int i = 0;
}


BFProjectSettings& BFSettings::GetDefaultProjectSettings ()
{
    return defaultPrj_;
}

bool BFSettings::GetReplaceMacros ()
{
    return bReplaceMacros_;
}

void BFSettings::SetReplaceMacros (bool bReplace)
{
    bReplaceMacros_ = bReplace;
}

bool BFSettings::GetWithFiles ()
{
    return bWithFiles_;
}

void BFSettings::SetWithFiles (bool bWithFiles)
{
    bWithFiles_ = bWithFiles;
}

long BFSettings::GetMaxLogFileSize ()
{
    return lMaxLogFileSize_;
}

void BFSettings::SetMaxLogFileSize (long lSizeInKiloByte)
{
    if (lSizeInKiloByte < 0)
        lMaxLogFileSize_ = 0;
    else
        lMaxLogFileSize_ = lSizeInKiloByte;
}

const wxArrayString& BFSettings::GetLastProjects ()
{
    return arrLastProjects_;
}

void BFSettings::SetLastProject (const wxChar* strFilename)
{
    if (strFilename == NULL)
        return;

    int idx = arrLastProjects_.Index(strFilename);

    if (idx != wxNOT_FOUND)
        arrLastProjects_.RemoveAt(idx);

    if (arrLastProjects_.GetCount() == 4)
        arrLastProjects_.RemoveAt(0);

    arrLastProjects_.Add(strFilename);
}

void BFSettings::SetOpenLastProject (bool bOpen)
{
    bOpenLastProject_ = bOpen;
}

bool BFSettings::GetOpenLastProject ()
{
    return bOpenLastProject_;
}

void BFSettings::SetVerboseLevelLog (BFMessageType lvl)
{
    verboseLog_ = lvl;
}

BFMessageType BFSettings::GetVerboseLevelLog ()
{
    return verboseLog_;
}
/*
void BFSettings::SetVerboseLevelMsg (BFMessageType lvl)
{
    verboseMsg_ = lvl;
}

BFMessageType BFSettings::GetVerboseLevelMsg ()
{
    return verboseMsg_;
}*/


bool BFSettings::Serialize (jbSerialize& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    rA.EnterObject();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        defaultPrj_.Serialize(rA);
        rA << bReplaceMacros_;
        rA << bWithFiles_;
        rA << (wxUint32&)lMaxLogFileSize_;
        rA << arrLastProjects_;
        rA << bOpenLastProject_;
        rA << (int)verboseLog_;
        //rA << (int)verboseMsg_;
    }
    else
    // ** serialize FROM file **
    {
        defaultPrj_.Serialize(rA);
        rA >> bReplaceMacros_;
        rA >> bWithFiles_;
        rA >> (wxUint32&)lMaxLogFileSize_;
        rA >> arrLastProjects_;
        rA >> bOpenLastProject_;
        rA >> (int&)verboseLog_;
        //rA >> (int&)verboseMsg_;
    }

    rA.LeaveObject();

    return true;
}
