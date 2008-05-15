/**
 * Name:        BFExcludeCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-03-27
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
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


#include "BFExcludeCtrl.h"

#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dirdlg.h>

#include "BFTask.h"
#include "ids.h"

#define BF_EXCLUDECTRL_BUTTON_ADD       1 + BF_EXCLUDECTRL_ID_HIGHEST

BEGIN_EVENT_TABLE(BFExcludeCtrl, wxPanel)
    EVT_BUTTON(BF_EXCLUDECTRL_BUTTON_ADD,   BFExcludeCtrl::OnButtonAdd)
END_EVENT_TABLE()
//
BFExcludeCtrl::BFExcludeCtrl (wxWindow* pParent, BFTask* pTask)
             : wxPanel(pParent, wxID_ANY),
               pPath_(NULL),
               pListPaths_(NULL),
               pTask_(pTask)
{
    // path
    pPath_ = new wxTextCtrl(this, wxID_ANY);

    // add button
    wxButton* pButtonAdd = new wxButton(this,
                                        BF_EXCLUDECTRL_BUTTON_ADD,
                                        _("Add"),
                                        wxDefaultPosition,
                                        wxDefaultSize,
                                        wxBU_EXACTFIT);

    // exclude list
    pListPaths_ = new wxListBox(this, wxID_ANY);

    pListPaths_->Connect
    (
        wxID_ANY,
        wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
        wxCommandEventHandler(BFExcludeCtrl::OnDoubleClick),
        NULL,
        this
    );

    // sizer and arrange
    wxBoxSizer* pSizer      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pSizerPath  = new wxBoxSizer(wxHORIZONTAL);
    pSizerPath->Add(pPath_,     wxSizerFlags(3).Expand());
    pSizerPath->Add(pButtonAdd, wxSizerFlags(1).Center());
    pSizer->Add(pSizerPath,   wxSizerFlags(0).Expand());
    pSizer->Add(pListPaths_,  wxSizerFlags(0).Expand());
    SetSizerAndFit(pSizer);
}


//
/*virtual*/ BFExcludeCtrl::~BFExcludeCtrl ()
{
}


/** write from 'pListPaths_' to 'rExclude'
and clear 'rExclude' before */
void BFExcludeCtrl::SetData (wxArrayString& rExclude)
{
    rExclude.Clear();
    rExclude = pListPaths_->GetStrings();
}

/** read 'rExclude' to 'pListPaths_'
and clear 'pListPaths_' before */
void BFExcludeCtrl::GetData (const wxArrayString& rExclude)
{
    pListPaths_->Clear();
    pListPaths_->Append(rExclude);
}

void BFExcludeCtrl::OnButtonAdd (wxCommandEvent& rEvent)
{
    if (pPath_->IsEmpty())
    { /* open dir dialog */
        wxDirDialog dlgDir(this,
                           _("Choose a directory to exclude from the backup"),
                           pTask_->GetSource());

        if (dlgDir.ShowModal() == wxID_OK)
            pListPaths_->Append(dlgDir.GetPath());
    }
    else
    { /* just add the entered path to list */
        pListPaths_->Append(pPath_->GetValue());
        pPath_->Clear();
    }
}

void BFExcludeCtrl::OnDoubleClick (wxCommandEvent& rEvent)
{
    pListPaths_->Delete(rEvent.GetInt());
}
