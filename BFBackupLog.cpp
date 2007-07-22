/**
 * implementation of class BFBackupLog
 * 2007-04-20
 ***/

#include "BFBackupLog.h"
#include "BFSystem.h"
#include "BFRootTask.h"


BFTaskLog::BFTaskLog (BFTask& rTask)
         : rTask_(rTask),
           bNormalEnd_(false),
           lCountWarnings_(0),
           lCountErrors_(0),
           lCountFatal_(0)
{
}

/*virtual*/ BFTaskLog::~BFTaskLog ()
{
}

void BFTaskLog::Started ()
{
    timeStart_ = wxDateTime::Now();
}

void BFTaskLog::Finished (bool bNormalEnd /*= true*/)
{
    timeEnd_ = wxDateTime::Now();
    bNormalEnd_ = bNormalEnd;
    Write();
}

void BFTaskLog::Message (BFMessageType type,
                         const wxDateTime& timestamp,
                         const wxChar* strMessage,
                         const wxChar* strLocation)
{
    wxString str;

    str << timestamp.Format(_T("%Y-%m-%d %H:%M:%S "));
    str << _T('[') << BFSystem::GetTypeString(type) << _T(']');
    str << _T(' ') << strMessage << _T('\n');

    arrLog_.Add(str);

    switch(type)
    {
        case MsgWARNING:
            ++lCountWarnings_;
            break;

        case MsgERROR:
            ++lCountErrors_;
            break;

        case MsgFATAL:
            ++lCountFatal_;
            break;
    };
}

bool BFTaskLog::Write ()
{
    // open log-file
    wxString strFilename(GetFileName());
    wxFile file(strFilename, wxFile::write);

    if ( !(file.IsOpened()) )
    {
        BFSystem::Fatal(wxString::Format(_("can not create/open the log-file %s"), strFilename.c_str()), _T("BFTaskLog::Write()"));
        return false;
    }

    // remember the name of the logfile
    BFRootTask::Instance().AddTaskLogFile(strFilename);

    // write the log
    wxString strLine;

    strLine.Clear();
    strLine << _("Task: ") << rTask_.GetName() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Type: ") << rTask_.GetTypeDescription() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Project: ") << BFRootTask::Instance().GetName() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Source: ") << rTask_.GetSource() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Destination: ") << rTask_.GetDestination() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Task-Start:\t") << timeStart_.Format(_T("%Y-%m-%d %H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Task-End:\t") << timeEnd_.Format(_T("%Y-%m-%d %H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    //strLine << _("Task-Time: ") << timeEnd_.Subtract(timeStart_).Format(_T("%Y-%m-%d %H:%M:%S"));
    strLine << _("Task-Time:\t") << timeEnd_.Subtract(timeStart_).Format(_T("%H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level: ")
            << BFProjectSettings::GetStopLevelString(BFRootTask::Instance().GetSettings().GetStopLevelOnWarning()) << _T(" Warnings, ")
            << BFProjectSettings::GetStopLevelString(BFRootTask::Instance().GetSettings().GetStopLevelOnError()) << _T(" Errors, ")
            << BFProjectSettings::GetStopLevelString(BFRootTask::Instance().GetSettings().GetStopLevelOnFatal()) << _T(" Fatal Errors\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Verbose-Level: ") << BFProjectSettings::GetVerboseString(BFRootTask::Instance().GetSettings().GetVerboseLevel()) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("\nlogged messages:") << _T("\n");
    file.Write(strLine);

    for (int i = 0; i < arrLog_.GetCount(); ++i)
        file.Write(arrLog_[i]);

    return true;
}

wxString BFTaskLog::GetFileName ()
{
    wxString str;
    str << rTask_.GetDestination() << wxFILE_SEP_PATH << rTask_.GetName() << _T(".log");
    BFTaskBase::ReplaceMacros(str);
    return str;
}

wxString BFTaskLog::GetSummary ()
{
    wxString str;

    // start
    str << timeStart_.Format(_T("%Y-%m-%d %H:%M:%S")) << _(" : Task \"") << rTask_.GetName() << _("\" started\n");

    // end
    str << timeEnd_.Format(_T("%Y-%m-%d %H:%M:%S")) << _(" : Task \"") << rTask_.GetName() << _T("\"");
    if (bNormalEnd_)
        str << _(" finished");
    else
        str << _(" skiped");
    str << _T('\n');

    // message count
    str << wxString::Format(_("Warnings: %d  Errors: %d  Fatal-Errors: %d\n"), lCountWarnings_, lCountErrors_, lCountFatal_);

    // log-file
    str << _("Log-File: ") << GetFileName();

    return str;
}


//
BFBackupLog::BFBackupLog ()
           : Observer(&(BFSystem::Instance())),
             bIsSaved_(false)
{
    BFSystem::Instance().IncrementBackupObservers();
}


//
/*virtual*/ BFBackupLog::~BFBackupLog ()
{
    if (!bIsSaved_)
        BackupFinished();

    BFSystem::Instance().DecrementBackupObservers();
}


