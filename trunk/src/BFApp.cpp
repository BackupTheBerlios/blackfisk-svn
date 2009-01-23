/**
 * Name:        BFApp.cpp
 * Purpose:     BFApp class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-05
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#include "BFApp.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/protocol/ftp.h>
#include <wx/snglinst.h>
#include <wx/msw/registry.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFBackupProgressDlg.h"
#include "BFTaskDlg.h"
#include "BFBackup.h"
#include "Progress.h"
#include "BFSettings.h"
#include "BFLog.h"
#include "BFwxLog.h"
#include "BFEnvironment.h"

IMPLEMENT_APP(BFApp);


BFMainFrame* BFApp::spMainFrame_ = NULL;

/*static*/ BFMainFrame* BFApp::MainFrame ()
{
    return spMainFrame_;
}

/*static*/ void BFApp::SetMainFrame (BFMainFrame* pMainFrame)
{
    spMainFrame_ = pMainFrame;
}

/*static*/ wxString BFApp::GetFullApplicationName ()
{
    return wxString::Format("%s %d.%d.%d %s",
                               BF_PRGNAME,
                               BF_VERSION_MAJOR,
                               BF_VERSION_MINOR,
                               BF_VERSION_RELEASE,
                               BF_VERSION_EXTENSION);
}

/*static*/ wxString BFApp::GetVersion ()
{
    return wxString::Format("%d.%d.%d %s",
                            BF_VERSION_MAJOR,
                            BF_VERSION_MINOR,
                            BF_VERSION_RELEASE,
                            BF_VERSION_EXTENSION);
}

/*static*/ wxArrayString& BFApp::PrependString (wxArrayString& rStrings,
                                                const wxString& strToPrepend)
{
    for (size_t i = 0; i < rStrings.GetCount(); ++i)
        rStrings[i].Prepend(strToPrepend);

    return rStrings;
}

/*static*/ wxString BFApp::ExtractCommunity (const wxArrayString& rStrings)
{
    wxString str;

    if (rStrings.GetCount() == 0)
        return str;

    size_t iCur = 0;
    wxChar cCur = _T('\0');

    while (true)
    {
        for (size_t i = 0; i < rStrings.GetCount(); ++i)
        {
            // check string length
            if (rStrings[i].Len() < (iCur+1))
                return str;

            if (cCur == _T('\0'))
                cCur = rStrings[i][iCur];

            if (cCur != rStrings[i][iCur])
                return str;
        }

        str = str + cCur;
        cCur = _T('\0');
        ++iCur;
    }

    return str;
}

BFApp::BFApp ()
     : pLog_(NULL),
       pSingleInstanceChecker_(NULL),
       strCmdOpen_(wxEmptyString),
       bCmdUsage_(false)
{
}

/*virtual*/ BFApp::~BFApp ()
{
}

void BFApp::ParseCmdLine ()
{
    //wxApp::argc;
    //wxApp::argv;

    wxString str;

    for (int i = 1; i < argc; ++i)
    {
        str = argv[i];

        if ( str.StartsWith("-h")
          || str.StartsWith("/h")
          || str.StartsWith("--help")
          || str.StartsWith("-?")
          || str.StartsWith("/?") )
        {
            bCmdUsage_ = true;
        }
        else if ( str.StartsWith("-r")
               || str.StartsWith("/r")
               || str.StartsWith("--run") )
        {
            if (argc > i)
            {
                ++i;
                strCmdOpen_ = argv[i];
                BFEnvironment::SetProjectScheduled(true);
            }
        }
        else
        {
            strCmdOpen_ = argv[i];
        }
    }
}

void BFApp::DisplayUsage ()
{
    wxMessageOutput* out = wxMessageOutput::Get();

    out->Printf("");
    out->Printf("=== %s === (GPLv3 licensed)", GetFullApplicationName());
    out->Printf("USAGE: %s [projectfile] [-r|--run projectfile] [-h|--help]", BF_PRGNAME);
    out->Printf("");
    out->Printf("  [projectfile]            Open the project.");
    out->Printf("");
    out->Printf("  -r|--run projectfile     Run the project.");
    out->Printf("");
    out->Printf("  -h|--help                Display this text.");
}

