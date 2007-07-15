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
    if (BFRootTask::Instance().Has(this))
    {
        if (strBackupLogLocation_.IsEmpty())
            strBackupLogLocation_ = BFApp::ExtractCommunity(BFRootTask::Instance().GetDestinations());

        if (strBackupLogLocation_.IsEmpty())
            if (BFRootTask::Instance().GetDestinations().GetCount() > 0)
                strBackupLogLocation_ = BFRootTask::Instance().GetDestinations()[0];
    }

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
    iStopLevelOnFatal_      = BFDO_ASK;
    iStopLevelOnError_      = BFDO_ASK;
    iStopLevelOnWarning_    = BFDO_ASK;
    wxArrayString arrDest   = BFRootTask::Instance().GetDestinations();
    strBackupLogLocation_   = BFApp::ExtractCommunity(arrDest);
}

/*static*/ wxString BFProjectSettings::GetVerboseString (BF_VerboseLevel verboseLevel)
{
    switch (verboseLevel)
    {
        case BFVERBOSE_ERROR:
            return _("Error");

        case BFVERBOSE_FATAL:
            return _("Fatal Error");

        case BFVERBOSE_WARNING:
            return _("Warning");

        case BFVERBOSE_INFO:
            return _("Information");
    };

    return _("unknown verbose level");
}

/*static*/ wxString BFProjectSettings::GetStopLevelString (BF_StopLevel stopLevel)
{
    switch (stopLevel)
    {
        case BFDO_ASK:
            return _("ask");

        case BFDO_IGNORE:
            return _("ignore just log");

        case BFDO_STOPTSK:
            return _("stop task");

        case BFDO_STOPPRJ:
            return _("stop project");
    };

    return _("unknown stopLevel");
}
