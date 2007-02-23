/**
 * implementation of class OBLog
 * 2007-02-21
 ***/

#include "OBLog.h"
#include "OBCore.h"

OBLog::OBLog ()
     : Observer(&(OBSystem::Instance())),
       fileLog_(OB_LOGFILE_NAME, wxFile::write_append)
{
    CareSize();
}

/*virtual*/ OBLog::~OBLog ()
{
}

void OBLog::CareSize ()
{
    // check log-file size
    if (fileLog_.Length() > OB_LOGFILE_MAXSIZE)
    {
        // close the file
        fileLog_.Close();
        // backup the file
        OBCore::Instance().MoveFile(OB_LOGFILE_NAME, OB_LOGFILE_BAKNAME, true);
        // create a new empty file
        fileLog_.Open(OB_LOGFILE_NAME, wxFile::write_append);
    }
}

/*virtual*/ void OBLog::ValueChanged (Subject* pSender)
{
    OBSystem* pSys = dynamic_cast<OBSystem*>(pSender);

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
        pSys->GetLastMessage(),
        pSys->GetLastLocation()
    );
}

void OBLog::Do(OBMessageType type,
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
        strLog << wxString(OBSystem::GetTypeString(type)).MakeUpper();

    strLog << _T(" :");

    // log message
    if (strLocation == NULL)
        strLog << strMessage << _T('\n');
    else
        strLog << _T(" in ") << strLocation << _T('\n');

    // write to the file
    fileLog_.Write(strLog);
}
