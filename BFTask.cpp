/**
 * Name:        BFTask.cpp
 * Purpose:     BFTask and BFTaskData class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-28
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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


#include "BFTask.h"

#include "BFRootTask.h"
#include "BFundef.h"
#include "BFIconTable.h"
#include "BFSystem.h"

BFTaskData::BFTaskData (BFTaskType type,
                        const wxChar* strSource,
                        const wxChar* strDestination,
                        const wxChar* strName,
                        bool bVerify,
                        BFArchiveFormat archive,
                        wxArrayString& arrExclude)
          : type_(type),
            strSource_(strSource),
            strDestination_(strDestination),
            strName_(strName),
            bVerify_(bVerify),
            archiveFormat_(archive),
            arrExclude_(arrExclude)
{
    oid_ = BFRootTask::Instance().CreateOID();
}

BFTaskData::BFTaskData ()
          : type_(TaskINVALID),
            oid_(BFInvalidOID)
{
}

/*virtual*/ BFTaskData::~BFTaskData ()
{
}

bool BFTaskData::IsPlausible ()
{
    if (oid_ == BFInvalidOID)
        return false;

    return true;
}

BFTaskType BFTaskData::GetType ()
{
    return type_;
}

const wxChar* BFTaskData::GetSource ()
{
    return strSource_.c_str();
}

const wxChar* BFTaskData::GetDestination ()
{
    return strDestination_.c_str();
}

const wxChar* BFTaskData::GetName ()
{
    return strName_.c_str();
}

bool BFTaskData::Verify ()
{
    return bVerify_;
}

BFArchiveFormat BFTaskData::GetArchiveFormat ()
{
    return archiveFormat_;
}

const wxArrayString& BFTaskData::GetExclude ()
{
    return arrExclude_;
}

bool BFTaskData::SetOID (BFoid oid)
{
    if (BFRootTask::Instance().HasTask(oid))
        return false;

    oid_ = oid;

    return true;
}

void BFTaskData::SetTaskType (BFTaskType type)
{
    type_ = type;
}

BFoid BFTaskData::GetOID ()
{
    return oid_;
}

void BFTaskData::SetSource (const wxChar* source)
{
    if (source == NULL)
        return;

    strSource_ = source;
}

void BFTaskData::SetDestination (const wxChar* dest)
{
    if (dest == NULL)
        return;

    strDestination_ = dest;
}

void BFTaskData::SetName (const wxChar* name)
{
    if (name == NULL)
        return;

    strName_ = name;
}

void BFTaskData::SetVerify (bool verify)
{
    bVerify_ = verify;
}

void BFTaskData::SetArchiveFormat (BFArchiveFormat archiveFormat)
{
    archiveFormat_ = archiveFormat;
}

void BFTaskData::SetExclude (const wxArrayString& exclude)
{
    arrExclude_ = exclude;
}

wxString BFTask::GetTypeDescription ()
{
    wxString strDesc;

    switch (GetType())
    {
        case TaskARCHIVE:
            strDesc << _("compress to a ") << GetArchiveExtension() << _(" archive");
            break;

        case TaskDIRCOPY:
            strDesc = _("copy the directory");
            break;

        case TaskFILECOPY:
            strDesc = _("copy the file");
            break;

        case TaskSYNC:
            strDesc = _("synchronize directories");
            break;

        default:
            strDesc = _("unknown task type");
            break;
    };

    return strDesc;
}

int BFTask::GetTypeIconId ()
{
    return GetTypeIconId(GetType());
}

/*static*/ int BFTask::GetTypeIconId (BFTaskType type)
{
    int iconId = -1;

    switch (type)
    {
        case TaskARCHIVE:
            iconId = BFIconTable::task_zip;
            break;

        case TaskDIRCOPY:
            iconId = BFIconTable::task_dircopy;
            break;

        case TaskFILECOPY:
            iconId = BFIconTable::task_filecopy;
            break;

        case TaskSYNC:
            iconId = BFIconTable::task_sync;
            break;

        default:
            BFSystem::Fatal(_T("no matching TaskType"), _T("BFTask::GetTypeIconId"));
            break;
    };

    return iconId;
}

bool BFTask::Serialize (jbSerialize& rA)
{
    // check parameters
    if ( !(rA.IsOpen()) )
        return false;

    bool rc = false;

    rA.EnterObject();

    // ** serialize TO file **
    if( rA.IsStoring() )
    {
        rA << GetOID();
        rA << GetType();
        rA << GetSource();
        rA << GetDestination();
        rA << GetName();
        rA << Verify();
        rA << GetArchiveFormat();
        rA << GetExclude();
    }
    else
    // ** serialize FROM file **
    {
        BFTaskType      type;
        BFArchiveFormat archiveFormat;
        wxString        strSource, strDestination, strName;
        bool            bVerify;
        wxArrayString   arrExclude;
        BFoid           oid;

        rA >> oid;
        rA >> type;
        rA >> strSource;
        rA >> strDestination;
        rA >> strName;
        rA >> bVerify;
        rA >> archiveFormat;
        rA >> arrExclude;

        SetTaskType(type);
        SetSource(strSource);
        SetDestination(strDestination);
        SetName(strName);
        SetVerify(bVerify);
        SetArchiveFormat(archiveFormat);
        SetExclude(arrExclude);

        if ( !(SetOID(oid)) )
        {
            BFSystem::Fatal(_("Unable to set an OID while loading a project file.\nThe file is corrupt!"), _T("BFTask::Serialize()"));
            rc = false;
        }
    }

    rA.LeaveObject();

    // check for errors
    if ( rA.IsOk())
    {
        rc =  true;
    }
    else
    {
        if (rA.Eof())
            rc = true;    // there is no need to handle EOF like an error
        else
            rc = false;   // on real error
    }

    return rc;
}

