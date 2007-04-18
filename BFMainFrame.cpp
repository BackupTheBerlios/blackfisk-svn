/**
 * implementation of class BFMainFrame
 * 2006-04-05
 ***/

#include "BFMainFrame.h"

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>
#include <wx/imaglist.h>

#include "blackfisk.h"
#include "BFApp.h"
#include "Progress.h"
#include "BFDirCtrl.h"
#include "BFBackupTree.h"
#include "BFIconTable.h"

BEGIN_EVENT_TABLE(BFMainFrame, wxFrame)
    EVT_MENU    (ID_OpenProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProject,        BFMainFrame::OnProject)
    EVT_MENU    (ID_SaveProjectAs,      BFMainFrame::OnProject)
    EVT_MENU    (ID_CloseProject,       BFMainFrame::OnProject)
    EVT_MENU    (ID_NewProject,         BFMainFrame::OnProject)
    EVT_MENU    (ID_Quit,               BFMainFrame::OnQuit)
    EVT_MENU    (ID_About,              BFMainFrame::OnAbout)
    EVT_MENU    (ID_Test,               BFMainFrame::OnTest)
    EVT_MENU    (ID_Backup,             BFMainFrame::OnBackup)
END_EVENT_TABLE()


/*private static*/ BFApp* BFMainFrame::spApp_ = NULL;

/*private*/ BFMainFrame::BFMainFrame (BFApp& rApp)
            : wxFrame (NULL,
                       -1,
                       wxString::Format(_("%s %s by %s with %s"), BF_PRGNAME, BF_VERSION, BF_AUTHOR, wxVERSION_STRING)),
                       msgDlg_(this)
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


    // ** menu PROJECT **
    wxMenu *menuFile = new wxMenu;
    menuFile->Append( ID_NewProject,    _("&New/Close Project") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_OpenProject,   _("&Open Project") );
    menuFile->Append( ID_SaveProject,   _("&Save Project") );
    menuFile->Append( ID_SaveProjectAs, _("&Save Project as ...") );

    // ** menu BLACKFISK **
    wxMenu* menuBlackfisk = new wxMenu;
    menuBlackfisk->Append( ID_Test,      _("&Testen") );
    menuBlackfisk->Append( ID_Backup,    _("&Backup") );
    menuBlackfisk->AppendSeparator();
    menuBlackfisk->Append( ID_Quit,      _("E&xit") );

    // ** menu HELP **
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append( ID_About,         _("&About...") );

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile,          _("&Project") );
    menuBar->Append( menuBlackfisk,     _("&blackfisk") );
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

    Show(TRUE);
    SetSize(wxSize(650, 600));
    Center();
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
    int i = 0;
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
                        spApp_->OpenProject(strProject);
            break;

        case ID_SaveProject:
            spApp_->SaveCurrentProject();
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
    }
}

bool BFMainFrame::AskModification ()
{
    // check for a modified project
    if (spApp_->IsProjectModified())
    {
        // ask for save
        int iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it?"));

        // cancel/abort
        if (iAnswer == wxID_CANCEL)
            return false;

        // save
        if (iAnswer == wxID_YES)
            if (spApp_->SaveCurrentProject() == false)
                return false;
    }

    // wxID_NO
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
        _("save message"),
        wxEmptyString,
        wxEmptyString,
        _("OctopusBackup files (*.ob)|*.ob"),
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
    wxMessageBox(wxVERSION_STRING,
        _("About Hello World"), wxOK | wxICON_INFORMATION, this);
}


void BFMainFrame::OnTest (wxCommandEvent& WXUNUSED(event))
{
    spApp_->Test();
}


void BFMainFrame::OnBackup (wxCommandEvent& WXUNUSED(event))
{
    spApp_->Backup();
}

bool BFMainFrame::QuestionYesNo (const wxChar* strQuestion)
{
    wxMessageDialog dlg(this, strQuestion, _("Question"), wxYES_NO | wxICON_QUESTION);

    int rc = dlg.ShowModal();

    if (rc == wxID_YES)
        return true;

    return false;
}

int BFMainFrame::QuestionYesNoCancel (const wxChar* strQuestion)
{
    wxMessageDialog dlg(this, strQuestion, _("Question"), wxYES_NO | wxCANCEL | wxICON_QUESTION);

    return dlg.ShowModal();
}
