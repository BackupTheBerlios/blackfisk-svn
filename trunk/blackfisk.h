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

#define BF_PRGNAME                  _T("blackfisk")
#define BF_VERSION_MAJOR            0
#define BF_VERSION_MINOR            0
#define BF_VERSION_RELEASE          0
#define BF_VERSION_EXTENSION        _T("alpha (SVN)")
#define BF_AUTHOR                   _T("Christian Buhtz")
//__VERSION__ (gcc version)

#define BF_GRAPHICDIR               _T("graphic/")

#define BF_SETTINGS                 _T("blackfisk.cfg")
#define BF_SETTINGS_CURRENT_VERSION 1000
#define BF_PROJECT_CURRENT_VERSION  1010

#define BF_LOGFILE_NAME             _T("bf.log")
#define BF_LOGFILE_BAKSUFFIX        _T(".bak")

#define BF_PROJECT_EXTENSION        _T(".bfp")
#define BF_PROJECT_EXTENSION_STRING _("Blackfisk project files (*.bfp)|*.bfp")
#endif // BLACKFISK_H
