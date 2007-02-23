/**
 * implementation of class OBTaskData
 * implementation of class OBTask
 * 2006-04-28
 ***/

#include "OBTask.h"
#include "OBRootTask.h"
#include "OBundef.h"

OBTaskData::OBTaskData (OBTaskType type,
                        const wxChar* strSource,
                        const wxChar* strDestination,
                        const wxChar* strName,
                        bool bVerify,
                        OBArchiveFormat archive,
                        wxArrayString& arrExclude)
          : type_(type),
            strSource_(strSource),
            strDestination_(strDestination),
            strName_(strName),
            bVerify_(bVerify),
            archiveFormat_(archive),
            arrExclude_(arrExclude)
{
    oid_ = OBRootTask::Instance().CreateOID();
}

OBTaskData::OBTaskData ()
          : type_(TaskINVALID),
            oid_(OBInvalidOID)
{
}

/*virtual*/ OBTaskData::~OBTaskData ()
{
}

bool OBTaskData::IsPlausible ()
{
    if (oid_ == OBInvalidOID)
        return false;

    return true;
}

OBTaskType OBTaskData::GetType ()
{
    return type_;
}

const wxChar* OBTaskData::GetSource ()
{
    return strSource_.c_str();
}

const wxChar* OBTaskData::GetDestination ()
{
    return strDestination_.c_str();
}

const wxChar* OBTaskData::GetName ()
{
    return strName_.c_str();
}

bool OBTaskData::Verify ()
{
    return bVerify_;
}

OBArchiveFormat OBTaskData::GetArchiveFormat ()
{
    return archiveFormat_;
}

const wxArrayString& OBTaskData::GetExclude ()
{
    return arrExclude_;
}

bool OBTaskData::SetOID (OBoid oid)
{
    if (OBRootTask::Instance().HasTask(oid))
        return false;

    oid_ = oid;

    return true;
}

void OBTaskData::SetTaskType (OBTaskType type)
{
    type_ = type;
}

OBoid OBTaskData::GetOID ()
{
    return oid_;
}

void OBTaskData::SetSource (const wxChar* source)
{
    if (source == NULL)
        return;

    strSource_ = source;
}

void OBTaskData::SetDestination (const wxChar* dest)
{
    if (dest == NULL)
        return;

    strDestination_ = dest;
}

void OBTaskData::SetName (const wxChar* name)
{
    if (name == NULL)
        return;

    strName_ = name;
}

void OBTaskData::SetVerify (bool verify)
{
    bVerify_ = verify;
}

void OBTaskData::SetArchiveFormat (OBArchiveFormat archiveFormat)
{
    archiveFormat_ = archiveFormat;
}

void OBTaskData::SetExclude (const wxArrayString& exclude)
{
    arrExclude_ = exclude;
}

wxString OBTask::GetTypeDescription ()
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

bool OBTask::Serialize (jbArchive& rA)
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
        OBTaskType      type;
        OBArchiveFormat archiveFormat;
        wxString        strSource, strDestination, strName;
        bool            bVerify;
        wxArrayString   arrExclude;
        OBoid           oid;

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
            OBSystem::Fatal(_("Unable to set an OID while loading a project file.\nThe file is corrupt!"), _T("OBTask::Serialize()"));
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

OBTask::OBTask (OBTaskType type,
                const wxChar* strSource,
                const wxChar* strDestination,
                const wxChar* strName,
                bool bVerify,
                OBArchiveFormat archiveFormat,
                wxArrayString& arrExclude)
      : OBTaskData(type, strSource, strDestination, strName, bVerify, archiveFormat, arrExclude)
{
}

OBTask::OBTask ()
{
}

//
/*virtual*/ OBTask::~OBTask ()
{
}


wxString OBTask::GetArchiveExtension()
{
    switch (GetArchiveFormat())
    {
        case CompressZIP:
            return _T("zip");
            break;
    }

    return _T("unknown");
}

bool OBTask::RunForArchive (ProgressWithMessage& rProgress)
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
            OBSystem::Fatal(_T("unknown archive type"), _T("OBTask::RunForArchive()"));
            break;
    }

    // delete temporary exclude array
    if (pArr != NULL)
        delete pArr;

    return true;
}


bool OBTask::RunForDirCopy (ProgressWithMessage& rProgress)
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


bool OBTask::RunForFileCopy(ProgressWithMessage& rProgress)
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


bool OBTask::Run (ProgressWithMessage& rProgress)
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