bool BFApp::OnInit()
{
    // check if another instance is running
    pSingleInstanceChecker_ = new wxSingleInstanceChecker
                                    (
                                        wxString::Format("%s-%s",
                                                         GetFullApplicationName(),
                                                         wxGetUserId())
                                    );

    if ( pSingleInstanceChecker_->IsAnotherRunning() )
    {
        wxMessageBox("Another program instance is already running!\nAborting...");
        return false;
    }

    //
    BFEnvironment::RememberApplicationDirectory(argv);

    //
    pLog_ = new BFLog();

    // log target
    wxLog::SetActiveTarget(new BFwxLog);

    wxString strToOpen;

    // parse command line
    ParseCmdLine ();

    // usage?
    if ( bCmdUsage_ )
    {
        DisplayUsage ();
        return false;
    }

    // run?
    if ( BFEnvironment::IsProjectScheduled() )
    {
        if ( strCmdOpen_.IsEmpty() )
        {
            DisplayUsage();
            return false;
        }
    }

    // log application start
    BFSystem::Log(wxString::Format(_("Starting %s ..."), GetFullApplicationName()));

    // read the configuration file
    BFSettings::Instance().InitDefaultValues();
    ReadSettings();

    // init locals
    locale_.Init( BFSettings::Instance().GetLanguage(), wxLOCALE_CONV_ENCODING);
    locale_.AddCatalogLookupPathPrefix(".\\locale");
    locale_.AddCatalog("bf");

    // check the scheduler
    if ( BFSettings::Instance().GetScheduler() == 1 )
    {
        if ( IsSchedulerInAutostart() == false )
        {
            BFSystem::Info
            (
                _("Blackfisk's own scheduler wxCron is selected to handle backup tasks." \
                "\nThe scheduler is setup to run automaticly on system startup!" \
                "\n\nSee \"Global Settings/Scheduler\" to change this behaviour.")
            );
            SetSchedulerInAutostart();
        }
    }

    // open the last project ?
    if (strCmdOpen_.IsEmpty())
        if (BFSettings::Instance().GetOpenLastProject())
            strCmdOpen_ = BFSettings::Instance().GetLastProject();

    if ( !(strCmdOpen_.IsEmpty()) )
        OpenProject(strCmdOpen_);

    /* init the main frame
       'BFApp::spMainFrame_' is set by the ctor of BFMainFrame itself */
    new BFMainFrame();

    //
    BFSystem::Log(wxString::Format(_("application verbose level: %s"), BFSystem::GetTypeString(BFSettings::Instance().GetVerboseLevelLog()).c_str()));

    // run automaticly?
    if ( BFEnvironment::IsProjectScheduled() )
    {
        BFSystem::Log(wxString::Format(_("Run the project %s automaticly..."), strCmdOpen_));

        // start the backup
        if ( BFBackup::Instance().PreBackupCheck() )
        {
            new BFBackupProgressDlg(BFMainFrame::Instance());
        }
        else
        {
            BFSystem::Warning(_("PreBackupCheck failed. Aborting ..."));
            return false;
        }
    }

    return true;
}

/*virtual*/ int BFApp::OnExit()
{
    SaveSettings();
    BFSystem::Log(wxString::Format(_("%s closed\n"), BF_PRGNAME).c_str());

    if (pLog_)
        delete pLog_;

    if (pSingleInstanceChecker_)
        delete pSingleInstanceChecker_;

    return 0;
}