/*virtual*/ void BFBackupLog::ValueChanged (Subject* pSender)
{
    // init
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    BF_VerboseLevel vLvl = BFRootTask::Instance().GetSettings().GetVerboseLevel();
    BFMessageType msgType = pSys->GetLastType();

    // check verboseLevel and messageType
    switch (vLvl)
    {
        case BFVERBOSE_FATAL:
            if (msgType != MsgFATAL)
                return;
            break;

        case BFVERBOSE_ERROR:
            if (msgType != MsgFATAL
             && msgType != MsgERROR)
                return;
            break;

        case BFVERBOSE_WARNING:
            if (msgType != MsgFATAL
             && msgType != MsgERROR
             && msgType != MsgWARNING)
                return;
            break;

        case BFVERBOSE_INFO:
            if (msgType != MsgFATAL
             && msgType != MsgERROR
             && msgType != MsgWARNING
             && msgType != MsgBACKUP)
                return;
            break;
    };

    vecTaskLogs_.back()->Message
    (
        pSys->GetLastType(),
        pSys->GetLastTimestamp(),
        pSys->GetLastMessage(),
        pSys->GetLastLocation()
    );

    // do something on warning?
    if (msgType == MsgWARNING)
    {
        switch (BFRootTask::Instance().GetSettings().GetStopLevelOnWarning())
        {
            case BFDO_STOPPRJ:
                BFRootTask::Instance().StopProject();
                break;

            case BFDO_STOPTSK:
                BFRootTask::Instance().StopCurrentTask();
                break;

            case BFDO_ASK:
                break;

            case BFDO_IGNORE:
                break;
        };
    }

    // do something on errors?
    if (msgType == MsgERROR)
    {
        switch (BFRootTask::Instance().GetSettings().GetStopLevelOnError())
        {
            case BFDO_STOPPRJ:
                BFRootTask::Instance().StopProject();
                break;

            case BFDO_STOPTSK:
                BFRootTask::Instance().StopCurrentTask();
                break;

            case BFDO_ASK:
                break;

            case BFDO_IGNORE:
                break;
        };
    }

    // do something on fatal error?
    if (msgType == MsgFATAL)
    {
        switch (BFRootTask::Instance().GetSettings().GetStopLevelOnFatal())
        {
            case BFDO_STOPPRJ:
                BFRootTask::Instance().StopProject();
                break;

            case BFDO_STOPTSK:
                BFRootTask::Instance().StopCurrentTask();
                break;

            case BFDO_ASK:
                break;

            case BFDO_IGNORE:
                break;
        };
    }
}

bool BFBackupLog::WhileBackup ()
{
    if (timeStart_ > timeEnd_)
        return true;

    return false;
}

void BFBackupLog::BackupStarted ()
{
    BFRootTask::Instance().ClearLastLogFiles();
    timeStart_ = wxDateTime::Now();
}

void BFBackupLog::BackupFinished ()
{
    timeEnd_ = wxDateTime::Now();
    bIsSaved_ = Write();
}

void BFBackupLog::TaskStarted (BFTask& rTask)
{
    vecTaskLogs_.push_back(new BFTaskLog(rTask));
    vecTaskLogs_.back()->Started();
}

void BFBackupLog::TaskFinished ()
{
    vecTaskLogs_.back()->Finished();
}

bool BFBackupLog::Write ()
{
    BFRootTask& rPrj = BFRootTask::Instance();

    // create filename
    wxString strFile;
    strFile = rPrj.GetSettings().GetBackupLogLocation();
    strFile = strFile + wxFILE_SEP_PATH + rPrj.GetName() + _T(".log");
    BFTaskBase::ReplaceMacros(strFile);

    // remember the filename
    BFRootTask::Instance().SetProjectLogFile(strFile);

    // open log-file
    wxFile file(strFile, wxFile::write);

    if ( !(file.IsOpened()) )
    {
        BFSystem::Fatal(wxString::Format(_("can not create/open the log-file %s"), strFile.c_str()), _T("BFBackupLog::Write()"));
        return false;
    }

    // project info
    wxString strLine;

    strLine.Clear();
    strLine << _("Project: ") << rPrj.GetName() << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("File: ") << strFile << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Backup-Start:\t") << timeStart_.Format(_T("%Y-%m-%d %H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Backup-End: \t") << timeEnd_.Format(_T("%Y-%m-%d %H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    //strLine << _("Backup-Time: ") << timeEnd_.Subtract(timeStart_).Format(_T("%Y-%m-%d %H:%M:%S"));
    strLine << _("Backup-Time:\t") << timeEnd_.Subtract(timeStart_).Format(_T("%H:%M:%S")) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level: ")
            << BFProjectSettings::GetStopLevelString(rPrj.GetSettings().GetStopLevelOnWarning()) <<  _(" Warnings, ")
            << BFProjectSettings::GetStopLevelString(rPrj.GetSettings().GetStopLevelOnError()) << _(" Errors, ")
            << BFProjectSettings::GetStopLevelString(rPrj.GetSettings().GetStopLevelOnFatal()) << _(" Fatal Errors \n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Verbose-Level: ") << BFProjectSettings::GetVerboseString(rPrj.GetSettings().GetVerboseLevel()) << _T("\n");
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Number of Tasks: ") << vecTaskLogs_.size() << _T("\n");
    file.Write(strLine);

    for (int i = 0; i < vecTaskLogs_.size(); ++i)
    {
        strLine.Clear();
        strLine << _T('\n') << vecTaskLogs_[i]->GetSummary();
        file.Write(strLine);
    }

    return true;
}
