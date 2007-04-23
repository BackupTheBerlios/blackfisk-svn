/**
 * implementation of class BFProjectSettings
 * 2007-04-20
 ***/

#include "BFProjectSettings.h"
#include "BFRootTask.h"
#include "BFApp.h"

//
BFProjectSettings::BFProjectSettings ()
{
    SetDefault();
}


//
/*virtual*/ BFProjectSettings::~BFProjectSettings ()
{
}

void BFProjectSettings::SetVerboseLevel (BF_VerboseLevel iLevel)
{
    iVerboseLevel_ = iLevel;
}

BF_VerboseLevel BFProjectSettings::GetVerboseLevel ()
{
    return iVerboseLevel_;
}

void BFProjectSettings::SetStopLevel (BF_StopLevel& iTarget, BF_StopLevel iStopLevel)
{
    iTarget = iStopLevel;
}


void BFProjectSettings::SetStopLevelOnFatal (BF_StopLevel iLevel)
{
    SetStopLevel (iStopLevelOnFatal_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnFatal ()
{
    return iStopLevelOnFatal_;
}

void BFProjectSettings::SetStopLevelOnError (BF_StopLevel iLevel)
{
    SetStopLevel (iStopLevelOnError_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnError ()
{
    return iStopLevelOnError_;
}

void BFProjectSettings::SetStopLevelOnWarning (BF_StopLevel iLevel)
{
    SetStopLevel(iStopLevelOnWarning_, iLevel);
}

BF_StopLevel BFProjectSettings::GetStopLevelOnWarning ()
{
    return iStopLevelOnWarning_;
}

const wxString& BFProjectSettings::GetBackupLogLocation ()
{
    return strBackupLogLocation_;
}

void BFProjectSettings::SetBackupLogLocation (const wxChar* strFile)
{
    if (strFile == NULL)
        return;

    strBackupLogLocation_ = strFile;
}

bool BFProjectSettings::Serialize (jbArchive& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    rA.EnterObject();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        rA << (int)iVerboseLevel_;
        rA << (int)iStopLevelOnFatal_;
        rA << (int)iStopLevelOnError_;
        rA << (int)iStopLevelOnWarning_;
        rA << strBackupLogLocation_;
    }
    else
    // ** serialize FROM file **
    {
        rA >> (int&)iVerboseLevel_;
        rA >> (int&)iStopLevelOnFatal_;
        rA >> (int&)iStopLevelOnError_;
        rA >> (int&)iStopLevelOnWarning_;
        rA >> strBackupLogLocation_;
    }

    rA.LeaveObject();

    return true;
}

void BFProjectSettings::SetDefault ()
{
    iVerboseLevel_          = BFVERBOSE_WARNING;
    iStopLevelOnFatal_      = BFDO_STOPPRJ;
    iStopLevelOnError_      = BFDO_STOPPRJ;
    iStopLevelOnWarning_    = BFDO_STOPPRJ;
    wxArrayString arrDest   = BFRootTask::Instance().GetDestinations();
    strBackupLogLocation_   = BFApp::ExtractCommunity(arrDest);
}