/*static*/ bool BFApp::ReadSettings ()
{
    if ( !(wxFileName::FileExists(BFEnvironment::GetSettingsFileName())) )
        SaveSettings();

    wxFileInputStream   in(BFEnvironment::GetSettingsFileName());
    jbSerialize         archive(in, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

/*static*/ bool BFApp::SaveSettings ()
{
    wxFileOutputStream  out(BFEnvironment::GetSettingsFileName());
    jbSerialize         archive(out, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

const wxString& BFApp::GetCurrentProjectFilename ()
{
    return BFBackup::Instance().GetCurrentFilename();
}

wxString BFApp::GetCurrentProjectName ()
{
    return wxString(BFBackup::Instance().GetProjectName());
}

bool BFApp::OpenProject (const wxString& strFilename)
{
    bool rc = BFBackup::Instance().ReadFromFile(strFilename);

    if (rc && !(BFEnvironment::IsProjectScheduled()))
    {
        BFSettings::Instance().SetLastProject(strFilename);
        SaveSettings();
    }

    return rc;
}

bool BFApp::SaveProject (const wxString& strFilename)
{
    bool rc = BFBackup::Instance().StoreToFile(strFilename);

    if (rc && !(BFEnvironment::IsProjectScheduled()))
    {
        BFSettings::Instance().SetLastProject(strFilename);
        SaveSettings();
    }

    return rc;
}

bool BFApp::SaveCurrentProject ()
{
    return BFBackup::Instance().StoreToFile
    (
        BFBackup::Instance().GetCurrentFilename()
    );
}

bool BFApp::ResetProject ()
{
    BFBackup::Instance().Reset();

    return true;
}

#ifdef _DEBUG
void BFApp::Test ()
{
    BFSystem::Info("Info");
    BFSystem::Warning("Warning");
    BFSystem::Error("Error");
    BFSystem::Fatal("Fatal");
}
#endif

bool BFApp::IsNewVersionAvailable ()
{
    // compare with newest version
    return GetVersion() == GetNewestVersion();
}

const wxString& BFApp::GetNewestVersion ()
{
    static const wxString s_strNewestVersionString = ReadFileFromFTP (BF_FTP_URL, BF_FTP_DIR, BF_FTP_FILE_VER);

    BFSettings::Instance().SetLastVersionCheck(wxDateTime::Now());

    return s_strNewestVersionString;
}

wxString BFApp::ReadFileFromFTP (const wxString& strFtpServer,
                                 const wxString& strFtpPath,
                                 const wxString& strFtpFile,
                                 const wxString& strFtpUser /*= wxEmptyString*/,
                                 const wxString& strFtpPwd /*= wxEmptyString*/)
{
    BFSystem::Log(wxString::Format("Try to connect FTP-Server: %s, path: %s, file: %s", strFtpServer, strFtpPath, strFtpFile));
    // create ftp-object
    wxFTP ftp;

    // set user if needed
    if ( !(strFtpUser.IsEmpty()) )
        ftp.SetUser(strFtpUser);

    // set password if needed
    if ( !(strFtpPwd.IsEmpty()) )
        ftp.SetPassword(strFtpPwd);

    // connect to server
    if ( !ftp.Connect(strFtpServer) )
    {
        BFSystem::Error(wxString::Format("Can not connect to the FTP-Server %s!"), strFtpServer);
        BFSystem::Log("FTP failed!");
        return wxEmptyString;
    }

    // go to path
    ftp.ChDir(strFtpPath);

    // check if file exists
    if ( !(ftp.FileExists(strFtpFile)) )
    {
        BFSystem::Error(wxString::Format("File %s doesn't exists!", strFtpFile));
        BFSystem::Log("FTP failed!");
        return wxEmptyString;
    }

    // get the input stream
    wxInputStream *in = ftp.GetInputStream(strFtpFile);
    if ( !in )
    {
        BFSystem::Error(wxString::Format("Can not get file %s!", strFtpFile));
        BFSystem::Log("FTP failed!");
        return wxEmptyString;
    }

    // init read buffers
    wxString strData;
    char c;

    while (true)
    {
        // get one character from stream
        c = in->GetC();

        // read successful?
        if (in->LastRead() == 1)
            // successful
            strData = strData + c;
        else
            // eof or error
            break;
    }

    // delete the stream
    delete in;

    // log end
    BFSystem::Log(wxString::Format("FTP successful! %d  characters read.", strData.Len()));

    return strData;
}

bool BFApp::IsSchedulerInAutostart ()
{
    wxRegKey key(BF_REGKEY_AUTOSTART);
    return key.HasValue(BF_REGKEY_VALUE);
}

void BFApp::SetSchedulerInAutostart ()
{
    wxRegKey key(BF_REGKEY_AUTOSTART);
    key.SetValue(BF_REGKEY_VALUE, BFEnvironment::GetApplicationDirectory() + wxFILE_SEP_PATH + "wxCron" + wxFILE_SEP_PATH + "wxCron.exe");
}

void BFApp::RemoveSchedulerFromAutostart ()
{
    wxRegKey key(BF_REGKEY_AUTOSTART);
    key.DeleteValue(BF_REGKEY_VALUE);
}
