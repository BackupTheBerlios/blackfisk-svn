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
#include <wx/checkbox.h>
#include "BFTimeCtrl.h"
#include "BFSettings.h"
#include "BFMainFrame.h"
#include "BFBackup.h"
#include "BFCore.h"
#include "BFProject.h"
#include "ids.h"
#include "blackfisk.h"

#define BFCRONCTRL_ID_RADIO_D		1 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_W		2 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_I		3 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBO_W		4 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBO_I		5 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_CHECK_R       6 + BF_CRONCTRL_ID_HIGHEST

BEGIN_EVENT_TABLE(BFCronCtrl, wxPanel)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_D,		BFCronCtrl::OnRadio_D)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_W,		BFCronCtrl::OnRadio_W)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_I,		BFCronCtrl::OnRadio_I)
    EVT_CHECKBOX    ( BFCRONCTRL_ID_CHECK_R,        BFCronCtrl::OnCheck_R)
END_EVENT_TABLE()

BFCronCtrl::BFCronCtrl (wxWindow* pParent,
                        const wxString& strCrontabline)
          : wxPanel(pParent),
            strCrontabline_(strCrontabline)
{
	// * type *
	pRadioDaily_		= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_D, _("daily   "));
	pRadioWeekly_		= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_W, _("weekly   "));
	pRadioIntervall_	= new wxRadioButton(this, BFCRONCTRL_ID_RADIO_I, _("intervall   "));

	// panels
	pPanelDaily_ = new wxPanel(this);
	pPanelWeekly_ = new wxPanel(this);
	pPanelIntervall_ = new wxPanel(this);
    pPanelRetry_ = new wxPanel(this);

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
			 					 BFCRONCTRL_ID_COMBO_W,
			 					 "X X X",
								 wxDefaultPosition,
								 wxDefaultSize,
								 7,
								 choicesDay,
								 wxCB_READONLY);
	pWComboDay_->SetSelection(0);

	// * WEEKLY: time *
	pWTimeCtrl_ = new BFTimeCtrl(pPanelWeekly_, wxID_ANY, 7, 38);


	// * INTERVALL *
	pISpin_ = new wxSpinCtrl(pPanelIntervall_,
							 wxID_ANY,
							 "0");
	int iWidth = BFMainFrame::GetTextWidth("000000000", pISpin_);
	pISpin_->SetSize(wxSize(iWidth, -1));

	wxString choicesIntervall[] = { _("days"), _("hours"), _("minutes") };
	pICombo_ = new wxComboBox (pPanelIntervall_,
							   BFCRONCTRL_ID_COMBO_I,
							   "X X X",
							   wxDefaultPosition,
							   wxDefaultSize,
							   3,
							   choicesIntervall,
							   wxCB_READONLY);
	pICombo_->SetSelection(0);

    // * RETRY *
    pCheckRetry_ = new wxCheckBox(this,
                                  BFCRONCTRL_ID_CHECK_R,
                                  _("retry the backup if it couldn't start"));
    pSpinRetryHours_ = new wxSpinCtrl(pPanelRetry_,
                                      wxID_ANY,
                                      "0");
    pSpinRetryHours_->SetRange(0, 24);
    pSpinRetryMinutes_ = new wxSpinCtrl(pPanelRetry_,
                                        wxID_ANY,
                                        "0");
    pSpinRetryMinutes_->SetRange(0, 59);
    iWidth = BFMainFrame::GetTextWidth("0000000", pSpinRetryHours_);
	pSpinRetryHours_->SetSize(wxSize(iWidth, -1));
    pSpinRetryMinutes_->SetSize(wxSize(iWidth, -1));

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
	pSizerI->Add ( pISpin_,		wxSizerFlags(0).FixedMinSize() );
	pSizerI->AddStretchSpacer(1);
	pSizerI->Add ( pICombo_,			wxSizerFlags(0) );
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

    wxBoxSizer* pSizerR = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pSizerRspin = new wxBoxSizer(wxHORIZONTAL);
    pSizerRspin->Add ( pSpinRetryMinutes_,      wxSizerFlags(0).FixedMinSize() );
    pSizerRspin->Add ( new wxStaticText(pPanelRetry_, wxID_ANY, _(" minutes and ")), wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL) );
    pSizerRspin->Add ( pSpinRetryHours_,        wxSizerFlags(0).FixedMinSize() );
    pSizerRspin->Add ( new wxStaticText(pPanelRetry_, wxID_ANY, _(" hours")), wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL) );
    pSizerR->Add ( new wxStaticText(pPanelRetry_, wxID_ANY, _("try every")), wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border() );
    pSizerR->AddStretchSpacer(1);
    pSizerR->Add ( pSizerRspin );
    pPanelRetry_->SetSizer(pSizerR);

    wxStaticBoxSizer*	pSizerSB = new wxStaticBoxSizer(wxVERTICAL, this);
	pSizerSB->Add ( pGBSizer );
    pSizerSB->Add ( new wxStaticLine(this), wxSizerFlags(0).Center().Expand().Border(wxTOP, 25) );
    pSizerSB->Add ( new wxStaticLine(this), wxSizerFlags(0).Center().Expand().Border(wxBOTTOM, 5) );
    pSizerSB->Add ( pCheckRetry_, wxSizerFlags(0).Border() );
    pSizerSB->Add ( pPanelRetry_, wxSizerFlags(0).Border() );
	SetSizer(pSizerSB);

    GetData();
}


