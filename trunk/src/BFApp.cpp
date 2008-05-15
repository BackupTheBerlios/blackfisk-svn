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
#include <wx/sound.h>
#include <wx/protocol/ftp.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFBackupProgressDlg.h"
#include "BFTaskDlg.h"
#include "BFRootTaskApp.h"
#include "Progress.h"
#include "BFSettings.h"
#include "BFBackupQuestionDlg.h"
#include "BFCmdLine.h"

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
    return wxString::Format(_T("%s %d.%d.%d %s"),
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
                                                const wxChar* strToPrepend)
{
    if (strToPrepend)
    {
        for (size_t i = 0; i < rStrings.GetCount(); ++i)
            rStrings[i].Prepend(strToPrepend);
    }

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
{
}

/*virtual*/ BFApp::~BFApp ()
{
}

bool BFApp::OnInit()
{
    wxString strToOpen;

    // parse command line
    BFCmdLine cmdLine;
    cmdLine.SetCmdLine(wxApp::argc, wxApp::argv);
    cmdLine.Parse();
    if (cmdLine.GetParamCount() > 0)
        strToOpen = cmdLine.GetParam(0);

    // log application start
    BFSystem::Log(wxString::Format(_("Starting %s ..."), GetFullApplicationName()));

    // read the configuration file
    ReadSettings();

    // init locals
    locale_.Init( BFSettings::Instance().GetLanguage(), wxLOCALE_CONV_ENCODING);
    locale_.AddCatalogLookupPathPrefix(".\\locale");
    locale_.AddCatalog("bf");

    // open the last project ?
    if (strToOpen.IsEmpty())
        if (BFSettings::Instance().GetOpenLastProject())
            strToOpen = BFSettings::Instance().GetLastProject();

    if ( !(strToOpen.IsEmpty()) )
    {
        OpenProject(strToOpen);
        //MainFrame()->RefreshTitle();
    }

    /* init the main frame
       'BFApp::spMainFrame_' is set by the ctor of BFMainFrame itself */
    new BFMainFrame();

    //
    BFSystem::Log(wxString::Format(_("application verbose level: %s"), BFSystem::GetTypeString(BFSettings::Instance().GetVerboseLevelLog()).c_str()));

    return TRUE;
}

/*virtual*/ int BFApp::OnExit()
{
    SaveSettings();
    BFSystem::Log(wxString::Format(_("%s closed\n"), BF_PRGNAME).c_str());

    return 1;
}

/*static*/ bool BFApp::ReadSettings ()
{
    if ( !(wxFileName::FileExists(BF_SETTINGS)) )
        SaveSettings();

    wxFileInputStream   in(BF_SETTINGS);
    jbSerialize         archive(in, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

/*static*/ bool BFApp::SaveSettings ()
{
    wxFileOutputStream  out(BF_SETTINGS);
    jbSerialize         archive(out, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

const wxString& BFApp::GetCurrentProjectFilename ()
{
    return BFRootTaskApp::Instance().GetCurrentFilename();
}

wxString BFApp::GetCurrentProjectName ()
{
    return wxString(BFRootTaskApp::Instance().GetProjectName());
}

bool BFApp::OpenProject (const wxChar* filename)
{
    bool rc = BFRootTaskApp::Instance().ReadFromFile(filename);

    if (rc)
    {
        BFSettings::Instance().SetLastProject(filename);
        SaveSettings();
    }

    return rc;
}

bool BFApp::SaveProject (const wxChar* filename)
{
    bool rc = BFRootTaskApp::Instance().StoreToFile(filename);

    if (rc)
        BFSettings::Instance().SetLastProject(filename);

    return rc;
}

bool BFApp::SaveCurrentProject ()
{
    return BFRootTaskApp::Instance().StoreToFile
    (
        BFRootTaskApp::Instance().GetCurrentFilename()
    );
}

bool BFApp::CloseCurrentProject (bool bCheckForModifications /*= true*/)
{
    if (bCheckForModifications)
        if (BFRootTaskApp::Instance().IsProjectModified())
            return false;

    BFRootTaskApp::Instance().Close();

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

void BFApp::Sound_BackupFinished ()
{
    wxSound(_T("sound\\finish.wav")).Play();
}


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

