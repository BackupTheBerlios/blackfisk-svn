/**
 * Name:        BFCronCtrl.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-06-30
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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
#include <wx/gbsizer.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "BFTimeCtrl.h"
#include "BFMainFrame.h"
#include "ids.h"

#define BFCRONCTRL_ID_RADIO_D	1 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_W	2 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_I	3 + BF_CRONCTRL_ID_HIGHEST

BEGIN_EVENT_TABLE(BFCronCtrl, wxPanel)
	EVT_RADIOBOX ( BFCRONCTRL_ID_RADIO_D,		BFCronCtrl::OnRadio_D)
	EVT_RADIOBOX ( BFCRONCTRL_ID_RADIO_W,		BFCronCtrl::OnRadio_W)
	EVT_RADIOBOX ( BFCRONCTRL_ID_RADIO_I,		BFCronCtrl::OnRadio_I)
END_EVENT_TABLE()

BFCronCtrl::BFCronCtrl (wxWindow* pParent,
                        const wxArrayString& arrCrontabline)
          : wxPanel(pParent),
            arrCrontabline_(arrCrontabline)
{
	// * type *
	pRadioDaily_		= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_D, _("daily"));
	pRadioWeekly_		= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_W, _("weekly"));
	pRadioIntervall_	= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_I, _("intervall"));

	// panels
	pPanelDaily_ = new wxPanel(this);
	pPanelWeekly_ = new wxPanel(this);
	pPanelIntervall_ = new wxPanel(this);

	// * DAILY: time *
	pDTimeCtrl_ = new BFTimeCtrl(pPanelDaily_, wxID_ANY, 7, 38);

	// * WEEKLY: day *
    wxString choicesDay[] = { _("Monday"),
							  _("Tuesday"),
							  _("Wednesday"),
							  _("Thursday"),
							  _("Friday"),
							  _("Saturday"),
							  _("Sunday")};
	pWComboDay_ = new wxComboBox(pPanelWeekly_,
			 					 wxID_ANY,
			 					 "X X X",
								 wxDefaultPosition,
								 wxDefaultSize,
								 7,
								 choicesDay,
								 wxCB_READONLY);
	// * WEEKLY: time *
	pWTimeCtrl_ = new BFTimeCtrl(pPanelWeekly_, wxID_ANY, 7, 38);


	// * INTERVALL *
	pISpinDays_ = new wxSpinCtrl(pPanelIntervall_,
								wxID_ANY,
								wxEmptyString,
								wxDefaultPosition,
								wxDefaultSize,
								wxSP_ARROW_KEYS,
								0,
								30);
	pISpinHours_ = new wxSpinCtrl(pPanelIntervall_,
								 wxID_ANY,
								 wxEmptyString,
								 wxDefaultPosition,
								 wxDefaultSize,
								 wxSP_ARROW_KEYS,
								 0,
								 23);
	pISpinMinutes_ = new wxSpinCtrl(pPanelIntervall_,
								   wxID_ANY,
								   wxEmptyString,
								   wxDefaultPosition,
								   wxDefaultSize,
								   wxSP_ARROW_KEYS,
								   0,
								   59);
	int iWidth = BFMainFrame::GetTextWidth("0000000", pISpinDays_);
	pISpinDays_->SetSize(wxSize(iWidth, -1));
	pISpinHours_->SetSize(wxSize(iWidth, -1));
	pISpinMinutes_->SetSize(wxSize(iWidth, -1));

	// ** sizer and arrange **
	wxBoxSizer* pSizerD = new wxBoxSizer(wxHORIZONTAL);
	pSizerD->Add ( pDTimeCtrl_ );
	pSizerD->Add ( new wxStaticText(pPanelDaily_, wxID_ANY, _("o'clock")), wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pPanelDaily_->SetSizer(pSizerD);

	wxBoxSizer* pSizerW = new wxBoxSizer(wxHORIZONTAL);
	pSizerW->Add ( pWComboDay_ );
	pSizerW->AddStretchSpacer(2);
	pSizerW->Add ( pWTimeCtrl_, wxSizerFlags(0).Border(wxLEFT) );
	pSizerW->Add ( new wxStaticText(pPanelWeekly_, wxID_ANY, _("o'clock")), wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pPanelWeekly_->SetSizer(pSizerW);

	wxBoxSizer* pSizerI = new wxBoxSizer(wxHORIZONTAL);
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("all ")),		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->Add ( pISpinDays_,		wxSizerFlags(0).FixedMinSize() );
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("days ")),	wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->AddStretchSpacer(2);
	pSizerI->Add ( pISpinHours_,		wxSizerFlags(0).FixedMinSize() );
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("hours ")),	wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->AddStretchSpacer(2);
	pSizerI->Add ( pISpinMinutes_,	wxSizerFlags(0).FixedMinSize() );
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("minutes ")), wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->AddStretchSpacer(2);
	pPanelIntervall_->SetSizer(pSizerI);
	
	wxGridBagSizer* pGBSizer = new wxGridBagSizer ();

	pGBSizer->Add ( pRadioDaily_,				wxGBPosition(1, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	pGBSizer->Add ( pPanelDaily_,				wxGBPosition(1, 4), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	pGBSizer->Add ( new wxStaticLine(this),		wxGBPosition(3, 0), wxGBSpan(1, 5), wxEXPAND );
	pGBSizer->Add ( pRadioWeekly_,				wxGBPosition(5, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	pGBSizer->Add ( pPanelWeekly_,				wxGBPosition(5, 4), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT );
	pGBSizer->Add ( new wxStaticLine(this),		wxGBPosition(7, 0), wxGBSpan(1, 5), wxEXPAND );
	pGBSizer->Add ( pRadioIntervall_,			wxGBPosition(9, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	pGBSizer->Add ( pPanelIntervall_,			wxGBPosition(9, 4), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT );
	pGBSizer->Add ( new wxPanel(this),			wxGBPosition(10, 0) );

    wxStaticBoxSizer*	pSizerSB = new wxStaticBoxSizer(wxHORIZONTAL, this);
	pSizerSB->Add ( pGBSizer );
	SetSizer(pSizerSB);

    GetData();
}


/*virtual*/ BFCronCtrl::~BFCronCtrl ()
{
}

