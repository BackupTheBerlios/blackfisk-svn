/**
 * implementation of class OBMainFrame
 * 2006-04-05
 ***/

#include "OctopusBackup.h"
#include "OBMainFrame.h"
#include "OBApp.h"
#include "OBMessageWindow.h"
#include "Progress.h"
#include "OBDirCtrl.h"
#include "OBBackupCtrl.h"

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>

BEGIN_EVENT_TABLE(OBMainFrame, wxFrame)
    EVT_MENU    (ID_OpenProject,        OBMainFrame::OnOpenProject)
    EVT_MENU    (ID_SaveProject,        OBMainFrame::OnSaveProject)
    EVT_MENU    (ID_Quit,               OBMainFrame::OnQuit)
    EVT_MENU    (ID_About,              OBMainFrame::OnAbout)
    EVT_MENU    (ID_Test,               OBMainFrame::OnTest)
    EVT_MENU    (ID_Backup,             OBMainFrame::OnBackup)
END_EVENT_TABLE()


/*private static*/ OBApp* OBMainFrame::spApp_ = NULL;

/*private*/ OBMainFrame::OBMainFrame (OBApp& rApp)
            : wxFrame (NULL,
                       -1,
                       wxString::Format(_("%s %s by %s with %s"), OB_PRGNAME, OB_VERSION, OB_AUTHOR, wxVERSION_STRING))
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
    pBackupCtrl_    = new OBBackupCtrl(pSplitter);
    pDirCtrl_       = new OBDirCtrl(pSplitter);
    pSplitter->SplitVertically(pBackupCtrl_, pDirCtrl_);

    // message window (DEBUG)
    pMsgWin_ = new OBMessageWindow(this);

    // GIF ani (DEBUG)
    //wxGIFAnimationCtrl* pAniCtrl = new wxGIFAnimationCtrl(this, -1, _T("graphic\\test01.gif"));
    //pAniCtrl->Play();
    //pAniCtrl->FitToAnimation();

    // ** sizer **
    wxSizer* pSizer = new wxBoxSizer (wxVERTICAL);
    pSizer->Add( pSplitter, wxSizerFlags(3).Expand() );
    pSizer->Add( pMsgWin_,  wxSizerFlags(1).Expand() );
    //pSizer->Add( pAniCtrl, wxSizerFlags(1).Expand() );

    CreateStatusBar();
    SetStatusText( _("Welcome to wxWindows!") );

    SetSizer(pSizer);

    Show(TRUE);
    SetSize(wxSize(650, 600));
    Center();
}

/*virtual*/ OBMainFrame::~OBMainFrame ()
{
    int i = 0;
}

void OBMainFrame::OnOpenProject (wxCommandEvent& event)
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
        wxOPEN
    );

    dlg.CentreOnParent();
    //dlg.SetDirectory(wxGetHomeDir());

    if (dlg.ShowModal() == wxID_OK)
        if ( spApp_->OpenProject(dlg.GetPath()) == false)
            Message(MsgFATAL, wxString::Format(_("Can not open Project %s"), dlg.GetPath().c_str()));
}

bool OBMainFrame::SaveProject ()
{
    wxFileDialog dlg
    (
        this,
        _("save message"),
        wxEmptyString,
        wxEmptyString,
        _("OctopusBackup files (*.ob)|*.ob"),
        wxSAVE | wxOVERWRITE_PROMPT
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
            Message(MsgFATAL, wxString::Format(_("Can not save Project %s"), dlg.GetPath().c_str()));
            return false;
        }
    }
    else
    {
        return false;
    }
}

void OBMainFrame::OnSaveProject (wxCommandEvent& event)
{
    SaveProject();
}

void OBMainFrame::OnQuit (wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void OBMainFrame::OnAbout (wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxVERSION_STRING,
        _("About Hello World"), wxOK | wxICON_INFORMATION, this);
}


void OBMainFrame::OnTest (wxCommandEvent& WXUNUSED(event))
{
    spApp_->Test();
}


void OBMainFrame::OnBackup (wxCommandEvent& WXUNUSED(event))
{
    spApp_->Backup();
}

bool OBMainFrame::QuestionYesNo (wxString strQuestion)
{
    wxMessageDialog dlg(this, strQuestion, _("Question"), wxYES_NO | wxICON_QUESTION);

    int rc = dlg.ShowModal();

    if (rc == wxID_YES)
        return true;

    return false;
}

int OBMainFrame::QuestionYesNoCancel (wxString strQuestion)
{
    wxMessageDialog dlg(this, strQuestion, _("Question"), wxYES_NO | wxCANCEL | wxICON_QUESTION);

    return dlg.ShowModal();
}

int OBMainFrame::Message (wxString strMessage)
{
    Message(MsgINFO, strMessage);
}

int OBMainFrame::Message (OBMessageType type, wxString strMessage, bool bLog /*= false*/)
{
    // init
    long                lStyle = 0;
    OBLogType           logType;
    wxString            strCaption;
    wxMessageDialog*    pDlg = NULL;

    // handle different message types
    switch (type)
    {
    	case MsgINFO:
            strCaption  = _("Information");
            logType     = LogINFO;
            lStyle      = wxOK | wxICON_INFORMATION;
    		break;

        case MsgERROR:
            strCaption  = _("Error");
            logType     = LogERROR;
            lStyle      = wxOK | wxICON_ERROR;
            OBCore::Instance().Log(LogERROR, strMessage);
            break;

        case MsgFATAL:
            strCaption  = _("Fatal Error");
            logType     = LogERROR;
            lStyle      = wxOK | wxICON_EXCLAMATION;
            OBCore::Instance().Log(LogFATAL, strMessage);
            break;

        case MsgDEBUG:
            strCaption  = _("Debug Message");
            logType     = LogDEBUG;
            lStyle      = wxOK;
            OBCore::Instance().Log(LogDEBUG, strMessage);
            break;

    	default:
            strCaption  = _("Unknown Message Type");
            logType     = LogFATAL;
            lStyle      = wxOK | wxICON_EXCLAMATION;
            OBCore::Instance().Log(OBLogType(-1), strMessage);
    		break;
    }

    // DEBUG
    pMsgWin_->Add(strMessage);

    // log the message
    if (bLog)
        OBCore::Instance().Log(logType, strMessage);

    // show the message
    wxMessageDialog dlg(this, strMessage, strCaption, lStyle);
    return dlg.ShowModal();
}


