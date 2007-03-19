/**
 * implementation of class BFLog
 * 2007-02-21
 ***/

#include "BFLog.h"
#include "BFCore.h"

BFLog::BFLog ()
     : Observer(&(BFSystem::Instance())),
       fileLog_(BF_LOGFILE_NAME, wxFile::write_append)
{
    CareSize();
}

/*virtual*/ BFLog::~BFLog ()
{
}

void BFLog::CareSize ()
{
    // check log-file size
    if (fileLog_.Length() > BF_LOGFILE_MAXSIZE)
    {
        // close the file
        fileLog_.Close();
        // backup the file
        BFCore::Instance().MoveFile(BF_LOGFILE_NAME, BF_LOGFILE_BAKNAME, true);
        // create a new empty file
        fileLog_.Open(BF_LOGFILE_NAME, wxFile::write_append);
    }
}

/*virtual*/ void BFLog::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    // check log-level
    if (pSys->GetLastType() < pSys->GetLogLevel())
        return;

    // log to file
    Do
    (
        pSys->GetLastType(),
        pSys->GetLastTimestamp(),
        pSys->GetLastMessage().c_str(),
        pSys->GetLastLocation().c_str()
    );
}

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
