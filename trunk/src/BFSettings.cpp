/**
 * Name:        BFSettings.cpp
 * Purpose:     BFSettings class implementation
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

#include "BFSettings.h"
#include "blackfisk.h"


/*static*/ BFSettings BFSettings::sSettings_;

/*static*/ const wxLanguage BFSettings::langIds_[] =
{
    wxLANGUAGE_DEFAULT,
    wxLANGUAGE_ENGLISH,
    wxLANGUAGE_GERMAN
};

/*static*/ const wxString BFSettings::langNames_[] =
{
    "System default",
    "English",
    "German"
};

//
BFSettings::BFSettings ()
          : bFillBlackfiskPlaceholders_(false),
            bWithFiles_(false),
            lMaxLogFileSize_(1024),
            bOpenLastProject_(false),
            verboseLog_(MsgINFO),
            lang_(wxLANGUAGE_DEFAULT),
            sizeMainWindow_(wxDefaultSize),
            pointMainWindow_(wxDefaultPosition),
            bMainWindowMaximized_(false),
            bSwitchMainCtrls_(false),
            iSashPositionInMainWindow_(-1),
            bShowHiddenFiles_(false),
            lDaysTillNextCheck_(0),
            dateLastVersionCheck_(wxDateTime::Now().SetYear(2000))
{
}


//
/*virtual*/ BFSettings::~BFSettings ()
{
}


BFProjectSettings& BFSettings::GetDefaultProjectSettings ()
{
    return defaultPrj_;
}

bool BFSettings::GetFillBlackfiskPlaceholders ()
{
    return bFillBlackfiskPlaceholders_;
}

void BFSettings::SetFillBlackfiskPlaceholders (bool bFill)
{
    bFillBlackfiskPlaceholders_ = bFill;
}

bool BFSettings::GetWithFiles ()
{
    return bWithFiles_;
}

void BFSettings::SetWithFiles (bool bWithFiles)
{
    bWithFiles_ = bWithFiles;
}

wxUint32 BFSettings::GetMaxLogFileSize ()
{
    return lMaxLogFileSize_;
}

void BFSettings::SetMaxLogFileSize (wxUint32 lSizeInKiloByte)
{
    if (lSizeInKiloByte < 0)
        lMaxLogFileSize_ = 0;
    else
        lMaxLogFileSize_ = lSizeInKiloByte;
}

const wxString& BFSettings::GetLastProject ()
{
    return strLastProject_;
}

void BFSettings::SetLastProject (const wxString& strFilename)
{
    strLastProject_ = strFilename;
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

void BFSettings::SetLanguage (wxLanguage lang)
{
    lang_ = lang;
}

wxLanguage BFSettings::GetLanguage ()
{
    return lang_;
}

void BFSettings::SetMainWindowSize (const wxSize& size)
{
    sizeMainWindow_ = size;
}

wxSize BFSettings::GetMainWindowSize ()
{
    return sizeMainWindow_;
}

void BFSettings::SetMainWindowMaximized (bool bMax)
{
    bMainWindowMaximized_ = bMax;
}

bool BFSettings::GetMainWindowMaximized ()
{
    return bMainWindowMaximized_;
}

void BFSettings::SetMainWindowPosition (const wxPoint& point)
{
    pointMainWindow_ = point;
}

wxPoint BFSettings::GetMainWindowPosition ()
{
    return pointMainWindow_;
}

void BFSettings::SetSwitchMainCtrls (bool bSwitch)
{
    bSwitchMainCtrls_ = bSwitch;
}

bool BFSettings::GetSwitchMainCtrls ()
{
    return bSwitchMainCtrls_;
}

void BFSettings::SetSashPositionInMainWindow (int pos)
{
    iSashPositionInMainWindow_ = pos;
}

int BFSettings::GetSashPositionInMainWindow ()
{
    return iSashPositionInMainWindow_;
}

void BFSettings::SetShowHiddenFiles (bool bShowHidden)
{
    bShowHiddenFiles_ = bShowHidden;
}

bool BFSettings::GetShowHiddenFiles ()
{
    return bShowHiddenFiles_;
}

void BFSettings::SetDaysTillNextCheck (wxUint32 lDays)
{
    if (lDays >= 0)
        lDaysTillNextCheck_ = lDays;
}

wxUint32 BFSettings::GetDaysTillNextCheck ()
{
    return lDaysTillNextCheck_;
}

void BFSettings::SetLastVersionCheck (wxDateTime date)
{
    dateLastVersionCheck_ = date;
}

wxDateTime BFSettings::GetLastVersionCheck ()
{
    return dateLastVersionCheck_;
}

bool BFSettings::CheckForNewVersion ()
{
    if ( lDaysTillNextCheck_ == 0
       || wxDateTime::Now().Subtract(dateLastVersionCheck_).GetDays() < lDaysTillNextCheck_)
        return false;

    return true;
}

bool BFSettings::Serialize (jbSerialize& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    rA.EnterObject();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        defaultPrj_.Serialize(rA);
        rA << bFillBlackfiskPlaceholders_;
        rA << bWithFiles_;
        rA << lMaxLogFileSize_;
        rA << strLastProject_;
        rA << bOpenLastProject_;
        rA << (int)verboseLog_;
        rA << (int)lang_;
        rA << sizeMainWindow_;
        rA << bMainWindowMaximized_;
        rA << pointMainWindow_;
        rA << bSwitchMainCtrls_;
        rA << iSashPositionInMainWindow_;
        rA << bShowHiddenFiles_;
        rA << lDaysTillNextCheck_;
        rA << dateLastVersionCheck_;
    }
    else
    // ** serialize FROM file **
    {
        defaultPrj_.Serialize(rA);
        rA >> bFillBlackfiskPlaceholders_;
        rA >> bWithFiles_;
        rA >> (wxUint32&)lMaxLogFileSize_;
        rA >> strLastProject_;
        rA >> bOpenLastProject_;
        rA >> (int&)verboseLog_;

        /* check if the current reading settings file is a older version
           and set default values */

        // language added in version 1010
        if (rA.GetVersion() < 1010)
            lang_ = wxLANGUAGE_DEFAULT;
        else
            rA >> (int&)lang_;

        // options added in version 1020
        if (rA.GetVersion() < 1020)
        {
            sizeMainWindow_             = wxDefaultSize;
            bMainWindowMaximized_       = false;
            pointMainWindow_            = wxDefaultPosition;
            bSwitchMainCtrls_           = false;
            iSashPositionInMainWindow_  = -1;
            bShowHiddenFiles_           = false;
            lDaysTillNextCheck_         = 0;
            dateLastVersionCheck_       = wxDateTime::Now().SetYear(2000);
        }
        else
        {
            rA >> sizeMainWindow_;
            rA >> bMainWindowMaximized_;
            rA >> pointMainWindow_;
            rA >> bSwitchMainCtrls_;
            rA >> iSashPositionInMainWindow_;
            rA >> bShowHiddenFiles_;
            rA >> lDaysTillNextCheck_;
            rA >> dateLastVersionCheck_;
        }
    }

    rA.LeaveObject();

    return true;
}