/*virtual*/ BFCronCtrl::~BFCronCtrl ()
{
}


bool BFCronCtrl::IsCrontablineDaily ()
{
	// positive example: "45 12 * * * command"

	// minute and hour fields without '*' and '/'
	if ( arrCrontabline_[0].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[0].Find('/') != wxNOT_FOUND
	  || arrCrontabline_[1].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[1].Find('/') != wxNOT_FOUND )
	  return false;

	// week, day and month are '*'
	if ( arrCrontabline_[2] != "*"
	  || arrCrontabline_[3] != "*"
	  || arrCrontabline_[4] != "*" )
	  return false;

	return true;
}

bool BFCronCtrl::IsCrontablineWeekly ()
{
	// positive example: "45 12 * * 2 command"

	// minute, hour and weekday fields without '*' and '/'
	if ( arrCrontabline_[0].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[0].Find('/') != wxNOT_FOUND
	  || arrCrontabline_[1].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[1].Find('/') != wxNOT_FOUND
	  || arrCrontabline_[4].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[4].Find('/') != wxNOT_FOUND )
	  return false;

	// day and month are '*'
	if ( arrCrontabline_[2] != "*"
	  || arrCrontabline_[3] != "*" )
	  return false;

	return true;
}


bool BFCronCtrl::IsCrontablineIntervall ()
{
	// positiv examples:
	// 0 0 */3 * * (days)
	// 0 */3 * * * (hours)
	// */3 * * * * (minutes)

	// invervall in days
	if ( arrCrontabline_[0].Find('0') != wxNOT_FOUND
	  || arrCrontabline_[1].Find('0') != wxNOT_FOUND
	  || arrCrontabline_[2].Find("*/") != wxNOT_FOUND
	  || arrCrontabline_[3].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[4].Find('*') != wxNOT_FOUND )
	  return true;

	// invervall in hours
	if ( arrCrontabline_[0].Find('0') != wxNOT_FOUND
	  || arrCrontabline_[1].Find("*/") != wxNOT_FOUND
	  || arrCrontabline_[2].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[3].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[4].Find('*') != wxNOT_FOUND )
	  return true;

	// invervall in minutes
	if ( arrCrontabline_[0].Find("*/") != wxNOT_FOUND
	  || arrCrontabline_[1].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[2].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[3].Find('*') != wxNOT_FOUND
	  || arrCrontabline_[4].Find('*') != wxNOT_FOUND )
	  return true;

  return false;
}

