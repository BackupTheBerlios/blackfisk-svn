/**
 * Name:        BFCrontabManager.h
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


#ifndef BFCRONTABMANAGER_H
#define BFCRONTABMANAGER_H

#include <wx/wx.h>

/// 
class BFCrontabManager
{
    private:

    protected:

    public:
        ///
        enum CronMode
        {
            DAILY,
            WEEKLY,
            INTERVALL_MINUTES,
            INTERVALL_HOURS,
            INTERVALL_DAYS
        };

        /// ctor
        BFCrontabManager ();

        /// virtual dtor
        virtual ~BFCrontabManager ();

        /** */
        static wxString CreateRetryLine (const wxString& orgCrontabline,
                                         long lHours,
                                         long lMinutes);

        ///
        static wxString CreateTimePart (CronMode mode,
                                        long lHour = 0,
                                        long lMinute = 0,
                                        long lDay = 0,
                                        long lWeekday = 0);

        /** Separate each part of a crontab-line and store it as an
            element of 'arr'. The string-array will cleared at the beginning
            of this methode!
            It means that the strings for minute, hour, day-of-month, month,
            day-of-week and the command are extracted.
            If there are not enough (<5) parameters in the line, 'arr' is empty. */
        static wxArrayString& ParseCrontabline (const wxString& strLine, wxArrayString& arr);

		/** Create a full crontab line/entry with
			the current binary and the current projectfile.
            'strTimepart' is by default BF_CRONTABLINE_DEFAULT
            or can be specified explicite. */
        static wxString CreateCrontablineDefault (const wxString& strTimepart = wxEmptyString);
};

#endif
