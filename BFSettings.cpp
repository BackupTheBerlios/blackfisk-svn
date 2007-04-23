/**
 * implementation of class BFSettings
 * 2007-04-20
 ***/

#include "BFSettings.h"

#define BFSETTINGS_FILENAME "blackfisk.set"

//
BFSettings::BFSettings ()
          : bReplaceMacros_(false),
            bWithFiles_(false)
{
}


//
/*virtual*/ BFSettings::~BFSettings ()
{
}


const BFProjectSettings& BFSettings::GetDefaultProjectSettings ()
{
    return defaultPrj_;
}

void BFSettings::SetDefaultProjectSettings (const BFProjectSettings& rDefaultPrj)
{
    defaultPrj_ = rDefaultPrj;
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

void BFSettings::SetMaxLogFileSize (long lSize)
{
    if (lSize < 0)
        lMaxLogFileSize_ = 0;
    else
        lMaxLogFileSize_ = lSize;
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
    }
    else
    // ** serialize FROM file **
    {
        defaultPrj_.Serialize(rA);
        rA >> bReplaceMacros_;
        rA >> bWithFiles_;
        rA >> (wxUint32&)lMaxLogFileSize_;
    }

    rA.LeaveObject();

    return true;
}