void BFCronCtrl::GetData_Daily()
{
	long lHour, lMinute;
	
	arrCrontabline_[1].ToLong(&lHour);
	arrCrontabline_[0].ToLong(&lMinute);

	pDTimeCtrl_->SetValue((int)lHour, (int)lMinute);
}


void BFCronCtrl::GetData_Weekly()
{
	long lHour, lMinute, lDay;
	
	arrCrontabline_[1].ToLong(&lHour);
	arrCrontabline_[0].ToLong(&lMinute);
	arrCrontabline_[4].ToLong(&lDay);

	// in the crontab weekday 0 & 7 are sunday
	if (lDay == 0)
		lDay = 7;

	// time
	pWTimeCtrl_->SetValue ( (int)lHour, (int)lMinute );

	// weekday
	pWComboDay_->SetSelection ( lDay - 1 );
}


void BFCronCtrl::GetData_Intervall()
{
	wxString strVal;	

	// days
	if ( arrCrontabline_[2].Find("*/") != wxNOT_FOUND )
	{
		strVal = arrCrontabline_[2];
		pICombo_->SetSelection(0);
	}

	// hourse
	if ( arrCrontabline_[1].Find("*/") != wxNOT_FOUND )
	{
		strVal = arrCrontabline_[1];
		pICombo_->SetSelection(1);
	}

	// minutes
	if ( arrCrontabline_[0].Find("*/") != wxNOT_FOUND )
	{
		strVal = arrCrontabline_[0];
		pICombo_->SetSelection(2);
	}

	//
	long lVal;

	strVal.Remove(0, 2);
	strVal.ToLong(&lVal);

	pISpin_->SetValue((int)lVal);
}

void BFCronCtrl::GetData()
{
    // * RETRY *
    if ( BFProject::Instance().IsRetry() )
    {
        pCheckRetry_->SetValue(true);
        pSpinRetryHours_->SetValue( BFProject::Instance().GetRetryHours() );
        pSpinRetryMinutes_->SetValue( BFProject::Instance().GetRetryMinutes() );
    }
    else
    {
        pCheckRetry_->SetValue(false);
    }

    OnCheck_R(wxCommandEvent());

    //
    BFBackup::ParseCrontabline(strCrontabline_, arrCrontabline_);

	// * DAILY * time
	if ( IsCrontablineDaily() )
	{
		pRadioDaily_->SetValue(true);
		OnRadio_D(wxCommandEvent());
		GetData_Daily();
		return;
	}

	// * WEEKLY * weekday + time
	if ( IsCrontablineWeekly() )
	{
		pRadioWeekly_->SetValue(true);
		OnRadio_W(wxCommandEvent());
		GetData_Weekly();
		return;
	}

	// * INTERVALL * days, hours, minutes
	if ( IsCrontablineIntervall() )
	{
		pRadioIntervall_->SetValue(true);
		OnRadio_I(wxCommandEvent());
		GetData_Intervall();
		return;
	}

	// * nothing fit *
	wxString strQuestion = wxString::Format
	(
		_("The current crontab entry is to complex.\n" \
		  "(%s)\n" \
		  "\nIt is not possible to modify it with blackfisk!\n" \
		  "\nIt can be modified manualy in the crontab file.\n" \
		  "(%s)\n" \
		  "\nOr you can reset the crontab entry.\n" \
		  "\nShould the crontab entry be reset?"),
		GetCrontabline(),
		BFSettings::Instance().GetCrontab()
	);

	if ( BFMainFrame::QuestionYesNo ( strQuestion ) )
	{
		// yes
		BFCore::Instance().ReplaceLineInFile
		(
			// filename
			BFSettings::Instance().GetCrontab(),
			// old
			BFBackup::Instance().GetCrontabline(),
			// new
			BFBackup::Instance().GetCrontablineDefault()
		);

		strCrontabline_ = BFBackup::Instance().GetCrontabline();

		GetData();
	}
	else
	{
		// no
		GetParent()->Close();
	}
}