void BFCronCtrl::GetData()
{
	// XXX
	return;

    int iType = 0;
    long lVal = 0;

    if ( arrCrontabline_[0].StartsWith("*/") )
        iType = 0;
    else if ( arrCrontabline_[1].StartsWith("*/") )
        iType = 1;
    else if ( arrCrontabline_[2].StartsWith("*/") )
        iType = 2;

    arrCrontabline_[iType].Mid(2).ToLong(&lVal);
    //pComboType_->Select(iType);
    //FillCombos();

    /*if ( (lVal-1) > (long)pComboIntervall_->GetCount() )
        return;

    pComboIntervall_->SetSelection(lVal-1);*/
}


void BFCronCtrl::SetData ()
{
	// XXX
    arrCrontabline_[0] = arrCrontabline_[1] = arrCrontabline_[2] = "*";

	wxString str;
    //wxString str = wxString::Format("*/%d", pComboIntervall_->GetSelection()+1);

/*    switch (pComboType_->GetSelection())
    {
        case 0:
			arrCrontabline_[0] = str;
            break;

        case 1:
            arrCrontabline_[0] = "0";
            arrCrontabline_[1] = str;
            break;

        case 2:
            arrCrontabline_[0] = "0";
            arrCrontabline_[1] = "0";
            arrCrontabline_[2] = str;
            break;
    }*/
}

wxString BFCronCtrl::GetCrontabline ()
{
    SetData();

    return wxString::Format("%s %s %s %s %s %s",
                            arrCrontabline_[0],
                            arrCrontabline_[1],
                            arrCrontabline_[2],
                            arrCrontabline_[3],
                            arrCrontabline_[4],
                            arrCrontabline_[5]);
}


bool /*static*/ BFCronCtrl::IsCrontablineUsable (const wxArrayString& arr)
{
    // enough arguments?
    if (arr.GetCount() < 5)
        return false;

    // month and weekday
    if (arr[3] != "*" || arr[4] != "*")
        return false;

    // minute
    if ( arr[0].StartsWith("*/") && arr[1] != "*" && arr[2] != "*")
        return false;

    // hour
    if ( arr[1].StartsWith("*/") && arr[0] != "0" && arr[2] != "*")
        return false;

    // day
    if ( arr[2].StartsWith("*/") && arr[1] != "0" && arr[0] != "0")
        return false;

    // no error detected
    return true;
}


void BFCronCtrl::OnRadio_D (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(true);
	pRadioWeekly_->SetValue(false);
	pRadioIntervall_->SetValue(false);
}

void BFCronCtrl::OnRadio_W (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(false);
	pRadioWeekly_->SetValue(true);
	pRadioIntervall_->SetValue(false);
}

void BFCronCtrl::OnRadio_I (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(false);
	pRadioWeekly_->SetValue(false);
	pRadioIntervall_->SetValue(true);
}


/*
void BFCronCtrl::FillCombos ()
{
    wxArrayString arrIntervall;
    //wxArrayString arrFixed;
    wxString      str;

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
            //arrFixed.Add(wxString::Format(_("each %d%s minute"), i, str));
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
            //arrFixed.Add(wxString::Format(_("each %d%s hour"), i, str));
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
            //arrFixed.Add(wxString::Format(_("each %d%s day"), i, str));
        }
    }

    // set the combos
    pComboIntervall_->Clear();
    pComboIntervall_->Append(arrIntervall);
    pComboIntervall_->Select(0);
}*/
