/**
 * Name:        BFTask.cpp
 * Purpose:     BFTask class implementation
 * Author:      Christian Buhtz
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
//#include "BFCore.h"
#include "BFProject.h"
#include "BFundef.h"
#include "BFSystem.h"
#include "blackfisk.h"

BFTask::BFTask (BFTaskType type,
                    const wxString& strSource,
                    const wxString& strDestination,
                    const wxString& strName,
                    bool bVerify,
                    bool bVerifyContent,
                    BFArchiveFormat archive,
                    wxArrayString& arrExclude,
                    bool bRealSync)
          : type_(type),
            strSource_(strSource),
            strDestination_(strDestination),
            strName_(strName),
            bVerify_(bVerify),
            bVerifyContent_(bVerifyContent),
            archiveFormat_(archive),
            arrExclude_(arrExclude),
            bRealSync_(bRealSync),
			bCallSetDestination_(false)
{
    oid_ = BFProject::Instance().CreateOID();
}

BFTask::BFTask ()
      : oid_(BFInvalidOID),
        type_(TaskINVALID),
        bVerify_(true),
        bVerifyContent_(false),
        archiveFormat_(CompressNOTUSED),
        bRealSync_(true),
		bCallSetDestination_(false)
{
}

/*virtual*/ BFTask::~BFTask ()
{
}

bool BFTask::IsValid ()
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

BFTaskType BFTask::GetType () const
{
    return type_;
}

const wxString& BFTask::GetSource () const
{
    return strSource_;
}

const wxString& BFTask::GetDestination () const
{
    return strDestination_;
}

const wxString& BFTask::GetName () const
{
    return strName_;
}

bool BFTask::Verify () const
{
    return bVerify_;
}

bool BFTask::VerifyContent () const
{
    return bVerifyContent_;
}

BFArchiveFormat BFTask::GetArchiveFormat () const
{
    return archiveFormat_;
}

const wxArrayString& BFTask::GetExclude () const
{
    return arrExclude_;
}

bool BFTask::GetRealSync () const
{
    return bRealSync_;
}

bool BFTask::SetOID (BFoid oid)
{
    if (BFProject::Instance().HasTask(oid))
        return false;

    oid_ = oid;

    return true;
}

void BFTask::SetTaskType (BFTaskType type)
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
}

BFoid BFTask::GetOID () const
{
    return oid_;
}

void BFTask::SetSource (const wxString& source)
{
    strSource_ = source;
}

bool BFTask::SetDestination (const wxString& dest)
{
	bCallSetDestination_ = true;
	wxString strD = dest;

    // upper case the drive letter
    strD.SetChar(0, wxString(strD[0]).MakeUpper()[0]);

	if ( BFProject::Instance().HandleNewDestination (this, strD ) )
	{
		// set destination to the task
		strDestination_ = strD;

		return true;
	}

	bCallSetDestination_ = false;

	return false;
}

void BFTask::SetName (const wxString& name)
{
    strName_ = name;
}

void BFTask::SetVerify (bool verify)
{
    bVerify_ = verify;
}

void BFTask::SetVerifyContent (bool verify_content)
{
    bVerifyContent_ = verify_content;
}

void BFTask::SetArchiveFormat (BFArchiveFormat archiveFormat)
{
    archiveFormat_ = archiveFormat;
}

void BFTask::SetExclude (const wxArrayString& exclude)
{
    arrExclude_ = exclude;
}

void BFTask::SetRealSync (bool bRealSync)
{
    bRealSync_ = bRealSync;
}

wxString BFTask::GetTypeDescription ()
{
    return BFProject::GetTypeDescription(GetType(), GetArchiveFormat());
}

void BFTask::GetAvailableTypes (BFTypeVector& rVecType)
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

wxString BFTask::GetArchiveExtension()
{
    return BFProject::GetArchiveExtension(GetArchiveFormat());
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
        rA << VerifyContent();
        rA << GetArchiveFormat();
        rA << GetExclude();
        rA << GetRealSync();
    }
    else
    // ** serialize FROM file **
    {
        BFTaskType      type;
        BFArchiveFormat archiveFormat;
        wxString        strSource, strDestination, strName;
        bool            bVerify, bVerifyContent, bRealSync;
        wxArrayString   arrExclude;
        BFoid           oid;

        rA >> oid;
        rA >> type;
        rA >> strSource;
        rA >> strDestination;
        rA >> strName;
        rA >> bVerify;

        // implemented at version 1010
        if (rA.GetVersion() < 1010)
        {
            bVerifyContent = false;
            BFProject::Instance().SetModified(true);
        }
        else
        {
            rA >> bVerifyContent;
        }

        rA >> archiveFormat;
        rA >> arrExclude;

        // impelemented at version 1020
        if (rA.GetVersion() < 1020)
        {
            bRealSync = true;
        }
        else
        {
            rA >> bRealSync;
        }

        SetTaskType(type);
        SetSource(strSource);
		strDestination_ = strDestination;
        SetName(strName);
        SetVerify(bVerify);
        SetVerifyContent(bVerifyContent);
        SetArchiveFormat(archiveFormat);
        SetExclude(arrExclude);
        SetRealSync(bRealSync);

        if ( !(SetOID(oid)) )
        {
            BFSystem::Fatal("Unable to set an OID while loading a project file.\nThe file is corrupt!", "BFTask::Serialize()");
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

bool BFTask::IsWhileSetDestinationCall ()
{
	return bCallSetDestination_;
}
