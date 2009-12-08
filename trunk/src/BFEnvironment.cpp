/**
 * Name:        BFEnvironment.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-11-25
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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

#include "BFApp.h"
#include "blackfisk.h"

#include <wx/filefn.h>
#include <wx/stdpaths.h>

/*static*/ wxString BFEnvironment::strApplicationDir_		= wxEmptyString;
/*static*/ wxString BFEnvironment::strApplicationBinary_	= wxEmptyString;
/*static*/ bool     BFEnvironment::bProjectScheduled_		= false;

BFEnvironment::BFEnvironment ()
{
}


/*virtual*/ BFEnvironment::~BFEnvironment ()
{
}


/*static* void BFEnvironment::RememberApplicationDirectoryAndName (const wxCmdLineArgsArray& argv)
{
    strApplicationDir_ = argv[0].BeforeLast(wxFILE_SEP_PATH);

    if (strApplicationDir_.IsEmpty())
        strApplicationDir_ = wxGetCwd();

	strApplicationName_ = argv[0].AfterLast(wxFILE_SEP_PATH);
}

/*static*/ const wxString& BFEnvironment::GetApplicationDirectory ()
{
	if ( strApplicationDir_.IsEmpty() )
		strApplicationDir_ = wxGetApp().argv[0].BeforeLast(wxFILE_SEP_PATH);

    if (strApplicationDir_.IsEmpty())
		strApplicationDir_ = wxGetCwd();

    return strApplicationDir_;
}

/*static*/ const wxString& BFEnvironment::GetApplicationBinary ()
{
	if ( strApplicationBinary_.IsEmpty() )
		strApplicationBinary_ = wxGetApp().argv[0].AfterLast(wxFILE_SEP_PATH);

    return strApplicationBinary_;
}

/*static*/ wxString BFEnvironment::GetApplicationPath ()
{
    return GetApplicationDirectory() + wxFILE_SEP_PATH + GetApplicationBinary();
}

/*static*/ wxString BFEnvironment::GetDocumentOpenCommand ()
{
	return GetApplicationPath() + " %1";
}

/*static*/ wxString BFEnvironment::GetUserConfigDir ()
{
	wxString str = wxStandardPaths::Get().GetUserConfigDir();

	if ( false == str.EndsWith(wxFILE_SEP_PATH) )
		str << wxFILE_SEP_PATH;

	str << BF_PRGNAME;

	if ( false == wxDirExists(str) )
		wxMkDir(str);

	return str;
}

/*static*/ const wxString BFEnvironment::GetLogFileName ()
{
	wxString str = GetUserConfigDir();

	str << wxFILE_SEP_PATH
		<< "bf.log";

	return str;
}

/*static*/ const wxString BFEnvironment::GetGraphicDir ()
{
	wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "graphic" + wxFILE_SEP_PATH;

	return str;
}

/*static*/ const wxString BFEnvironment::GetSoundDir ()
{
	wxString str = GetApplicationDirectory() + wxFILE_SEP_PATH + "sound" + wxFILE_SEP_PATH;

    return str;
}

/*static*/ const wxString BFEnvironment::GetSettingsFileName ()
{
	wxString str = GetUserConfigDir();

	str << wxFILE_SEP_PATH
		<< "blackfisk.cfg";

	return str;
}

/*static*/ const wxString BFEnvironment::GetInBuildCrontabFileName ()
{
	wxString str = wxStandardPaths::Get().GetUserConfigDir();

	if ( false == str.EndsWith(wxFILE_SEP_PATH) )
		str << wxFILE_SEP_PATH;

	str << "wxCron" << wxFILE_SEP_PATH << "crontab";

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
