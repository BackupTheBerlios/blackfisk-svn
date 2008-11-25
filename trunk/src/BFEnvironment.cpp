/**
 * Name:        BFEnvironment.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-11-25
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#include "BFEnvironment.h"

#include <wx/filefn.h>

/*static*/ wxString BFEnvironment::strApplicationDir_   = wxEmptyString;
/*static*/ bool     BFEnvironment::bProjectScheduled_   = false;

BFEnvironment::BFEnvironment ()
{
}


/*virtual*/ BFEnvironment::~BFEnvironment ()
{
}


/*static*/ void BFEnvironment::RememberApplicationDirectory (const wxCmdLineArgsArray& argv)
{
// XXX
#ifdef _DEBUG
    strApplicationDir_ = "D:\\Garage\\projekte\\blackfisk\\trunk";
    return;
#endif

    strApplicationDir_ = argv[0].BeforeLast(wxFILE_SEP_PATH);

    if (strApplicationDir_.IsEmpty())
        strApplicationDir_ = wxGetCwd();
}

/*static*/ const wxString& BFEnvironment::GetApplicationDirectory ()
{
    return strApplicationDir_;
}

/*static*/ const wxString BFEnvironment::GetLogFileName ()
{
    #if defined(__WXMSW__)
        wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "bf.log";
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString BFEnvironment::GetGraphicDir ()
{
    #if defined(__WXMSW__)
        wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "graphic" + wxFILE_SEP_PATH;
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString BFEnvironment::GetSoundDir ()
{
    #if defined(__WXMSW__)
        wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "sound" + wxFILE_SEP_PATH;
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString BFEnvironment::GetSettingsFileName ()
{
    #if defined(__WXMSW__)
        wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "blackfisk.cfg";
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString BFEnvironment::GetInBuildCrontabFileName ()
{
    #if defined(__WXMSW__)
        wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "wxCron" + wxFILE_SEP_PATH + "crontab";
    #elif defined(__UNIX__)
        #error "UNIX not supported plattform! Please contact the project maintainer for support!"
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ void BFEnvironment::SetProjectScheduled (bool bScheduled)
{
    bProjectScheduled_ = bScheduled;
}

/*static*/ bool BFEnvironment::IsProjectScheduled ()
{
    return bProjectScheduled_;
}
