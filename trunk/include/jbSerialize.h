/**
 * Name:        jbSerialize.h
 * Purpose:     tyedef for wxSerialize (aka wxArchive)
 *              wxArchive is not a part of the official wxWidgets library
 *              that is why I want another name for it
 *              I just used the wxArchive-developers name (Jorgen Bodde) as prefix
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

#ifndef JBSERIALIZE_H
#define JBSERIALIZE_H

#include "../third-party/wxserialize-1.23/include/wx/wxSerialize.h"

typedef wxSerialize jbSerialize;

#endif  // JBSERIALIZE_H
