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
#include "BFRootTaskApp.h"
#include "BFBackupProgressDlg.h"
#include "BFHyperlinkCtrl.h"
#include "BFThread_ProjectRunner.h"
#include "ids.h"
#include "BFIsWorkingCtrl.h"
#include "BFHelpCtrl.h"
#include "BFMessageDlg.h"
#include "BFBitmapButton.h"
#include "BFProjectPlannerDlg.h"
#include "BFEnvironment.h"


BEGIN_EVENT_TABLE(BFMainFrame, wxFrame)
    EVT_CLOSE   (BFMainFrame::OnClose)
    EVT_MENU    (BF_ID_MAINFRAME_OPENPRJ,        	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_SAVEPRJ,        	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_SAVEPRJAS,      	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_CLOSEPRJ,       	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_NEWPRJ,         	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_PRJSETTINGS,    	BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_PRJPLANNER,        BFMainFrame::OnProject)
    EVT_MENU    (BF_ID_MAINFRAME_QUIT,              BFMainFrame::OnQuit)
    EVT_MENU    (BF_ID_MAINFRAME_ABOUT,             BFMainFrame::OnAbout)
    EVT_MENU    (BF_ID_MAINFRAME_SHOWLOG,           BFMainFrame::OnDisplayLog)
    EVT_MENU    (BF_ID_MAINFRAME_SHOWLICENSE,       BFMainFrame::OnShowLicense)
    EVT_MENU    (BF_ID_MAINFRAME_SHOWHISTORY,       BFMainFrame::OnShowHistory)
    EVT_MENU    (BF_ID_MAINFRAME_OPENWEBSITE,       BFMainFrame::OnOpenWebSite)
    EVT_BUTTON  (BF_ID_MAINFRAME_OPENWEBSITE,       BFMainFrame::OnOpenWebSite)
    EVT_MENU    (BF_ID_MAINFRAME_SUBMITBUG,         BFMainFrame::OnSubmitBug)
    EVT_BUTTON  (BF_ID_MAINFRAME_SUBMITBUG,         BFMainFrame::OnSubmitBug)
    EVT_MENU    (BF_ID_MAINFRAME_FEAUTERREQUEST,    BFMainFrame::OnFeauterRequest)
    EVT_BUTTON  (BF_ID_MAINFRAME_FEAUTERREQUEST,    BFMainFrame::OnFeauterRequest)
    EVT_MENU    (BF_ID_MAINFRAME_MAIL,              BFMainFrame::OnMail)
    EVT_BUTTON  (BF_ID_MAINFRAME_MAIL,              BFMainFrame::OnMail)
    EVT_MENU    (BF_ID_MAINFRAME_BACKUP,            BFMainFrame::OnBackup)
    EVT_MENU    (BF_ID_MAINFRAME_SETTINGS,          BFMainFrame::OnSettings)
#ifdef _DEBUG
    EVT_MENU    (BF_ID_MAINFRAME_TEST,              BFMainFrame::OnTest)
    EVT_BUTTON  (BF_ID_MAINFRAME_TEST,              BFMainFrame::OnTest)
#endif

    EVT_COMMAND (BF_ID_MAINFRAME, BF_EVENT_THREAD_END, BFMainFrame::OnThreadEnd)
END_EVENT_TABLE()


/*static*/ BFMainFrame* BFMainFrame::Instance()
{
    return wxGetApp().MainFrame();
}

