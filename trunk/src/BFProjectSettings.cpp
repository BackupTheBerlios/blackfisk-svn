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
#include "BFProject.h"
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

wxString BFProjectSettings::GetBackupLogLocation ()
{
    wxString strReturn = BFApp::ExtractCommunity(BFProject::Instance().GetAllDestinations());

    if (strBackupLogLocation_.IsEmpty())
    {
        strReturn = BFApp::ExtractCommunity(BFProject::Instance().GetAllDestinations());

        if (strReturn.IsEmpty())
            if (BFProject::Instance().GetAllDestinations().GetCount() > 0)
                strReturn = BFProject::Instance().GetAllDestinations()[0];
    }
    else
    {
        if ( strBackupLogLocation_ != strReturn )
        {
            strReturn = strBackupLogLocation_;
        }
        else
        {
            strBackupLogLocation_ = wxEmptyString;
        }
    }

    return strReturn;
}

void BFProjectSettings::SetBackupLogLocation (const wxString& strFile)
{
	wxString strReturn = BFApp::ExtractCommunity(BFProject::Instance().GetAllDestinations());

	if (strFile == strReturn)
		strBackupLogLocation_ = wxEmptyString;
	else
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

