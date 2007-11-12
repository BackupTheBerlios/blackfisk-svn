/**
 * Name:        BFMainFrame.cpp
 * Purpose:     BFMainFrame class implementation
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

#include "BFMainFrame.h"

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>
#include <wx/tooltip.h>
#include <wx/aboutdlg.h>

#include "blackfisk.h"
#include "BFApp.h"
#include "Progress.h"
#include "BFDirCtrl.h"
#include "BFBackupTree.h"
#include "BFIconTable.h"
#include "BFProjectSettingsDlg.h"
#include "BFSettingsDlg.h"
#include "BFSettings.h"
#include "BFLogViewDlg.h"
#include "BFAboutDlg.h"
#include "BFSettingsDlg2.h"
#include "BFRootTask.h"
#include "BFBackupProgressDlg.h"
#include "BFHyperlinkCtrl.h"
#include "BFThread_ProjectRunner.h"
#include "ctrlids.h"


BEGIN_EVENT_TABLE(BFMainFrame, wxFrame)
    EVT_CLOSE   (BFMainFrame::OnClose)
    EVT_MENU    (ID_OpenProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProjectAs,      BFMainFrame::OnProject)
    EVT_MENU    (ID_CloseProject,       BFMainFrame::OnProject)
    EVT_MENU    (ID_NewProject,         BFMainFrame::OnProject)
    EVT_MENU    (ID_ProjectSettings,    BFMainFrame::OnProject)
    EVT_MENU    (ID_Quit,               BFMainFrame::OnQuit)
    EVT_MENU    (ID_About,              BFMainFrame::OnAbout)
    EVT_MENU    (ID_DisplayLog,         BFMainFrame::OnDisplayLog)
    EVT_MENU    (ID_ShowLicense,        BFMainFrame::OnShowLicense)
    EVT_MENU    (ID_ShowHistory,        BFMainFrame::OnShowHistory)
    EVT_MENU    (ID_OpenWebSite,        BFMainFrame::OnOpenWebSite)
    EVT_MENU    (ID_SubmitBug,          BFMainFrame::OnSubmitBug)
    EVT_MENU    (ID_Backup,             BFMainFrame::OnBackup)
    EVT_MENU    (ID_Settings,           BFMainFrame::OnSettings)
#ifdef _DEBUG
    EVT_MENU    (ID_Test,               BFMainFrame::OnTest)
#endif

    EVT_COMMAND (BF_ID_MAINFRAME, BF_EVENT_THREAD_END, BFMainFrame::OnThreadEnd)
END_EVENT_TABLE()


/*private*/ BFMainFrame::BFMainFrame (BFApp& rApp)
            : wxFrame (NULL,
                       BF_ID_MAINFRAME,
                       wxEmptyString),
                       menuProject_(NULL)
{
    /* the BFMainFrame object set its own reference in BFApp itself
       because the reference (BFApp::spMainFrame_) is needed quite
       early while creation time of the BFMainFrame object */
    wxGetApp().SetMainFrame(this);

    // set the window icon
    SetIcon(BFIconTable::Instance()->GetIcon(BFIconTable::logo));

    // set as top window
    wxGetApp().SetTopWindow(this);

    // ** menu BLACKFISK **
    wxMenu* menuBlackfisk = new wxMenu;
    menuBlackfisk->Append( ID_Settings,     _("Gobal &Settings") );
    menuBlackfisk->Append( ID_DisplayLog,   _("show &log file") );
    menuBlackfisk->AppendSeparator();
    menuBlackfisk->Append( ID_Quit,         _("E&xit") );
#ifdef _DEBUG
    menuBlackfisk->AppendSeparator();
    menuBlackfisk->Append( ID_Test,         _("&Testen") );
#endif

    // ** menu PROJECT **
    menuProject_ = new wxMenu;
    menuProject_->Append( ID_ProjectSettings,    _("&Project Settings") );
    menuProject_->Append( ID_Backup,             _("&Run Backup...") );
    menuProject_->AppendSeparator();
    menuProject_->Append( ID_NewProject,         _("&New/Close Project") );
    menuProject_->Append( ID_OpenProject,        _("&Open Project") );
    menuProject_->Append( ID_SaveProject,        _("&Save Project") );
    menuProject_->Append( ID_SaveProjectAs,      _("Save Project &as ...") );

    // ** menu HELP **
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append( ID_OpenWebSite,   _("&Website") );
    menuHelp->Append( ID_SubmitBug,     _("Report a &Bug") );
    menuHelp->Append( ID_ShowLicense,   _("&License") );
    menuHelp->Append( ID_ShowHistory,   _("&History") );
    menuHelp->AppendSeparator();
    menuHelp->Append( ID_About,         _("&About") );

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuBlackfisk,     _("&blackfisk") );
    menuBar->Append( menuProject_,      _("&Project") );
    menuBar->Append( menuHelp,          _("&Help") );
    SetMenuBar( menuBar );

    // splitter window
    wxSplitterWindow* pSplitter = new wxSplitterWindow(this);
    pBackupCtrl_    = new BFBackupCtrl(pSplitter);
    pDirCtrl_       = new BFDirCtrl(pSplitter);
    pSplitter->SplitVertically(pBackupCtrl_, pDirCtrl_);

    // ** sizer **
    wxSizer* pSizer = new wxBoxSizer (wxVERTICAL);
    pSizer->Add( pSplitter, wxSizerFlags(3).Expand() );

    CreateStatusBar();
    SetStatusText( _("Welcome to wxWindows!") );

    SetSizer(pSizer);

    RefreshTitle();

    SetSize(wxSize(650, 600));
    Center();
    Show(TRUE);