/*private*/ BFMainFrame::BFMainFrame ()
            : wxFrame (NULL,
                       BF_ID_MAINFRAME,
                       wxEmptyString),
                       menuProject_(NULL)
{
    // install help provider
    BFHelpCtrl::InitHelpProvider();

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
    menuBlackfisk->Append( BF_ID_MAINFRAME_SETTINGS,    _("Gobal &Settings") );
    menuBlackfisk->Append( BF_ID_MAINFRAME_SHOWLOG,     _("show &log file") );
    menuBlackfisk->AppendSeparator();
    menuBlackfisk->Append( BF_ID_MAINFRAME_QUIT,        _("E&xit") );
#ifdef _DEBUG
    menuBlackfisk->AppendSeparator();
    menuBlackfisk->Append( BF_ID_MAINFRAME_TEST,        "&Test" );
#endif

    // ** menu PROJECT **
    menuProject_ = new wxMenu;
    menuProject_->Append( BF_ID_MAINFRAME_BACKUP,       _("&Run Backup...") );
    menuProject_->AppendSeparator();
    menuProject_->Append( BF_ID_MAINFRAME_PRJSETTINGS,  _("Project Se&ttings") );
    menuProject_->Append( BF_ID_MAINFRAME_PRJPLANNER,   _("Project P&lanner") );
    if ( BFSettings::Instance().GetScheduler() == 0 )
        menuProject_->Enable(BF_ID_MAINFRAME_PRJPLANNER, false);
    menuProject_->AppendSeparator();
    menuProject_->Append( BF_ID_MAINFRAME_NEWPRJ,       _("&New Project") );
    menuProject_->Append( BF_ID_MAINFRAME_OPENPRJ,      _("&Open Project") );
    menuProject_->Append( BF_ID_MAINFRAME_SAVEPRJ,      _("&Save Project") );
    menuProject_->Append( BF_ID_MAINFRAME_SAVEPRJAS,    _("Save Project &as ...") );

    // ** menu HELP **
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append( BF_ID_MAINFRAME_OPENWEBSITE,   _("&Website") );
    menuHelp->Append( BF_ID_MAINFRAME_SUBMITBUG,     _("Report a &Bug") );
    menuHelp->Append( BF_ID_MAINFRAME_FEAUTERREQUEST,_("Submit a &Feauter Request") );
    menuHelp->Append( BF_ID_MAINFRAME_MAIL,          _("E-Mail Contact") );
    menuHelp->Append( BF_ID_MAINFRAME_SHOWLICENSE,   _("&License") );
    menuHelp->Append( BF_ID_MAINFRAME_SHOWHISTORY,   _("&History") );
    menuHelp->AppendSeparator();
    menuHelp->Append( BF_ID_MAINFRAME_ABOUT,         _("&About") );

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuBlackfisk,     "&blackfisk" );
    menuBar->Append( menuProject_,      _("&Project") );
    menuBar->Append( menuHelp,          _("&Help") );
    SetMenuBar( menuBar );

    // splitter window
    pSplitterCtrl_  = new wxSplitterWindow(this);
    pBackupCtrl_    = new BFBackupCtrl(pSplitterCtrl_);
    pDirCtrl_       = new BFDirCtrl(pSplitterCtrl_);

    // switch child ctrls?
    if (BFSettings::Instance().GetSwitchMainCtrls())
        pSplitterCtrl_->SplitVertically(pDirCtrl_, pBackupCtrl_);
    else
        pSplitterCtrl_->SplitVertically(pBackupCtrl_, pDirCtrl_);

    // sash position
    if (BFSettings::Instance().GetSashPositionInMainWindow() != -1)
        pSplitterCtrl_->SetSashPosition(BFSettings::Instance().GetSashPositionInMainWindow());

    pSplitterCtrl_->SetSashGravity(0.5);

    // buttons
    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* pButtonBug        = new wxButton(this, BF_ID_MAINFRAME_SUBMITBUG,      _("Bug Report"));
    wxButton* pButtonFeauter    = new wxButton(this, BF_ID_MAINFRAME_FEAUTERREQUEST, _("Feauter Request"));
    wxButton* pButtonWebsite    = new wxButton(this, BF_ID_MAINFRAME_OPENWEBSITE,    _("Website"));
    wxButton* pButtonMail       = new wxButton(this, BF_ID_MAINFRAME_MAIL,           _("E-Mail Contact"));

#ifdef _DEBUG
    wxButton* pButtonTest       = new wxButton(this, BF_ID_MAINFRAME_TEST, _(" T E S T "));
    pButtonSizer->Add( pButtonTest, wxSizerFlags(0).Border(wxALL, 10) );
    pButtonSizer->AddStretchSpacer(1);
#endif

    pButtonSizer->Add( pButtonBug,      wxSizerFlags(0).Border(wxALL, 10));
    pButtonSizer->AddStretchSpacer(1);
    pButtonSizer->Add( pButtonFeauter,  wxSizerFlags(0).Border(wxALL, 10));
    pButtonSizer->AddStretchSpacer(1);
    pButtonSizer->Add( pButtonWebsite,  wxSizerFlags(0).Border(wxALL, 10));
    pButtonSizer->AddStretchSpacer(1);
    pButtonSizer->Add( pButtonMail,     wxSizerFlags(0).Border(wxALL, 10));

    // ** sizer **
    wxSizer* pSizer = new wxBoxSizer (wxVERTICAL);
    pSizer->Add( pSplitterCtrl_,    wxSizerFlags(6).Expand() );
    pSizer->Add( pButtonSizer,      wxSizerFlags(0).Center() );
    SetSizer(pSizer);

    // background color
    SetBackgroundColour(menuBar->GetBackgroundColour());

    // window position
    Move(BFSettings::Instance().GetMainWindowPosition());

    // window size or maximized state
    SetSize(BFSettings::Instance().GetMainWindowSize());

    // maximized state
    if (BFSettings::Instance().GetMainWindowMaximized())
        Maximize();

    // sash position
    pSplitterCtrl_->SetSashPosition(BFSettings::Instance().GetSashPositionInMainWindow());

    //
    Show(TRUE);

    // set the title bar
    RefreshTitle();

    // check for new version if needed
    if (BFSettings::Instance().CheckForNewVersion())
    {
        if(wxGetApp().IsNewVersionAvailable())
        {
            BFSystem::Info
            (
                wxString::Format
                (
                    "A new version of blackfisk is availabel!\n" \
                    "New version is %s\n" \
                    "This version is %s\n" \
                    "Please visit the website (see in help-menu).",
                    wxGetApp().GetNewestVersion(),
                    wxGetApp().GetVersion()
                )
            );
        }
    }

