/**
 * Name:        blackfisk.h
 * Purpose:     global header
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-03-29
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

#ifndef BLACKFISK_H
#define BLACKFISK_H

#ifndef _DEBUG
#define BF_PRGNAME                  "blackfisk"
#else
#define BF_PRGNAME                  "blackfisk DEBUG"
#endif
#define BF_VERSION_MAJOR            0
#define BF_VERSION_MINOR            0
#define BF_VERSION_RELEASE          0
#define BF_VERSION_EXTENSION        "xxx" //snapshot 2009-05-24 (rev149)"
#define BF_AUTHOR                   "Christian Buhtz"
//__VERSION__ (gcc version)

#define BF_URL_WEBSITE              "http://www.blackfisk.org"
#define BF_URL_BUGREPORT            "http://developer.berlios.de/bugs/?group_id=8687"
#define BF_URL_FEAUTERREQUEST       "http://developer.berlios.de/feature/?group_id=8687"
#define BF_URL_MAIL                 "mailto:blackfisk@web.de"

#define BF_FTP_URL                  "ftp.berlios.de"
#define BF_FTP_DIR                  "/pub/blackfisk"
#define BF_FTP_FILE_VER             "current_version"

#define BF_SETTINGS_CURRENT_VERSION 1050
#define BF_PROJECT_CURRENT_VERSION  1040

#define BF_PROJECT_EXTENSION        ".bfp"
#define BF_PROJECT_EXTENSION_STRING "Blackfisk project files (*.bfp)|*.bfp"

#define BF_REGKEY_AUTOSTART         "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define BF_REGKEY_VALUE             "wxCron (blackfisk in-build scheduler)"

#define BF_CRONTABLINE_DEFAULT      "0 12 * * * \"%s\" --run \"%s\""

#define BF_MSG_DEFAULT_TIMER        10

#endif // BLACKFISK_H