#ifdef _DEBUG
    //Test();
#endif
}

void BFMainFrame::OnClose (wxCloseEvent& event)
{
    int iAnswer = wxYES;

    // check for a modified project
    if (wxGetApp().IsProjectModified())
    {
        // ask for save
        iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it?"));

        // cancel/abort
        if (iAnswer == wxCANCEL)
            event.Veto();

        // save
        if (iAnswer == wxYES)
        {
            wxString strProject;

            if (wxGetApp().GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    wxGetApp().SaveProject(strProject);
                else
                    iAnswer = wxCANCEL;
            }
            else
            {
                wxGetApp().SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxCANCEL)
        Destroy();
}

void BFMainFrame::RefreshTitle ()
{
    // application name and version
    wxTopLevelWindow::SetTitle( wxString::Format(_T("%s (%s) - %s"),
                                                 wxGetApp().GetCurrentProjectName().c_str(),
                                                 wxGetApp().GetCurrentProjectFilename().c_str(),
                                                 BFApp::GetFullApplicationName()) );
}


BFBackupCtrl* BFMainFrame::BackupCtrl ()
{
    return pBackupCtrl_;
}

BFBackupTree* BFMainFrame::BackupTree ()
{
    if (BackupCtrl())
        return BackupCtrl()->BackupTree();
    else
        return NULL;
}

/*virtual*/ BFMainFrame::~BFMainFrame ()
{
}

void BFMainFrame::OnDisplayLog (wxCommandEvent& event)
{
    wxArrayString arr;
    arr.Add(BF_LOGFILE_NAME);
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnShowLicense (wxCommandEvent& event)
{
    wxArrayString arr;
    arr.Add(_T("LICENSE"));
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnShowHistory (wxCommandEvent& event)
{
    wxArrayString arr;
    arr.Add(_T("HISTORY"));
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnOpenWebSite (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(_T("http://www.blackfisk.org"));
}

void BFMainFrame::OnSubmitBug (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(_T("http://developer.berlios.de/bugs/?group_id=8687"));
}

void BFMainFrame::OnProject (wxCommandEvent& event)
{
    wxString strProject;

    switch (event.GetId())
    {
        case ID_OpenProject:
            if (AskModification())
                wxGetApp().CloseCurrentProject(false);

            if (AskOpenProject(strProject))
                wxGetApp().OpenProject(strProject);
            break;

        case ID_SaveProject:
            if (wxGetApp().GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    wxGetApp().SaveProject(strProject);
            }
            else
            {
                wxGetApp().SaveCurrentProject();
            }
            break;

        case ID_SaveProjectAs:
            if (AskSaveProject(strProject))
                wxGetApp().SaveProject(strProject);
            break;

        case ID_CloseProject:
        case ID_NewProject:
            if (AskModification())
                wxGetApp().CloseCurrentProject(false);
            break;

        case ID_ProjectSettings:
            OpenProjectSettings();
            break;
    }

    RefreshTitle();
}

void BFMainFrame::OnSettings (wxCommandEvent& event)
{
    new BFSettingsDlg(this);
}

void BFMainFrame::OnThreadEnd (wxCommandEvent& event)
{
    DeleteRememberedThreads();
    Iconize(false);
    new BFLogViewDlg(this, BFRootTask::Instance().GetLastLogFiles());
}

void BFMainFrame::DeleteRememberedThreads ()
{
    BFThreadVector::iterator it;

    for (it = vecThreads_.begin();
         it != vecThreads_.end();
         it++)
    {
        (*it)->Wait();
        delete (*it);
    }

    vecThreads_.clear();
}

void BFMainFrame::RememberThread (wxThread* pThread)
{
    if (pThread == NULL)
        return;

    vecThreads_.push_back(pThread);
}

bool BFMainFrame::AskModification ()
{
    // check for a modified project
    if (wxGetApp().IsProjectModified())
    {
        // ask for save
        int iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it?"));

        // cancel/abort
        if (iAnswer == wxCANCEL)
            return false;

        // save
        if (iAnswer == wxYES)
            if (wxGetApp().SaveCurrentProject() == false)
                return false;
    }

    // wxNO
    return true;
}

bool BFMainFrame::AskOpenProject (wxString& strProject)
{
    if ( !(AskModification()) )
        return false;

    wxFileDialog dlg
    (
        this,
        _("Open a project"),
        BFRootTask::Instance().GetCurrentFilename().BeforeLast(wxFILE_SEP_PATH),
        wxEmptyString,
        BF_PROJECT_EXTENSION_STRING,
        wxFD_OPEN
    );

    dlg.CentreOnParent();

    if (dlg.ShowModal() == wxID_OK)
    {
        strProject = dlg.GetPath();
        return true;
    }

    return false;
}

bool BFMainFrame::AskSaveProject (wxString& strProject)
{
    wxFileDialog dlg
    (
        this,
        _("Save the project"),
        BFRootTask::Instance().GetCurrentFilename().BeforeLast(wxFILE_SEP_PATH),
        BFRootTask::Instance().GetCurrentFilename().AfterLast(wxFILE_SEP_PATH),
        BF_PROJECT_EXTENSION_STRING,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    dlg.CentreOnParent();

    if (dlg.ShowModal() == wxID_OK)
    {
        strProject = dlg.GetPath();
        return true;
    }

    return false;
}

void BFMainFrame::OnQuit (wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void BFMainFrame::OnAbout (wxCommandEvent& WXUNUSED(event))
{
    new BFAboutDlg();
}

#ifdef _DEBUG
void BFMainFrame::OnTest (wxCommandEvent& WXUNUSED(event))
{
    Test();
}

void BFMainFrame::Test ()
{
}
#endif

void BFMainFrame::OnBackup (wxCommandEvent& WXUNUSED(event))
{
    int iAnswer = wxYES;

    // check for a modified project
    if (wxGetApp().IsProjectModified())
    {
        // ask for save
        iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it before running the backup?"));

        // save
        if (iAnswer == wxYES)
        {
            wxString strProject;

            if (wxGetApp().GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    wxGetApp().SaveProject(strProject);
                else
                    iAnswer = wxCANCEL;
            }
            else
            {
                wxGetApp().SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxCANCEL)
        new BFBackupProgressDlg(this);
}

bool BFMainFrame::QuestionYesNo (const wxChar* strQuestion)
{
    int rc = wxMessageBox(strQuestion, _("Question"), wxYES_NO | wxICON_QUESTION);

    if (rc == wxYES)
        return true;

    return false;
}

int BFMainFrame::QuestionYesNoCancel (const wxChar* strQuestion)
{
    return ::wxMessageBox(strQuestion, _("Question"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
}

void BFMainFrame::OpenProjectSettings ()
{
    new BFProjectSettingsDlg(this);
}
