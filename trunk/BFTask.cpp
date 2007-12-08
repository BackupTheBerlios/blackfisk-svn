/**
 * Name:        BFTask.cpp
 * Purpose:     BFTask and BFTaskData class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-28
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


#include "BFTask.h"
#include "BFCore.h"
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
            oid_(BFInvalidOID),
            archiveFormat_(CompressNOTUSED)
{
}

/*virtual*/ BFTaskData::~BFTaskData ()
{
}

bool BFTaskData::IsValid ()
{
    if (oid_ == BFInvalidOID)
        return false;

    if (type_ == TaskINVALID || (type_ == TaskARCHIVE && archiveFormat_ == CompressNOTUSED))
        return false;

    if (strDestination_.Length() == 0)
        return false;

    if (strSource_.Length() == 0)
        return false;

    if (strName_.Length() == 0)
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
    // it is a fresh task (maybe while serializing from file)
    if (GetType() == TaskINVALID)
    {
        type_ = type;
        return;
    }

    // check if the task can be modified
    BFTypeVector vecTypes;
    GetAvailableTypes(vecTypes);

    for (BFTypeVector::iterator it = vecTypes.begin();
         it != vecTypes.end();
         ++it)
    {
        if ((*it) == type)
        {
            type_ = type;
            return;
        }
    }

    BFSystem::Fatal(_T("not able to set task type"), _T("BFTaskData::SetTaskType()"));
}

BFoid BFTaskData::GetOID () const
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

    // upper case the drive letter
    strDestination_.SetChar(0, wxString(strDestination_[0]).MakeUpper()[0]);
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

/*static*/ wxString BFTask::GetTypeDescription (BFTaskType type, BFArchiveFormat format /* = CompressNOTUSED*/)
{
    wxString strDesc;

    switch (type)
    {
        case TaskARCHIVE:
            strDesc << _("compress to a ") << GetArchiveExtension(format) << _(" archive");
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

wxString BFTask::GetTypeDescription ()
{
    return GetTypeDescription(GetType(), GetArchiveFormat());
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

void BFTaskData::GetAvailableTypes (BFTypeVector& rVecType)
{
    switch (GetType())
    {
        case TaskINVALID:
        case TaskARCHIVE:
        case TaskDIRCOPY:
        case TaskSYNC:
            if (GetType() != TaskARCHIVE)
                rVecType.push_back (TaskARCHIVE);
            if (GetType() != TaskDIRCOPY)
                rVecType.push_back (TaskDIRCOPY);
            if (GetType() != TaskSYNC)
                rVecType.push_back (TaskSYNC);
            break;
    };

    rVecType.insert (rVecType.begin(), GetType());
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
    return GetArchiveExtension(GetArchiveFormat());
}

/*static*/ wxString BFTask::GetArchiveExtension(BFArchiveFormat format)
{
    switch (format)
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
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // exclude something?
    wxArrayString* pArr = NULL;
    if (GetExclude().Count() > 0)
        pArr = new wxArrayString(GetExclude());

    switch (GetArchiveFormat())
    {
        case CompressZIP:
            BFCore::Instance().CreateZipFromDir
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
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // copy dir
    return BFCore::Instance().CopyDir
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
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // copy file
    return BFCore::Instance().CopyFile
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
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    return BFCore::Instance().Synchronize
    (
        strSrc.wx_str(),
        strDest.wx_str(),
        Verify(),
        &rProgress
    );
}

void BFTask::StopTask ()
{
    bStopTask_ = true;
}

bool BFTask::Run (ProgressWithMessage& rProgress)
{
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

