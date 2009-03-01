/**
 * Name:        BFTaskBase.h
 * Purpose:     BFTaskBase class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-29
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFTASKBASE_H
#define BFTASKBASE_H

#include <wx/wx.h>
#include <vector>

#define BFTASK_PLACEHOLDER_DATE   "<date>"
#define BFTASK_PLACEHOLDER_TIME   "<time>"

///
typedef wxUint32 BFoid;
#define BFInvalidOID (BFoid)-1

///
enum BF_StopLevel
{
    BFDO_STOPPRJ,
    BFDO_STOPTSK,
    BFDO_ASK,
    BFDO_IGNORE,
    BFDO_UNKNOWN
};


/** define the type of the task
    it says what the tasks exactly do */
typedef wxUint8 BFTaskType;
#define TaskINVALID     0
#define TaskARCHIVE     1       // comprase backup-source to an archive
#define TaskDIRCOPY     2       // copy a complete directory
#define TaskFILECOPY    3       // copy a file
#define TaskSYNC        4       // synchronise directories
#define TaskHIGHEST     TaskSYNC    // ! just to know the end of the taks types !

///
typedef std::vector<BFTaskType> BFTypeVector;

/** define the compression types
    which compression format should be used for TaskARCHIVE */
typedef wxUint8 BFArchiveFormat;
#define CompressNOTUSED 0
#define CompressZIP     1
//#define Compress7Z    2
//#define CompressTAR   3
//#define ...

// forwarde declarations
class BFTask;

///
typedef std::vector<BFTask*>    BFTaskVector;
///
typedef BFTaskVector::iterator  BFTaskVectorIt;


#endif    // BFTASKBASE_H
