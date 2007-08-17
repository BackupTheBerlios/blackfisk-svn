/**
 * Name:        BFMainFrame.cpp
 * Purpose:     BFMainFrame class implementation
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

#include "BFMainFrame.h"

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>
#include <wx/tooltip.h>

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

#ifdef _DEBUG
#include "BFRootTask.h"
#endif

BEGIN_EVENT_TABLE(BFMainFrame, wxFrame)
    EVT_CLOSE   (BFMainFrame::OnClose)
    EVT_MENU    (ID_OpenProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProjectAs,      BFMainFrame::OnProject)
    EVT_MENU    (ID_CloseProject,       BFMainFrame::OnProject)
    EVT_MENU    (ID_NewProject,         BFMainFrame::OnProject)
    EVT_MENU    (ID_ProjectSettings,    BFMainFrame::OnProject)
    EVT_MENU    (ID_LastProject1,       BFMainFrame::OnLastProject)
    EVT_MENU    (ID_LastProject2,       BFMainFrame::OnLastProject)
    EVT_MENU    (ID_LastProject3,       BFMainFrame::OnLastProject)
    EVT_MENU    (ID_LastProject4,       BFMainFrame::OnLastProject)
    EVT_MENU    (ID_Quit,               BFMainFrame::OnQuit)
    EVT_MENU    (ID_About,              BFMainFrame::OnAbout)
    EVT_MENU    (ID_DisplayLog,         BFMainFrame::OnDisplayLog)
#ifdef _DEBUG
    EVT_MENU    (ID_Test,               BFMainFrame::OnTest)
#endif
    EVT_MENU    (ID_Backup,             BFMainFrame::OnBackup)
    EVT_MENU    (ID_Settings,           BFMainFrame::OnSettings)
END_EVENT_TABLE()


/*private static*/ BFApp* BFMainFrame::spApp_ = NULL;

/*private*/ BFMainFrame::BFMainFrame (BFApp& rApp)
            : wxFrame (NULL,
                       -1,
                       wxEmptyString),
                       // XXX qmsgDlg_(this),
                       menuProject_(NULL)
{
    // set application variable
    spApp_ = &rApp;

    /* the BFMainFrame object set its own reference in BFApp itself
       because the reference (BFApp::spMainFrame_) is needed quite
       early while creation time of the BFMainFrame object */
    spApp_->SetMainFrame(this);

    // set the window icon
    SetIcon(BFIconTable::Instance()->GetIcon(BFIconTable::logo));

    // set as top window
    spApp_->SetTopWindow(this);

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
    menuProject_->Append( ID_Backup,             _("&Run Backup...") );
    menuProject_->AppendSeparator();
    menuProject_->Append( ID_NewProject,         _("&New/Close Project") );
    menuProject_->Append( ID_OpenProject,        _("&Open Project") );
    menuProject_->Append( ID_SaveProject,        _("&Save Project") );
    menuProject_->Append( ID_SaveProjectAs,      _("Save Project &as ...") );
    menuProject_->AppendSeparator();
    menuProject_->Append( ID_ProjectSettings,    _("&Project Settings") );

    // ** menu HELP **
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append( ID_About,         _("&About...") );

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuBlackfisk,     _("&blackfisk") );
    menuBar->Append( menuProject_,      _("&Project") );
    menuBar->Append( menuHelp,          _("&Help") );
    SetMenuBar( menuBar );

    CreateLastProjectMenu ();

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

    //wxToolTip::SetDelay(1000);

    Show(TRUE);
    SetSize(wxSize(650, 600));
    Center();

#ifdef _DEBUG
    //Test();
#endif
}

void BFMainFrame::OnClose (wxCloseEvent& event)
{
    int iAnswer = wxYES;

    // check for a modified project
    if (spApp_->IsProjectModified())
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

            if (spApp_->GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    spApp_->SaveProject(strProject);
                else
                    iAnswer = wxCANCEL;
            }
            else
            {
                spApp_->SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxCANCEL)
        Destroy();
}

void BFMainFrame::RefreshTitle ()
{
    // application name and version
    wxTopLevelWindow::SetTitle( wxString::Format(_T("%s (%s) - %s %s [with %s]"),
                                                 App()->GetCurrentProjectName().c_str(),
                                                 App()->GetCurrentProjectFilename().c_str(),
                                                 BF_PRGNAME,
                                                 BF_VERSION,
                                                 jbSerialize::GetLibVersionString().c_str()) );
}

