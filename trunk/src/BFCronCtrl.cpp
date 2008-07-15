/**
 * Name:        BFCronCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-06-30
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


#include "BFCronCtrl.h"

#include <wx/sizer.h>

#include "ids.h"

#define BFCRONCTRL_ID_COMBOTYPE                     1 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBOINTERVALL                2 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBOFIXED                    3 + BF_CRONCTRL_ID_HIGHEST

BEGIN_EVENT_TABLE(BFCronCtrl, wxPanel)
    EVT_RADIOBUTTON(wxID_ANY,                   BFCronCtrl::OnRadioButton)
    EVT_COMBOBOX(BFCRONCTRL_ID_COMBOTYPE,       BFCronCtrl::OnCombo)
END_EVENT_TABLE()

BFCronCtrl::BFCronCtrl (wxWindow* pParent)
          : wxPanel(pParent)
{
    wxString choicesType[] = { _("hourly"),
                               _("daily"),
                               _("monthly") };

    pComboType_ = new wxComboBox(this,
                                 BFCRONCTRL_ID_COMBOTYPE,
                                 "X X X",
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 3,
                                 choicesType,
                                 wxCB_READONLY);
    pComboType_->Select(0);

    pComboIntervall_ = new wxComboBox(this,
                                      BFCRONCTRL_ID_COMBOINTERVALL,
                                      "11",
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      0,
                                      NULL,
                                      wxCB_READONLY);

    pComboFixed_ = new wxComboBox(this,
                                  BFCRONCTRL_ID_COMBOFIXED,
                                  "33",
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  0,
                                  NULL,
                                  wxCB_READONLY);

    pRadioIntervall_ = new wxRadioButton(this,
                                         wxID_ANY,
                                         _("Interval"),
                                         wxDefaultPosition,
                                         wxDefaultSize,
                                         wxRB_GROUP);

    pRadioFixed_ = new wxRadioButton(this,
                                     wxID_ANY,
                                     _("Fixed"));

    // sizer and arrange
    wxBoxSizer*         pSizerV = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer*    pSizerFG = new wxFlexGridSizer(2, 25, 5);
    wxStaticBoxSizer*   pSizerSB = new wxStaticBoxSizer(wxVERTICAL, this);
    pSizerFG->Add ( pRadioIntervall_,   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL) );
    pSizerFG->Add ( pComboIntervall_,   wxSizerFlags(0).Expand() );
    pSizerFG->Add ( pRadioFixed_,       wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL) );
    pSizerFG->Add ( pComboFixed_,       wxSizerFlags(0).Expand() );
    pSizerSB->Add ( pSizerFG,   wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 15) );
    pSizerV->Add ( pComboType_, wxSizerFlags(0).Center().DoubleBorder() );
    pSizerV->Add ( pSizerSB,    wxSizerFlags(0).Center().Border() );
    SetSizer(pSizerV);

    // init
    CheckRadios();
    FillCombos();
}


/*virtual*/ BFCronCtrl::~BFCronCtrl ()
{
}

void BFCronCtrl::OnCombo (wxCommandEvent& rEvent)
{
    FillCombos();
}

void BFCronCtrl::FillCombos ()
{
    wxArrayString arrIntervall;
    wxArrayString arrFixed;
    wxString      str;
/*
1st
2nd
3rd
4th
*/
    // hourly
    if (pComboType_->GetSelection() == 0)
    {
        for (long i = 1; i < 60; ++i)
        {
            switch (i)
            {
                case 1:
                    str = _("st");
                    break;

                case 2:
                    str = _("nd");
                    break;

                case 3:
                    str = _("rd");
                    break;

                default:
                    str = _("th");
            }

            arrIntervall.Add(wxString::Format(_("every %d minutes"), i));
            arrFixed.Add(wxString::Format(_("each %d%s minute"), i, str));
        }
    }
    // daily
    else if (pComboType_->GetSelection() == 1)
    {
        for (long i = 1; i < 24; ++i)
        {
            switch (i)
            {
                case 1:
                    str = _("st");
                    break;

                case 2:
                    str = _("nd");
                    break;

                case 3:
                    str = _("rd");
                    break;

                default:
                    str = _("th");
            }

            arrIntervall.Add(wxString::Format(_("every %d hours"), i));
            arrFixed.Add(wxString::Format(_("each %d%s hour"), i, str));
        }
    }
    // monthly
    else if (pComboType_->GetSelection() == 2)
    {
        for (long i = 1; i < 31; ++i)
        {
            switch (i)
            {
                case 1:
                    str = _("st");
                    break;

                case 2:
                    str = _("nd");
                    break;

                case 3:
                    str = _("rd");
                    break;

                default:
                    str = _("th");
            }

            arrIntervall.Add(wxString::Format(_("every %d days"), i));
            arrFixed.Add(wxString::Format(_("each %d%s day"), i, str));
        }
    }

    // set the combos
    pComboFixed_->Clear();
    pComboFixed_->Append(arrFixed);
    pComboFixed_->Select(0);
    pComboIntervall_->Clear();
    pComboIntervall_->Append(arrIntervall);
    pComboIntervall_->Select(0);
}

void BFCronCtrl::CheckRadios ()
{
    pComboIntervall_->Enable(pRadioIntervall_->GetValue());

    pComboFixed_->Enable(pRadioFixed_->GetValue());
}

void BFCronCtrl::OnRadioButton (wxCommandEvent& rEvent)
{
    CheckRadios();
}
