/**
 * Name:        BFSystemBase.h
 * Purpose:     BFSystemBase
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-24
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

#ifndef BFSYSTEMBASE_H
#define BFSYSTEMBASE_H


/// verbose level for message handling
enum BF_VerboseLevel
{
    BFVERBOSE_INFO,
    BFVERBOSE_WARNING,
    BFVERBOSE_ERROR,
    BFVERBOSE_FATAL
};

///
enum BFMessageType
{
    MsgUNKNOWN,
    MsgBACKUP,
    MsgINFO,
    MsgWARNING,
    MsgERROR,
    MsgFATAL,
    MsgLOG,
    MsgDEBUG
};

#endif // BFSYSTEMBASE_H