void BFMainFrame::CreateLastProjectMenu ()
{
    if (menuProject_ == NULL)
        return;

    if (menuProject_->FindItem(ID_LastProject1) != NULL)
        menuProject_->Delete(ID_LastProject1);
    if (menuProject_->FindItem(ID_LastProject2) != NULL)
        menuProject_->Delete(ID_LastProject2);
    if (menuProject_->FindItem(ID_LastProject3) != NULL)
        menuProject_->Delete(ID_LastProject3);
    if (menuProject_->FindItem(ID_LastProject4) != NULL)
        menuProject_->Delete(ID_LastProject4);

    int iCount = BFSettings::Instance().GetLastProjects().GetCount();
    for (int i = iCount-1; i > -1 ; --i)
        menuProject_->Append( ID_LastProject4 - i, BFSettings::Instance().GetLastProjects()[i] );
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

void BFMainFrame::OnLastProject (wxCommandEvent& event)
{
    if (AskModification())
        spApp_->CloseCurrentProject(false);
        {
            spApp_->OpenProject(BFSettings::Instance().GetLastProjects()[ID_LastProject4 - event.GetId()]);
            CreateLastProjectMenu();
            RefreshTitle();
        }
}

void BFMainFrame::OnProject (wxCommandEvent& event)
{
    wxString strProject;

    switch (event.GetId())
    {
        case ID_OpenProject:
            if (AskModification())
                spApp_->CloseCurrentProject(false);
                    if (AskOpenProject(strProject))
                    {
                        spApp_->OpenProject(strProject);
                        CreateLastProjectMenu();
                    }
            break;

        case ID_SaveProject:
            if (spApp_->GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    spApp_->SaveProject(strProject);
            }
            else
            {
                spApp_->SaveCurrentProject();
            }
            break;

        case ID_SaveProjectAs:
            if (AskSaveProject(strProject))
                spApp_->SaveProject(strProject);
            break;

        case ID_CloseProject:
        case ID_NewProject:
            if (AskModification())
                spApp_->CloseCurrentProject(false);
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

bool BFMainFrame::AskModification ()
{
    // check for a modified project
    if (spApp_->IsProjectModified())
    {
        // ask for save
        int iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it?"));

        // cancel/abort
        if (iAnswer == wxCANCEL)
            return false;

        // save
        if (iAnswer == wxYES)
            if (spApp_->SaveCurrentProject() == false)
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
        _("message"),
        wxEmptyString,
        wxEmptyString,
        _("BlackfiskBackup files (*.ob)|*.ob"),
        wxFD_OPEN
    );

    dlg.CentreOnParent();

    if (dlg.ShowModal() == wxOK)
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
        _("save message"),
        wxEmptyString,
        wxEmptyString,
        _("OctopusBackup files (*.ob)|*.ob"),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    dlg.CentreOnParent();

    if (dlg.ShowModal() == wxOK)
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
    wxMessageBox(wxVERSION_STRING,
        _("About Hello World"), wxOK | wxICON_INFORMATION, this);
/*
    <program>  Copyright (C) <year>  <name of author>
    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.
*/
}

#ifdef _DEBUG
void BFMainFrame::OnTest (wxCommandEvent& WXUNUSED(event))
{
    Test();
}

void BFMainFrame::Test ()
{
    //spApp_->Test();
}
#endif

void BFMainFrame::OnBackup (wxCommandEvent& WXUNUSED(event))
{
    int iAnswer = wxYES;

    // check for a modified project
    if (spApp_->IsProjectModified())
    {
        // ask for save
        iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it before running the backup?"));

        // save
        if (iAnswer == wxYES)
        {
            wxString strProject;

            if (spApp_->GetCurrentProjectFilename().Len() == 0)
            {
                if (AskSaveProject(strProject))
                    spApp_->SaveProject(strProject);
                else
                    iAnswer = wxCANCEL;
            }
            else
            {
                spApp_->SaveCurrentProject();
            }
        }
    }

    if (iAnswer != wxCANCEL)
    {
        spApp_->Backup();
        new BFLogViewDlg(this, BFRootTask::Instance().GetLastLogFiles());
    }
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
