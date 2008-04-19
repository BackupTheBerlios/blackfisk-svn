/**
 * Name:        BFBackupLog.cpp
 * Purpose:     BFBackupLog class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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


#include "BFBackupLog.h"
#include "BFSystem.h"
//#include "BFRootTask.h"
#include "BFRootTaskApp.h"
#include <wx/file.h>

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

    str << timestamp.Format("%Y-%m-%d %H:%M:%S ");
    str << '[' << BFSystem::GetTypeString(type) << ']';
    str << ' ' << strMessage << '\n';

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
        BFSystem::Fatal(wxString::Format("can not create/open the log-file %s", strFilename.c_str()), "BFTaskLog::Write()");
        return false;
    }

    // remember the name of the logfile
    BFRootTaskApp::Instance().AddTaskLogFile(strFilename);

    // write the log
    wxString strLine;

    strLine.Clear();
    strLine << _("Task: ") << rTask_.GetName() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Type: ") << rTask_.GetTypeDescription() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Project: ") << BFRootTaskApp::Instance().GetProjectName() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Source: ") << rTask_.GetSource() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Destination: ") << rTask_.GetDestination() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Task-Start:\t") << timeStart_.Format("%Y-%m-%d %H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Task-End:\t") << timeEnd_.Format("%Y-%m-%d %H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Task-Duration:\t") << timeEnd_.Subtract(timeStart_).Format("%H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Warnings:\t\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnWarning()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Errors:\t\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnError()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Fatal Errors:\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnFatal()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Verbose-Level: ") << BFSystem::GetVerboseString(BFRootTaskApp::Instance().GetVerboseLevel()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("\nlogged messages:") << "\n";
    file.Write(strLine);

    for (int i = 0; i < arrLog_.GetCount(); ++i)
        file.Write(arrLog_[i]);

    return true;
}

wxString BFTaskLog::GetFileName ()
{
    wxString str;
    str << rTask_.GetDestination() << wxFILE_SEP_PATH << rTask_.GetName() << ".log";
    BFTask::FillBlackfiskPlaceholders(str);
    return str;
}

wxString BFTaskLog::GetSummary ()
{
    wxString str;

    // start
    str << timeStart_.Format("%Y-%m-%d %H:%M:%S") << _(" : Task \"") << rTask_.GetName() << _("\" started\n");

    // end
    str << timeEnd_.Format("%Y-%m-%d %H:%M:%S") << _(" : Task \"") << rTask_.GetName() << "\"";
    if (bNormalEnd_)
        str << _(" finished");
    else
        str << _(" stoped");
    str << '\n';

    // message count
    str << wxString::Format(_("Warnings: %d  Errors: %d  Fatal-Errors: %d\n"), lCountWarnings_, lCountErrors_, lCountFatal_);

    // log-file
    str << _("Log-File: ") << GetFileName() << "\n";

    return str;
}


//
BFBackupLog::BFBackupLog ()
           : Observer(&(BFSystem::Instance()), 200),
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

BFTaskLog* BFBackupLog::GetCurrentTaskLog ()
{
    return vecTaskLogs_.back();
}

/*virtual*/ void BFBackupLog::ValueChanged (Subject* pSender)
{
    // init
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    if (pSys == NULL)
        return;

    if ( pSys->HandleLastMessage(BFRootTaskApp::Instance().GetVerboseLevel()) )
    {
        if (vecTaskLogs_.back() != NULL)
            vecTaskLogs_.back()->Message
            (
                pSys->GetLastType(),
                pSys->GetLastTimestamp(),
                pSys->GetLastMessage(),
                pSys->GetLastLocation()
            );
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
    BFRootTaskApp::Instance().ClearLastLogFiles();
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

void BFBackupLog::TaskStoped ()
{
    vecTaskLogs_.back()->Finished(false);
}

bool BFBackupLog::Write ()
{
    // create filename
    wxString strFile;
    strFile = BFRootTaskApp::Instance().GetBackupLogLocation();
    strFile = strFile + wxFILE_SEP_PATH + BFRootTaskApp::Instance().GetProjectName() + ".log";
    BFTask::FillBlackfiskPlaceholders(strFile);

    // remember the filename
    BFRootTaskApp::Instance().SetProjectLogFile(strFile);

    // open log-file
    wxFile file(strFile, wxFile::write);

    if ( !(file.IsOpened()) )
    {
        BFSystem::Fatal(wxString::Format("can not create/open the log-file %s", strFile.c_str()), "BFBackupLog::Write()");
        return false;
    }

    // project info
    wxString strLine;

    strLine.Clear();
    strLine << _("Project: ") << BFRootTaskApp::Instance().GetProjectName() << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("File: ") << strFile << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Backup-Start:\t") << timeStart_.Format("%Y-%m-%d %H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Backup-End: \t") << timeEnd_.Format("%Y-%m-%d %H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Backup-Duration:\t") << timeEnd_.Subtract(timeStart_).Format("%H:%M:%S") << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Verbose-Level:\t") << BFSystem::GetVerboseString(BFRootTaskApp::Instance().GetVerboseLevel()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Warnings:\t\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnWarning()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Errors:\t\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnError()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Stop-Level on Fatal Errors:\t") << BFRootTaskApp::GetStopLevelString(BFRootTaskApp::Instance().GetStopLevelOnFatal()) << "\n";
    file.Write(strLine);

    strLine.Clear();
    strLine << _("Number of Tasks:\t") << vecTaskLogs_.size() << "\n";
    file.Write(strLine);

    for (int i = 0; i < vecTaskLogs_.size(); ++i)
    {
        strLine.Clear();
        strLine << '\n' << vecTaskLogs_[i]->GetSummary();
        file.Write(strLine);
    }

    return true;
}
