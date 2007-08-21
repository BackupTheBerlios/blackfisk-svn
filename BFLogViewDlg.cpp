/**
 * Name:        BFLogViewDlg.h
 * Purpose:     BFLogViewDlg class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-07-21
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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
BFLogViewDlg::BFLogViewDlg (wxWindow* pParent, const wxArrayString& arrLogs)
            : wxDialog(pParent, wxID_ANY, wxString(_("Log View"))),
              arrLogs_(arrLogs)
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
    // the book
    wxNotebook* pBook = new wxNotebook (this, wxID_ANY);
    pBook->SetMinSize(wxSize(570, 400));
    wxSizer* pBookSizer = new wxBoxSizer(wxVERTICAL);
    pBookSizer->Add(pBook);

    // the log files
    for (int i = 0; i < arrLogs_.GetCount(); ++i)
    {
        // the log file
        wxTextFile fileLog;
        fileLog.Open(arrLogs_[i]);

        // error while opening
        if ( !(fileLog.IsOpened()) )
        {
            BFSystem::Error(wxString::Format(_("can not open the log file %s"), arrLogs_[i].c_str()), _T("BFLogViewDlg::Init()"));
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
        pBook->AddPage(pCtrl, arrLogs_[i]);
    }

    SetSizerAndFit(pBookSizer);
}

void BFLogViewDlg::OnClose (wxCloseEvent& event)
{
    Destroy();
}
