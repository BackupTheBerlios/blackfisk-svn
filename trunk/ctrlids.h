/**
 * Name:        ctrlids.h
 * Purpose:
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


#ifndef CTRLIDS_H
#define CTRLIDS_H

///
#define BFBACKUPCTRL_ID_ADDDESTINATION              1 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_COPY_DIR                    2 + wxID_HIGHEST
#define BFDIRCTRL_ID_BACKUP_DIRCOPY                 BFBACKUPCTRL_ID_COPY_DIR
#define BFBACKUPCTRL_ID_COPY_FILE                   3 + wxID_HIGHEST
#define BFDIRCTRL_ID_BACKUP_FILECOPY                BFBACKUPCTRL_ID_COPY_FILE
#define BFDIRCTRL_ID_FILEBUTTON                     4 + wxID_HIGHEST
#define BFDIRCTRL_ID_ADDDESTINATION                 5 + wxID_HIGHEST
#define BFDIRCTRL_ID_CREATEDESTINATION              6 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_CREATEDESTINATION           7 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_MACROBUTTON                 8 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_SYNC_DIR                    9 + wxID_HIGHEST
#define BFDIRCTRL_ID_BACKUP_SYNCDIR                 BFBACKUPCTRL_ID_SYNC_DIR
#define BFPLACEHOLDERBUTTON_ID_PRESSED              10 + wxID_HIGHEST
#define BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_DATE     11 + wxID_HIGHEST
#define BFPLACEHOLDERBUTTON_ID_PLACEHOLDER_TIME     12 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_PROJECTSETTINGS             13 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_TASKSETTINGS                14 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_DELETETASK                  15 + wxID_HIGHEST
#define BFBACKUPCTRL_ID_ARCHIVE_DIR                 16 + wxID_HIGHEST
#define BFDIRCTRL_ID_ARCHIVEDIR                     BFBACKUPCTRL_ID_ARCHIVE_DIR
#define BF_ID_MAINFRAME                             17 + wxID_HIGHEST
#define BF_BACKUPPROGRESSDLG_QUESTION               18 + wxID_HIGHEST  // custom event
#define BFBACKUPCTRL_ID_MODIFYDDESTINATION          19 + wxID_HIGHEST
#define BF_BACKUPTREE_REBUILD                       20 + wxID_HIGHEST  // custom event

#endif  // CTRLIDS_H