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
#include "BFSettings.h"
#include "BFMainFrame.h"
#include "BFBackup.h"
#include "BFCore.h"
#include "ids.h"
#include "blackfisk.h"

//XXX
#include "BFSystem.h"

#define BFCRONCTRL_ID_RADIO_D		1 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_W		2 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_RADIO_I		3 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBO_W		4 + BF_CRONCTRL_ID_HIGHEST
#define BFCRONCTRL_ID_COMBO_I		5 + BF_CRONCTRL_ID_HIGHEST

BEGIN_EVENT_TABLE(BFCronCtrl, wxPanel)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_D,		BFCronCtrl::OnRadio_D)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_W,		BFCronCtrl::OnRadio_W)
	EVT_RADIOBUTTON ( BFCRONCTRL_ID_RADIO_I,		BFCronCtrl::OnRadio_I)
	EVT_COMBOBOX	( BFCRONCTRL_ID_COMBO_W,		BFCronCtrl::OnCombo_W)
	EVT_COMBOBOX	( BFCRONCTRL_ID_COMBO_I,		BFCronCtrl::OnCombo_I)
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
	// * WEEKLY: time *
	pWTimeCtrl_ = new BFTimeCtrl(pPanelWeekly_, wxID_ANY, 7, 38);


	// * INTERVALL *
	pISpin_ = new wxSpinCtrl(pPanelIntervall_,
							 wxID_ANY,
							 "0000");
	/*pISpinHours_ = new wxSpinCtrl(pPanelIntervall_,
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
								   59);*/
	int iWidth = BFMainFrame::GetTextWidth("0000000000", pISpin_);
	pISpin_->SetSize(wxSize(iWidth, -1));
	/*pISpinHours_->SetSize(wxSize(iWidth, -1));
	pISpinMinutes_->SetSize(wxSize(iWidth, -1));*/

	wxString choicesIntervall[] = { _("days"), _("hours"), _("minutes") };
	pICombo_ = new wxComboBox (pPanelIntervall_,
							   BFCRONCTRL_ID_COMBO_I,
							   "X X X",
							   wxDefaultPosition,
							   wxDefaultSize,
							   3,
							   choicesIntervall,
							   wxCB_READONLY);

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
	/*pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("days ")),	wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->AddStretchSpacer(2);
	pSizerI->Add ( pISpinHours_,		wxSizerFlags(0).FixedMinSize() );
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("hours ")),	wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );
	pSizerI->AddStretchSpacer(2);
	pSizerI->Add ( pISpinMinutes_,	wxSizerFlags(0).FixedMinSize() );
	pSizerI->Add ( new wxStaticText(pPanelIntervall_, wxID_ANY, _("minutes ")), wxSizerFlags(0).Border(wxLEFT).Align(wxALIGN_CENTER_VERTICAL) );*/
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

    wxStaticBoxSizer*	pSizerSB = new wxStaticBoxSizer(wxHORIZONTAL, this);
	pSizerSB->Add ( pGBSizer );
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


void BFCronCtrl::GetData_Time()
{
	long lHour, lMinute;
	
	arrCrontabline_[1].ToLong(&lHour);
	arrCrontabline_[0].ToLong(&lMinute);

	pDTimeCtrl_->SetValue((int)lHour, (int)lMinute);
}

void BFCronCtrl::GetData()
{
	//
    BFBackup::ParseCrontabline(strCrontabline_, arrCrontabline_);

	// * DAILY * time
	if ( IsCrontablineDaily() )
	{
		pRadioDaily_->SetValue(true);
		OnRadio_D(wxCommandEvent());
		GetData_Time();
		return;
	}

	// * WEEKLY * weekday + time

	// * INTERVALL * days, hours, minutes

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

		// XXX
		BFSystem::Fatal ( wxString::Format("after Repalce\n\nstrCrontabline_: %s", strCrontabline_) );
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
	}

	// * INTERAVALL *
	if ( pRadioIntervall_->GetValue() )
	{
	}

	// create new crontab line
	strCrontabline_ = wxString::Format("%s %s %s %s %s %s",
									   arrCrontabline_[0],
									   arrCrontabline_[1],
									   arrCrontabline_[2],
									   arrCrontabline_[3],
									   arrCrontabline_[4],
									   arrCrontabline_[5] );
}

wxString BFCronCtrl::GetCrontabline ()
{
    // XXX needed? SetData();

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


void BFCronCtrl::OnCombo_I (wxCommandEvent& rEvent)
{
}


void BFCronCtrl::OnCombo_W (wxCommandEvent& rEvent)
{
}