#ifdef _DEBUG
    //Test();
#endif
}

void BFMainFrame::OnClose (wxCloseEvent& event)
{
    int iAnswer = wxID_YES;

    if (IsMaximized())
    {
        // remember window maximized state
        BFSettings::Instance().SetMainWindowMaximized(true);
    }
    else
    {
        // remember window position
        BFSettings::Instance().SetMainWindowPosition(GetPosition());

        // remember window size
        BFSettings::Instance().SetMainWindowSize(GetSize());

        // remember window maximized state
        BFSettings::Instance().SetMainWindowMaximized(false);
    }

    // remember sash position
    BFSettings::Instance().SetSashPositionInMainWindow(pSplitterCtrl_->GetSashPosition());

    // check for a modified project
    if (BFRootTaskApp::Instance().IsProjectModified())
    {
        // ask for save
        iAnswer = QuestionYesNoCancel(_("The current project is modified!\nSave it?"));

        // cancel/abort
        if (iAnswer == wxID_CANCEL)
            event.Veto();

        // save
        if (iAnswer == wxID_YES)
        {
            wxString strProject;

            if (wxGetApp().GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    wxGetApp().SaveProject(strProject);
                else
                    iAnswer = wxID_CANCEL;
            }
            else
            {
                wxGetApp().SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxID_CANCEL)
        Destroy();
}

void BFMainFrame::RefreshTitle ()
{
    // application name and version
    wxTopLevelWindow::SetTitle( wxString::Format("%s (%s) - %s",
                                                 wxGetApp().GetCurrentProjectName().c_str(),
                                                 wxGetApp().GetCurrentProjectFilename().c_str(),
                                                 BFApp::GetFullApplicationName()) );
}

void BFMainFrame::EnableMenuProjectPlanner (bool bEnable)
{
    menuProject_->Enable(BF_ID_MAINFRAME_PRJPLANNER, bEnable);
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
    arr.Add(BFEnvironment::GetLogFileName());
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnShowLicense (wxCommandEvent& event)
{
    wxArrayString arr;
    arr.Add("LICENSE");
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnShowHistory (wxCommandEvent& event)
{
    wxArrayString arr;
    arr.Add("HISTORY");
    new BFLogViewDlg(this, arr);
}

void BFMainFrame::OnOpenWebSite (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(BF_URL_WEBSITE);
}

void BFMainFrame::OnSubmitBug (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(BF_URL_BUGREPORT);
}

void BFMainFrame::OnFeauterRequest (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(BF_URL_FEAUTERREQUEST);
}

void BFMainFrame::OnMail (wxCommandEvent& event)
{
    new BFThread_LaunchBrowser(BF_URL_MAIL);
}

void BFMainFrame::OnProject (wxCommandEvent& event)
{
    wxString strProject;

    switch (event.GetId())
    {
        case BF_ID_MAINFRAME_OPENPRJ:
            if (AskModification())
                wxGetApp().CloseCurrentProject(false);

            if (AskOpenProject(strProject))
                wxGetApp().OpenProject(strProject);
            break;

        case BF_ID_MAINFRAME_SAVEPRJ:
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

        case BF_ID_MAINFRAME_SAVEPRJAS:
            if (AskSaveProject(strProject))
                wxGetApp().SaveProject(strProject);
            break;

        case BF_ID_MAINFRAME_CLOSEPRJ:
        case BF_ID_MAINFRAME_NEWPRJ:
            if (AskModification())
                wxGetApp().CloseCurrentProject(false);
            break;

        case BF_ID_MAINFRAME_PRJSETTINGS:
            OpenProjectSettings();
            break;

        case BF_ID_MAINFRAME_PRJPLANNER:
            OpenProjectPlanner();
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
    //
    DeleteRememberedThreads();

    // un-minimize the main-window
    Iconize(false);

    /* The placeholder-strings (date, time, ...) are created new on
       start of a backup. So we need to refresh the old displayed
       strings in the backuptree. */
    BackupTree()->RefreshPlaceholders();

    // view backup-log
    new BFLogViewDlg(this, BFRootTaskApp::Instance().GetLastLogFiles());
}

void BFMainFrame::DeleteRememberedThreads ()
{
    BFThreadVector::iterator it;

    for (it = vecThreads_.begin();
         it != vecThreads_.end();
         ++it)
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
    if (BFRootTaskApp::Instance().IsProjectModified())
    {
        // ask for save
        int iAnswer = QuestionYesNoCancel(_("The current project is modified!\nSave it?"));

        // cancel/abort
        if (iAnswer == wxID_CANCEL)
            return false;

        // save
        if (iAnswer == wxID_YES)
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
        BFRootTaskApp::Instance().GetCurrentFilename().BeforeLast(wxFILE_SEP_PATH),
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
    wxString strPath = BFRootTaskApp::Instance().GetCurrentFilename().BeforeLast(wxFILE_SEP_PATH);
    wxString strFile = BFRootTaskApp::Instance().GetCurrentFilename().AfterLast(wxFILE_SEP_PATH);

    if (strFile.IsEmpty())
        strFile = BFRootTaskApp::Instance().GetProjectName();

    wxFileDialog dlg
    (
        this,
        _("Save the project"),
        strPath,
        strFile,
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
#include "BFSound.h"
void BFMainFrame::OnTest (wxCommandEvent& WXUNUSED(event))
{
    //wxBell();
    //wxSystem(wxString::Format("echo %c", 7));

    /*Beep(3500, 1000);
    wxGetApp().Sound_BackupFinished();*/

    BFMessageDlg dlg1(BF_MSGDLG_INFO, "test");
    BFMessageDlg dlg2(BF_MSGDLG_WARNING, "test");
    BFMessageDlg dlg3(BF_MSGDLG_ERROR, "test");
    BFMessageDlg dlg4(BF_MSGDLG_FATAL, "test");
    BFSound::Finish();
}

void BFMainFrame::Test ()
{
}
#endif

void BFMainFrame::OnBackup (wxCommandEvent& WXUNUSED(event))
{
    int iAnswer = wxID_YES;

    // check for a modified project
    if (BFRootTaskApp::Instance().IsProjectModified())
    {
        // ask for save
        iAnswer = QuestionYesNoCancel(_("The current project is modified!\nSave it before running the backup?"));

        // save
        if (iAnswer == wxID_YES)
        {
            wxString strProject;

            if (wxGetApp().GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    wxGetApp().SaveProject(strProject);
                else
                    iAnswer = wxID_CANCEL;
            }
            else
            {
                wxGetApp().SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxID_CANCEL)
    {
        // start the backup
        if ( BFRootTaskApp::Instance().PreBackupCheck() )
            new BFBackupProgressDlg(this);
        else
            BFSystem::Log(_("PreBackup failed. Aborting ..."));
    }
}

/*static*/ bool BFMainFrame::QuestionYesNo (const wxString& strQuestion)
{
    BFMessageDlg dlg(BF_MSGDLG_QUESTION_YESNO, strQuestion, _("Question"));

    if (dlg.ShowModal() == BF_MSGDLG_ID_YES)
        return true;

    return false;
}

/*static*/ int BFMainFrame::QuestionYesNoCancel (const wxString& strQuestion)
{
    BFMessageDlg dlg(BF_MSGDLG_QUESTION_YESNOCANCEL, strQuestion, _("Question"));

    return dlg.ShowModal();
}

void BFMainFrame::OpenProjectSettings ()
{
    new BFProjectSettingsDlg(this);
}

void BFMainFrame::OpenProjectPlanner ()
{
    new BFProjectPlannerDlg(this);
}


wxString& BFMainFrame::Wrap (wxString& str, int iWidthInPixel)
{
    wxWindowDC dc(this);
    wxSize sizeMsg = dc.GetTextExtent(str);

    if (sizeMsg.GetWidth() <= iWidthInPixel)
        return str;

    int iWidthCurrent = 0;
    wxString strNew;
    wxString strFin;
    while (str.Length() > 0)
    {
        // get n characters from the new string
        strNew = strNew + str.Left(1);

        // remove n characters from the old string
        str = str.Right(str.Length()-1);

        // new line character inside?
        if (strNew.EndsWith("\n"))
        {
            // rememeber line
            strFin = strFin + strNew;

            // new line
            strNew.Clear();

            // go on
            continue;
        }

        // calculate width of new string
        iWidthCurrent = dc.GetTextExtent(strNew).GetWidth();

        // check width
        if (iWidthCurrent > iWidthInPixel)
        {
            // get n-1 characters back from the new string to the old one
            str = strNew.Right(1) + str;

            // remove n-1 characters from the new string
            strNew = strNew.Left(strNew.Length()-1);

            // remember line
            strFin = strFin + strNew + '\n';

            // new line
            strNew.Clear();
        }
    }

    // remember last line
    strFin = strFin + strNew;

    str = strFin;

    return str;
}
