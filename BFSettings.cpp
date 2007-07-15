/**
 * implementation of class BFSettings
 * 2007-04-20
 ***/

#include "BFSettings.h"


/*static*/ BFSettings BFSettings::sSettings_;

//
BFSettings::BFSettings ()
          : bReplaceMacros_(false),
            bWithFiles_(false),
            bOpenLastProject_(false),
            lMaxLogFileSize_(1024)
{
}


//
/*virtual*/ BFSettings::~BFSettings ()
{
    int i = 0;
}


BFProjectSettings& BFSettings::GetDefaultProjectSettings ()
{
    return defaultPrj_;
}

bool BFSettings::GetReplaceMacros ()
{
    return bReplaceMacros_;
}

void BFSettings::SetReplaceMacros (bool bReplace)
{
    bReplaceMacros_ = bReplace;
}

bool BFSettings::GetWithFiles ()
{
    return bWithFiles_;
}

void BFSettings::SetWithFiles (bool bWithFiles)
{
    bWithFiles_ = bWithFiles;
}

long BFSettings::GetMaxLogFileSize ()
{
    return lMaxLogFileSize_;
}

void BFSettings::SetMaxLogFileSize (long lSizeInKiloByte)
{
    if (lSizeInKiloByte < 0)
        lMaxLogFileSize_ = 0;
    else
        lMaxLogFileSize_ = lSizeInKiloByte;
}

const wxArrayString& BFSettings::GetLastProjects ()
{
    return arrLastProjects_;
}

void BFSettings::SetLastProject (const wxChar* strFilename)
{
    if (strFilename == NULL)
        return;

    int idx = arrLastProjects_.Index(strFilename);

    if (idx != wxNOT_FOUND)
        arrLastProjects_.RemoveAt(idx);

    if (arrLastProjects_.GetCount() == 4)
        arrLastProjects_.RemoveAt(0);

    arrLastProjects_.Add(strFilename);
}

void BFSettings::SetOpenLastProject (bool bOpen)
{
    bOpenLastProject_ = bOpen;
}

bool BFSettings::GetOpenLastProject ()
{
    return bOpenLastProject_;
}

bool BFSettings::Serialize (jbArchive& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    rA.EnterObject();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        defaultPrj_.Serialize(rA);
        rA << bReplaceMacros_;
        rA << bWithFiles_;
        rA << (wxUint32&)lMaxLogFileSize_;
        rA << arrLastProjects_;
        rA << bOpenLastProject_;
    }
    else
    // ** serialize FROM file **
    {
        defaultPrj_.Serialize(rA);
        rA >> bReplaceMacros_;
        rA >> bWithFiles_;
        rA >> (wxUint32&)lMaxLogFileSize_;
        rA >> arrLastProjects_;
        rA >> bOpenLastProject_;
    }

    rA.LeaveObject();

    return true;
}
