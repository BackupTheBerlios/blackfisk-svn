/**
 * Name:        BFProjectSettings.cpp
 * Purpose:     BFProjectSettings class implementation
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

#include "BFProjectSettings.h"
#include "BFRootTask.h"
#include "BFApp.h"

//
BFProjectSettings::BFProjectSettings ()
{
    SetDefault();
}


//
/*virtual*/ BFProjectSettings::~BFProjectSettings ()
{
}

void BFProjectSettings::SetVerboseLevel (BF_VerboseLevel iLevel)
{
    iVerboseLevel_ = iLevel;
}

BF_VerboseLevel BFProjectSettings::GetVerboseLevel ()
{
    return iVerboseLevel_;
}

void BFProjectSettings::SetStopLevel (BF_StopLevel& iTarget, BF_StopLevel iStopLevel)
{
    iTarget = iStopLevel;
}


void BFProjectSettings::SetStopLevelOnFatal (BF_StopLevel iLevel)
{
    SetStopLevel (iStopLevelOnFatal_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnFatal ()
{
    return iStopLevelOnFatal_;
}

void BFProjectSettings::SetStopLevelOnError (BF_StopLevel iLevel)
{
    SetStopLevel (iStopLevelOnError_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnError ()
{
    return iStopLevelOnError_;
}

void BFProjectSettings::SetStopLevelOnWarning (BF_StopLevel iLevel)
{
    SetStopLevel(iStopLevelOnWarning_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnWarning ()
{
    return iStopLevelOnWarning_;
}

const wxString& BFProjectSettings::GetBackupLogLocation ()
{
    if (BFRootTask::Instance().Has(this))
    {
        if (strBackupLogLocation_.IsEmpty())
            strBackupLogLocation_ = BFApp::ExtractCommunity(BFRootTask::Instance().GetDestinations());

        if (strBackupLogLocation_.IsEmpty())
            if (BFRootTask::Instance().GetDestinations().GetCount() > 0)
                strBackupLogLocation_ = BFRootTask::Instance().GetDestinations()[0];
    }

    return strBackupLogLocation_;
}

void BFProjectSettings::SetBackupLogLocation (const wxChar* strFile)
{
    if (strFile == NULL)
        return;

    strBackupLogLocation_ = strFile;
}

bool BFProjectSettings::Serialize (jbSerialize& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    rA.EnterObject();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        rA << (int)iVerboseLevel_;
        rA << (int)iStopLevelOnFatal_;
        rA << (int)iStopLevelOnError_;
        rA << (int)iStopLevelOnWarning_;
        rA << strBackupLogLocation_;
    }
    else
    // ** serialize FROM file **
    {
        rA >> (int&)iVerboseLevel_;
        rA >> (int&)iStopLevelOnFatal_;
        rA >> (int&)iStopLevelOnError_;
        rA >> (int&)iStopLevelOnWarning_;
        rA >> strBackupLogLocation_;
    }

    rA.LeaveObject();

    return true;
}

void BFProjectSettings::SetDefault ()
{
    iVerboseLevel_          = BFVERBOSE_WARNING;
    iStopLevelOnFatal_      = BFDO_ASK;
    iStopLevelOnError_      = BFDO_ASK;
    iStopLevelOnWarning_    = BFDO_ASK;
    strBackupLogLocation_   = wxEmptyString;
}