BFTask::BFTask (BFTaskType type,
                const wxChar* strSource,
                const wxChar* strDestination,
                const wxChar* strName,
                bool bVerify,
                BFArchiveFormat archiveFormat,
                wxArrayString& arrExclude)
      : BFTaskData(type, strSource, strDestination, strName, bVerify, archiveFormat, arrExclude),
        bStopTask_(false)
{
}

BFTask::BFTask ()
{
}

//
/*virtual*/ BFTask::~BFTask ()
{
}


wxString BFTask::GetArchiveExtension()
{
    switch (GetArchiveFormat())
    {
        case CompressZIP:
            return _T("zip");
            break;
    }

    return _T("unknown");
}

bool BFTask::RunForArchive (ProgressWithMessage& rProgress)
{
    // create destination string
    wxString strDest, strSrc;
    strDest = strDest + GetDestination() + wxFILE_SEP_PATH + GetName() + _T(".") + GetArchiveExtension();
    strSrc  = GetSource();
    ReplaceMacros(strDest);
    ReplaceMacros(strSrc);

    // exclude something?
    wxArrayString* pArr = NULL;
    if (GetExclude().Count() > 0)
        pArr = new wxArrayString(GetExclude());

    switch (GetArchiveFormat())
    {
        case CompressZIP:
            Core().CreateZipFromDir
            (
                strDest.c_str(),
                strSrc.c_str(),
                pArr,
                Verify(),
                &rProgress
            );
        break;

        default:
            BFSystem::Fatal(_T("unknown archive type"), _T("BFTask::RunForArchive()"));
            break;
    }

    // delete temporary exclude array
    if (pArr != NULL)
        delete pArr;

    return true;
}


bool BFTask::RunForDirCopy (ProgressWithMessage& rProgress)
{
    wxString strDest, strSrc;
    strDest = strDest + GetDestination() + wxFILE_SEP_PATH + GetName();
    strSrc  = GetSource();
    ReplaceMacros(strDest);
    ReplaceMacros(strSrc);

    // copy dir
    return Core().CopyDir
    (
        strSrc.c_str(),
        strDest.c_str(),
        Verify(),
        &rProgress
    );
}


bool BFTask::RunForFileCopy(ProgressWithMessage& rProgress)
{
    // create destination string
    wxString strDest, strSrc;
    strDest = strDest + GetDestination() + wxFILE_SEP_PATH + GetName();
    strSrc  = GetSource();
    ReplaceMacros(strDest);
    ReplaceMacros(strSrc);

    // copy file
    return Core().CopyFile
    (
        strSrc.c_str(),
        strDest.c_str(),
        true,
        Verify()
    );
}

bool BFTask::RunForDirSync (ProgressWithMessage& rProgress)
{
    // create destination string
    wxString strDest, strSrc;
    strDest = strDest + GetDestination() + wxFILE_SEP_PATH + GetName();
    strSrc  = GetSource();
    ReplaceMacros(strDest);
    ReplaceMacros(strSrc);

    return Core().Synchronize
    (
        GetSource(),
        GetDestination(),
        Verify()
    );
}

void BFTask::StopTask ()
{
    bStopTask_ = true;
}

bool BFTask::Run (ProgressWithMessage& rProgress)
{
    // XXX
    //BFSystem::Info(_T("TEST INFO 1"));
    BFSystem::Warning(_T("TEST WARNING 1"));
    //BFSystem::Error(_T("TEST ERROR 1"));
    //BFSystem::Fatal(_T("TEST FATAL 1"));
    //BFSystem::Debug(_T("TEST DEBUG 1"));
    //BFSystem::Log(_T("TEST LOG 1"));
    //BFSystem::Backup(_T("TEST BACKUP 1"));

    switch (GetType())
    {
        case TaskARCHIVE:
            RunForArchive(rProgress);
            break;

        case TaskDIRCOPY:
            RunForDirCopy(rProgress);
            break;

        case TaskFILECOPY:
            RunForFileCopy(rProgress);
            break;

        case TaskSYNC:
            RunForDirSync(rProgress);
            break;

        default:
            break;
    };  // switch(type_)

    if ( BFCore::IsStop() )
        return false;

    return true;
}

