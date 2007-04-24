/**
 * implementation of class BFLog
 * 2007-02-21
 ***/

#include "BFLog.h"
#include "blackfisk.h"
#include "BFSettings.h"

void BFLog::Do(BFMessageType type,
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


BFLog::BFLog ()
     : BFLogBase(BF_LOGFILE_NAME, BFSettings::Instance().GetMaxLogFileSize())
{
}

/*virtual*/ BFLog::~BFLog ()
{
}
