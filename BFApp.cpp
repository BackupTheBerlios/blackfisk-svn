/**
 * Name:        BFApp.cpp
 * Purpose:     BFApp class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-04-05
 * Copyright:   (c) 2006 Christian Buhtz <exsudat@gmx.de>
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
#include <wx/sound.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFTaskProgressDlg.h"
#include "BFTaskDlg.h"
#include "BFRootTask.h"
#include "Progress.h"
#include "BFSettings.h"
#include "BFwxLog.h"
#include "BFBackupQuestionDlg.h"

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

/*static*/ wxString BFApp::SumStrings (const wxArrayString& rStrings, const wxChar cSeperator /*= _T('\n')*/)
{
    wxString strResult;

    strResult.Clear();

    for (int i = 0; i < rStrings.GetCount(); ++i)
        strResult = strResult + rStrings[i] + cSeperator;

    return strResult;
}

/*static*/ wxArrayString& BFApp::PrependString (wxArrayString& rStrings,
                                                const wxChar* strToPrepend)
{
    if (strToPrepend)
    {
        for (int i = 0; i < rStrings.GetCount(); ++i)
            rStrings[i].Prepend(strToPrepend);
    }

    return rStrings;
}

/*static*/ wxString BFApp::ExtractCommunity (const wxArrayString& rStrings)
{
    wxString str;

    if (rStrings.GetCount() == 0)
        return str;

    int iCur = 0;
    wxChar cCur = _T('\0');

    while (true)
    {
        for (int i = 0; i < rStrings.GetCount(); ++i)
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
    int i = 0;
}

bool BFApp::OnInit()
{
    ReadSettings();

    BFSystem::Log(wxString::Format(_("%s %s by %s started"), BF_PRGNAME, BF_VERSION, BF_AUTHOR).c_str());

    // available languages
    // TODO just look at the locals directory to know what is available
    static const wxLanguage langIds[] =
    {
        wxLANGUAGE_DEFAULT,
        wxLANGUAGE_FRENCH,
        wxLANGUAGE_GERMAN,
        wxLANGUAGE_ENGLISH
    };

    locale_.AddCatalogLookupPathPrefix(_T(".\\locale"));
    locale_.Init( langIds[2] );
    locale_.AddCatalog(_T("ob"));

    /* init the main frame
       'BFApp::spMainFrame_' is set by the ctor of BFMainFrame itself */
    new BFMainFrame(*this);

    // open the last project
    if (BFSettings::Instance().GetOpenLastProject())
        if (BFSettings::Instance().GetLastProjects().GetCount() > 0)
        {
            OpenProject(BFSettings::Instance().GetLastProjects().Last());
            MainFrame()->RefreshTitle();
        }

    //
    BFSystem::Log(wxString::Format(_("application verbose level: %s"), BFSystem::GetTypeString(BFSettings::Instance().GetVerboseLevelLog()).c_str()));

    return TRUE;
}

/*virtual*/ int BFApp::OnExit()
{
    SaveSettings();
    BFSystem::Log(wxString::Format(_("%s closed\n"), BF_PRGNAME).c_str());
}

/*static*/ bool BFApp::ReadSettings ()
{
    if ( !(wxFileName::FileExists(BF_SETTINGS)) )
        SaveSettings();

    wxFileInputStream   in(BF_SETTINGS);
    jbSerialize           archive(in, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

/*static*/ bool BFApp::SaveSettings ()
{
    wxFileOutputStream  out(BF_SETTINGS);
    jbSerialize           archive(out, BF_SETTINGS_CURRENT_VERSION);

    return BFSettings::Instance().Serialize(archive);
}

const wxString& BFApp::GetCurrentProjectFilename ()
{
    return BFRootTask::Instance().GetCurrentFilename();
}

wxString BFApp::GetCurrentProjectName ()
{
    return wxString(BFRootTask::Instance().GetName());
}

bool BFApp::OpenProject (const wxChar* filename)
{
    BFSettings::Instance().SetLastProject(filename);
    return BFRootTask::Instance().ReadFromFile(filename);
}

bool BFApp::SaveProject (const wxChar* filename)
{
    BFSettings::Instance().SetLastProject(filename);
    return BFRootTask::Instance().StoreToFile(filename);
}

bool BFApp::SaveCurrentProject ()
{
    return BFRootTask::Instance().StoreToFile
    (
        BFRootTask::Instance().GetCurrentFilename()
    );
}

bool BFApp::CloseCurrentProject (bool bCheckForModifications /*= true*/)
{
    if (bCheckForModifications)
        if (IsProjectModified())
            return false;

    BFRootTask::Instance().Close();
}

bool BFApp::IsProjectModified ()
{
    return BFRootTask::Instance().IsModified();
}

void BFApp::Test ()
{
    wxArrayString   arr;
    wxString        str;
    arr.Add(_T("ABCDEFGHIJKLMN"));
    arr.Add(_T("ABCDEfghi239"));
    arr.Add(_T("ABCd333"));
    arr.Add(_T("ABC"));

    str = ExtractCommunity(arr);
}


void BFApp::Backup()
{
    BFwxLog log;

    /* deactivate the default wxLog target
       and set a new one that handle messages
       with BFSystem */
    wxLog::SetActiveTarget(&log);

    // ** RUN **
    if (BFRootTask::Instance().Run(this->MainFrame()))
    {
        wxSound(_T("sound\\finish.wav")).Play();
    }
    else
    {
        BFSystem::Fatal(_T("there was an ERROR while Backup"), _T("BFApp::Backup()"));
    }

    // reset the default wxLog target
    wxLog::SetActiveTarget(NULL);
}
