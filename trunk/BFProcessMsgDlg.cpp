/**
 * Name:        BFProcessMsgDlg.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-12-28
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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


#include "BFProcessMsgDlg.h"

#include "BFProcessMsgSubject.h"
#include "BFSystem.h"

//
BFProcessMsgDlg::BFProcessMsgDlg (wxWindow* pParent,
                                  BFProcessMsgSubject* pSubject,
                                  int iLines,
                                  int iRows)
               : wxDialog(pParent, wxID_ANY, wxEmptyString),
                 Observer(pSubject)
{
    // lines
    wxString str;
    while (iLines > 0)
    {
        str << _T("\n");
        --iLines;
    }

    // rows
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    pMsg_ = new wxStaticText(this,
                             wxID_ANY,
                             str);
    pMsg_->SetMinSize(wxSize(dc.GetCharWidth() * iRows, pMsg_->GetSize().GetHeight()));
    pMsg_->SetWindowStyle(wxST_NO_AUTORESIZE);
    //pMsg_->SetBackgroundColour(*wxRED);

    // arrange
    wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
    pSizer->Add(pMsg_, wxSizerFlags(0).Expand().Border());
    SetSizerAndFit(pSizer);
    Show();
}


//
/*virtual*/ BFProcessMsgDlg::~BFProcessMsgDlg ()
{
}

/*virtual*/ void BFProcessMsgDlg::ValueChanged (Subject* pSender)
{
    BFProcessMsgSubject* pMsgSubject = dynamic_cast<BFProcessMsgSubject*>(pSender);

    if (pMsgSubject == NULL)
        return;

    SetTitle(pMsgSubject->GetCaption());
    pMsg_->SetLabel(pMsgSubject->GetMsg());

    Layout();
}

