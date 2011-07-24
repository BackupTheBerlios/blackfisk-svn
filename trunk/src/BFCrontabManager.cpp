/**
 * Name:        BFCrontabManager.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2011-03-15
 * Copyright:   (c) 2011 Christian Buhtz <exsudat@gmx.de>
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


#include "BFCrontabManager.h"
#include "BFBackup.h"
#include "BFApp.h"
#include "ids.h"
#include "blackfisk.h"

BFCrontabManager::BFCrontabManager ()
{
}


/*virtual*/ BFCrontabManager::~BFCrontabManager ()
{
}

/*static*/ wxString BFCrontabManager::CreateRetryLine (const wxString& orgCrontabline,
                                                       long lHours,
                                                       long lMinutes)
{
    wxArrayString arr;
    wxString strTime;

    ParseCrontabline(orgCrontabline, arr);

    if ( lHours == 0 )
        strTime = CreateTimePart (INTERVALL_MINUTES, 0, lMinutes);
    else
        strTime = CreateTimePart (INTERVALL_HOURS, lHours, 0);

    return wxString::Format("%s %s", strTime, arr[5]);
}

/*static*/ wxString BFCrontabManager::CreateTimePart (CronMode mode,
                                                      long lHour /*= 0*/,
                                                      long lMinute /*= 0*/,
                                                      long lDay /*= 0*/,
                                                      long lWeekday /*= 0*/)
{
    wxString strLine;

    switch ( mode )
    {
        case DAILY:
	        strLine = wxString::Format("%d %d * * *",
									    lMinute,
                                        lHour);
            break;

        case WEEKLY:
	        strLine = wxString::Format("%d %d * * %d",
                                        lMinute,
                                        lHour,
                                        lWeekday);
            break;

        case INTERVALL_MINUTES:
	        strLine = wxString::Format("*/%d * * * *",
                                        lMinute);
            break;

        case INTERVALL_HOURS:
	        strLine = wxString::Format("0 */%d * * *",
                                        lHour);
            break;

        case INTERVALL_DAYS:
	        strLine = wxString::Format("0 0 */%d * *",
                                        lDay);
            break;
	}

    return strLine;
}

/*static*/ wxArrayString& BFCrontabManager::ParseCrontabline (const wxString& strLine,
                                                              wxArrayString& arr)
{
    wxString        str(strLine);
    wxString        strCurrent;
    size_t          idx;

    arr.Clear();

    // extract the first five parameters
    while ( !(str.IsEmpty()) && arr.GetCount() < 5 )
    {
        // find white spaces at the beginning
        idx = 0;
        while (str[idx] == ' ' || idx == str.Length())
            ++idx;

        // erase white spaces at the beginning
        str = str.Mid(idx);

        //
        strCurrent = str.BeforeFirst(' ');
        str = str.Mid(strCurrent.Length());

        //
        arr.Add(strCurrent);
    }

    // remember the unparsed rest
    if ( !(str.IsEmpty()) )
        arr.Add(str);

    // enough parameters
    if (arr.GetCount() < 6)
        arr.Clear();

    // default?
    if ( arr.IsEmpty() )
		ParseCrontabline ( CreateCrontablineDefault(), arr );

    return arr;
}

/*static*/ wxString BFCrontabManager::CreateCrontablineDefault (const wxString& strTimepart /*= wxEmptyString*/)
{
	wxString strRC = wxString::Format( BF_CRONTABLINE_DEFAULT,
                                       wxTheApp->argv[0],
                                       BFBackup::Instance().GetCurrentFilename() );

    if ( strTimepart.Len() > 0 )
        strRC.Replace(strRC.BeforeFirst('"'), wxString::Format("%s ", strTimepart));

    return strRC;
}