void BFCronCtrl::SetData ()
{
	// * DAILY *
	if ( pRadioDaily_->GetValue() )
	{
		int iHour, iMinute;
		pDTimeCtrl_->GetValue(iHour, iMinute);
		
		arrCrontabline_[1] = wxString::Format("%d", iHour);
		arrCrontabline_[0] = wxString::Format("%d", iMinute);
		arrCrontabline_[2] = '*';
		arrCrontabline_[3] = '*';
		arrCrontabline_[4] = '*';
	}

	// * WEEKLY *
	if ( pRadioWeekly_->GetValue() )
	{
		int iHour, iMinute;
		pWTimeCtrl_->GetValue(iHour, iMinute);
		
		arrCrontabline_[1] = wxString::Format("%d", iHour);
		arrCrontabline_[0] = wxString::Format("%d", iMinute);
		arrCrontabline_[2] = '*';
		arrCrontabline_[3] = '*';
		arrCrontabline_[4] = wxString::Format("%d", pWComboDay_->GetSelection() + 1);
	}

	// * INTERAVALL *
	if ( pRadioIntervall_->GetValue() )
	{
		wxString strVal = wxString::Format("*/%d", pISpin_->GetValue());

		switch ( pICombo_->GetSelection() )
		{
			// days
			case 0:
				arrCrontabline_[0] = '0';
				arrCrontabline_[1] = '0';
				arrCrontabline_[2] = strVal;
				break;

			// hours
			case 1:
				arrCrontabline_[0] = '0';
				arrCrontabline_[1] = strVal;
				arrCrontabline_[2] = '*';
				break;

			// minutes
			case 2:
				arrCrontabline_[0] = strVal;
				arrCrontabline_[1] = '*';
				arrCrontabline_[2] = '*';
				break;
		}

		arrCrontabline_[3] = '*';
		arrCrontabline_[4] = '*';
	}

	// create new crontab line
	strCrontabline_ = wxString::Format("%s %s %s %s %s %s",
									   arrCrontabline_[0],
									   arrCrontabline_[1],
									   arrCrontabline_[2],
									   arrCrontabline_[3],
									   arrCrontabline_[4],
									   arrCrontabline_[5] );

    // * RETRY *
    if ( pCheckRetry_->IsChecked() == false )
    {
        BFProject::Instance().SetRetry(0, 0);
    }
    else
    {
        BFProject::Instance().SetRetry
        (
            pSpinRetryMinutes_->GetValue(),
            pSpinRetryHours_->GetValue()
        );
    }
    BFProject::Instance().SetModified();
}

wxString BFCronCtrl::GetCrontabline ()
{
    return strCrontabline_;
}


void BFCronCtrl::OnRadio_D (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(true);
	BFMainFrame::SetFontBold(pRadioDaily_);
	pPanelDaily_->Enable();
	pRadioWeekly_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioWeekly_, false);
	pPanelWeekly_->Disable();
	pRadioIntervall_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioIntervall_, false);
	pPanelIntervall_->Disable();
}

void BFCronCtrl::OnRadio_W (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioDaily_, false);
	pPanelDaily_->Disable();
	pRadioWeekly_->SetValue(true);
	BFMainFrame::SetFontBold(pRadioWeekly_);
	pPanelWeekly_->Enable();
	pRadioIntervall_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioIntervall_, false);
	pPanelIntervall_->Disable();
}

void BFCronCtrl::OnRadio_I (wxCommandEvent& rEvent)
{
	pRadioDaily_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioDaily_, false);
	pPanelDaily_->Disable();
	pRadioWeekly_->SetValue(false);
	BFMainFrame::SetFontBold(pRadioWeekly_, false);
	pPanelWeekly_->Disable();
	pRadioIntervall_->SetValue(true);
	BFMainFrame::SetFontBold(pRadioIntervall_);
	pPanelIntervall_->Enable();
}

void BFCronCtrl::OnCheck_R (wxCommandEvent& rEvent)
{
    if ( pCheckRetry_->IsChecked() )
        pPanelRetry_->Enable();
    else
        pPanelRetry_->Disable();
}
