/**
 * Name:        BFLogViewDlg.h
 * Purpose:     BFLogViewDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-21
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***/

#include "BFLogViewDlg.h"

#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/textfile.h>

#include "BFRootTask.h"

BEGIN_EVENT_TABLE(BFLogViewDlg, wxDialog)
  EVT_CLOSE (BFLogViewDlg::OnClose)
END_EVENT_TABLE()

//
BFLogViewDlg::BFLogViewDlg (wxWindow* pParent)
            : wxDialog(pParent, wxID_ANY, wxString(_("Log View")))
{
    SetWindowStyle(GetWindowStyle() | wxRESIZE_BORDER);
    Init();
    ShowModal();
}


//
/*virtual*/ BFLogViewDlg::~BFLogViewDlg ()
{
}

void BFLogViewDlg::Init ()
{
    // the files
    wxArrayString arrLogs(BFRootTask::Instance().GetLastLogFiles());

    // the book
    wxNotebook* pBook = new wxNotebook (this, wxID_ANY);
    pBook->SetMinSize(wxSize(500, 400));
    wxSizer* pBookSizer = new wxBoxSizer(wxVERTICAL);
    pBookSizer->Add(pBook);

    // the log files
    for (int i = 0; i < arrLogs.GetCount(); ++i)
    {
        // the log file
        wxTextFile fileLog;
        fileLog.Open(arrLogs[i]);

        // error while opening
        if ( !(fileLog.IsOpened()) )
        {
            BFSystem::Error(wxString::Format(_("can not open the log file %s"), arrLogs[i].c_str()), _T("BFLogViewDlg::Init()"));
            continue;
        }

        // create the text control
        wxTextCtrl* pCtrl = new wxTextCtrl(pBook,
                                           wxID_ANY,
                                           wxEmptyString,
                                           wxDefaultPosition,
                                           wxDefaultSize,
                                           wxTE_MULTILINE | wxTE_READONLY);
        wxFont font(pCtrl->GetFont());
        font.SetFaceName(_T("Courier New"));
        font.SetFamily(wxFONTFAMILY_MODERN);
        font.SetPointSize(8);
        pCtrl->SetFont(font);

        // read the file
        for (wxString strLine = fileLog.GetFirstLine();
             !fileLog.Eof();
             strLine = fileLog.GetNextLine())
        {
            (*pCtrl) << strLine << _T("\n");
        }



        // add as a tab
        pBook->AddPage(pCtrl, arrLogs[i]);
    }

    SetSizerAndFit(pBookSizer);
}

void BFLogViewDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}
