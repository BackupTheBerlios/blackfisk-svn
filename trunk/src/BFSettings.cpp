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
            bOpenLastProject_(false),
            lMaxLogFileSize_(1024),
            verboseLog_(MsgINFO)
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

const wxString& BFSettings::GetLastProject ()
{
    return strLastProject_;
}

void BFSettings::SetLastProject (const wxChar* strFilename)
{
    if (strFilename == NULL)
        return;

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
        rA << (wxUint32&)lMaxLogFileSize_;
        rA << strLastProject_;
        rA << bOpenLastProject_;
        rA << (int)verboseLog_;
        rA << (int)lang_;
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

        if (rA.GetVersion() < BF_SETTINGS_CURRENT_VERSION)
        {
            lang_ = wxLANGUAGE_DEFAULT;
        }
        else
        {
            rA >> (int&)lang_;
        }
    }

    rA.LeaveObject();

    return true;
}
