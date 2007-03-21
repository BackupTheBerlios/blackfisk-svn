/**
 * implementation of class BFTaskData
 * implementation of class BFTask
 * 2006-04-28
 ***/

#include "BFTask.h"
#include "BFRootTask.h"
#include "BFundef.h"

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

        default:
            strDesc = _("unknown task type");
            break;
    };

    return strDesc;
}

int BFTask::GetTypeIconId ()
{
    int iconId = -1;

    switch (GetType())
    {
        case TaskARCHIVE:
            iconId = BFICON_TASKZIP;
            break;

        case TaskDIRCOPY:
            iconId = BFICON_TASKDC;
            break;

        case TaskFILECOPY:
            iconId = BFICON_TASKFC;
            break;

        default:
            BFSystem::Fatal(_T("no matching TaskType"), _T("BFTask::GetTypeIconId"));
            break;
    };

    return iconId;
}

bool BFTask::Serialize (jbArchive& rA)
{
    // check parameters
    if ( !(rA.IsOpen()) )
        return false;

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
            return false;
        }
    }

    // check for errors
    if ( rA.IsOk())
    {
        return true;
    }
    else
    {
        if (rA.Eof())
            return true;    // there is no need to handle EOF like an error
        else
            return false;   // on real error
    }
}

BFTask::BFTask (BFTaskType type,
                const wxChar* strSource,
                const wxChar* strDestination,
                const wxChar* strName,
                bool bVerify,
                BFArchiveFormat archiveFormat,
                wxArrayString& arrExclude)
      : BFTaskData(type, strSource, strDestination, strName, bVerify, archiveFormat, arrExclude)
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

    // progress message
    // ... do something with rProgress ...

    // copy file
    return Core().CopyFile
    (
        strSrc.c_str(),
        strDest.c_str(),
        true,
        Verify()
    );
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

        default:
            break;
    };  // switch(type_)

    return true;
}

