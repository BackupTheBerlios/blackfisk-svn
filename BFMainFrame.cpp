/**
 * implementation of class BFMainFrame
 * 2006-04-05
 ***/

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFApp.h"
#include "Progress.h"
#include "BFDirCtrl.h"
#include "BFBackupTree.h"

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(BFMainFrame, wxFrame)
    EVT_MENU    (ID_OpenProject,        BFMainFrame::OnOpenProject)
    EVT_MENU    (ID_SaveProject,        BFMainFrame::OnSaveProject)
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
    spApp_ = &rApp;

    // set as top window
    spApp_->SetTopWindow(this);

    // ** menu FILE **
    wxMenu *menuFile = new wxMenu;
    menuFile->Append( ID_OpenProject,   _("&Open Project") );
    menuFile->Append( ID_SaveProject,   _("&Save Project") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_About,         _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit,          _("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;

    // ** menu TEST **
    wxMenu* menuTest = new wxMenu;
    menuTest->Append( ID_Test,      _("&Testen") );
    menuTest->AppendSeparator();
    menuTest->Append( ID_Backup,    _("&Backup") );

    menuBar->Append( menuFile,  _("&File") );
    menuBar->Append( menuTest,  _("&Test") );
    SetMenuBar( menuBar );

    // splitter window
    wxSplitterWindow* pSplitter = new wxSplitterWindow(this);
    pBackupCtrl_    = new BFBackupCtrl(pSplitter);
    pDirCtrl_       = new BFDirCtrl(pSplitter);
    pSplitter->SplitVertically(pBackupCtrl_, pDirCtrl_);

    // GIF ani (DEBUG)
    //wxGIFAnimationCtrl* pAniCtrl = new wxGIFAnimationCtrl(this, -1, _T("graphic\\test01.gif"));
    //pAniCtrl->Play();
    //pAniCtrl->FitToAnimation();

    // ** sizer **
    wxSizer* pSizer = new wxBoxSizer (wxVERTICAL);
    pSizer->Add( pSplitter, wxSizerFlags(3).Expand() );
    //pSizer->Add( pAniCtrl, wxSizerFlags(1).Expand() );

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

void BFMainFrame::OnOpenProject (wxCommandEvent& event)
{
    // check for a modified project
    if (spApp_->IsProjectModified())
    {
        // ask for save
        int iAnswer = QuestionYesNoCancel(_("The current project is modified!\nDo you want to save it before open a new one?"));

        // cancel/abort
        if (iAnswer == wxID_CANCEL)
            return;

        // save
        if (iAnswer == wxID_YES)
            if (SaveProject() == false)
                return;
    }

    wxFileDialog dlg
    (
        this,
        _("message"),
        wxEmptyString,
        wxEmptyString,
        _("OctopusBackup files (*.ob)|*.ob"),
        wxFD_OPEN
    );

    dlg.CentreOnParent();
    //dlg.SetDirectory(wxGetHomeDir());

    if (dlg.ShowModal() == wxID_OK)
        if ( spApp_->OpenProject(dlg.GetPath()) == false)
            BFSystem::Fatal(wxString::Format(_("Can not open Project %s"), dlg.GetPath().c_str()), _T("BFMainFrame::OpenProject()"));
}

bool BFMainFrame::SaveProject ()
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
        if ( spApp_->SaveProject(dlg.GetPath()))
        {
            return true;
        }
        else
        {
            BFSystem::Fatal(wxString::Format(_("Can not save Project %s"), dlg.GetPath().c_str()), _T("BFMainFrame::SaveProject()"));
            return false;
        }
    }
    else
    {
        return false;
    }
}

void BFMainFrame::OnSaveProject (wxCommandEvent& event)
{
    SaveProject();
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
