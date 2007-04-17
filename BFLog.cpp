/**
 * implementation of class BFLog
 * 2007-02-21
 ***/

#include "BFLog.h"
#include "BFCore.h"

BFLogBase::BFLogBase (const wxChar* strLogFileName, long lMaxSize /*= 0*/ )
     : Observer(&(BFSystem::Instance())),
       fileLog_(strLogFileName, wxFile::write_append),
       strLogFileName_(strLogFileName),
       lMaxSize_(lMaxSize)
{
    CareSize();
}

/*virtual*/ BFLogBase::~BFLogBase ()
{
}

long BFLogBase::GetMaxSize ()
{
    return lMaxSize_;
}

void BFLogBase::SetMaxSize (long lSize)
{
    lMaxSize_ = lSize;
}

void BFLogBase::CareSize ()
{
    // the size does not matter
    if (GetMaxSize() <= 0)
        return;

    if ( !(fileLog_.IsOpened()) )
        return;

    // check log-file size
    if (fileLog_.Length() > GetMaxSize())
    {
        // close the file
        fileLog_.Close();
        // backup the file
        BFCore::Instance().MoveFile(strLogFileName_, strLogFileName_ + BF_LOGFILE_BAKSUFFIX, true);
        // create a new empty file
        fileLog_.Open(strLogFileName_, wxFile::write_append);
    }
}

void BFLogBase::Do(BFMessageType type,
                   const wxDateTime& timestamp,
                   const wxChar* strMessage,
                   const wxChar* strLocation)
{
    // check arguments
    if (strMessage == NULL)
        return;

    // result string written to the file
    wxString strLog;

    // create timestamp
    strLog << timestamp.Format(_T("%Y-%m-%d %H:%M:%S"));

    // message type identifier
    if (type != MsgLOG)
        strLog << _T(" ") << wxString(BFSystem::GetTypeString(type)).MakeUpper() << _T(" ");

    strLog << _T(": ");

    // log message
    if (wxStrlen(strLocation) == 0)
        strLog << strMessage << _T('\n');
    else
        strLog << strMessage << _T(" in ") << strLocation << _T('\n');

    // write to the file
    fileLog_.Write(strLog);
}


/*virtual*/ void BFLog::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    // check log-level
    if (pSys->GetLastType() < pSys->GetLogLevel())
    {
        // check for message of type MsgBACKUP
        if (pSys->GetLastType() != MsgBACKUP)
        {
            return;
        }
        else
        {
            // handle MsgBACKUP just if there is no other observer for it
            if (pSys->GetBackupObservers() > 0)
                return;
        }
    }


    // log to file
    Do
    (
        pSys->GetLastType(),
        pSys->GetLastTimestamp(),
        pSys->GetLastMessage().c_str(),
        pSys->GetLastLocation().c_str()
    );
}


BFLog::BFLog (const wxChar* strLogFileName)
     : BFLogBase(strLogFileName, BF_LOGFILE_MAXSIZE)
{
}

/*virtual*/ BFLog::~BFLog ()
{
}
