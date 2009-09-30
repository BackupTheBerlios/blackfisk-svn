/**
 * Name:        BFTimeCtrl.cpp
 * Purpose:
 * Based on:	class MyTimeCtrl
 *              authored by "jeyoung@priscimon.com" in 2003
 *				related Links:
 *				http://coding.moris.org/wp-content/MyTimeCtrl.zip
 *				http://wiki.wxwidgets.org/MyTimerCtrl
 *				no licence information about the original code
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-06-28
 * Copyright:   (c) 2009 Christian Buhtz <exsudat@gmx.de>
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

#include <wx/datetime.h>

#include "BFTimeCtrl.h"
#include "BFMainFrame.h"


IMPLEMENT_DYNAMIC_CLASS(BFTimeCtrl::TextCtrl, wxTextCtrl)

BFTimeCtrl::TextCtrl::TextCtrl()
					: wxTextCtrl()
{
}

BFTimeCtrl::TextCtrl::TextCtrl (wxWindow* pParent, BFTimeCtrl* pTimeCtrl,
							   const wxString& strValue /* = wxEmptyString*/)
					: wxTextCtrl (pParent,
								  wxID_ANY,
								  strValue,
								  wxDefaultPosition,
								  wxDefaultSize)
{
	pTimeCtrl_	= pTimeCtrl;
	strBuffer_	= wxEmptyString;
}

/*virtual*/ BFTimeCtrl::TextCtrl::~TextCtrl()
{
}


void BFTimeCtrl::TextCtrl::Increment(bool bPositiv)
{
	/**
	 * Format 00:00
	 *        01234
	 */
	 
	long lValue;
	strBuffer_.ToLong(&lValue);

	if (bPositiv)
		lValue++;
	else
		lValue--;
	
	FixValue (lValue);

	strBuffer_ = wxString::Format("%.2d", (int) lValue);
}


void BFTimeCtrl::TextCtrl::ApplyIncrement (bool bPositiv)
{
	UpdatePosition();
	SelectPart();	
	UpdateBuffer();
	Increment(bPositiv);
	FlushBuffer();
	SelectPart();
}

void BFTimeCtrl::TextCtrl::OnChar (wxKeyEvent& event)
{
	UpdatePosition();
	SelectPart();
	UpdateBuffer();
		
	int iKeycode = event.GetKeyCode();
	
	if (iKeycode >= 48 && iKeycode <= 57 && iPos_ < 8)
	{
		char cKeycode = iKeycode;
		wxString strTemp = strBuffer_ + (wchar_t) cKeycode;

		long lValue;
		strTemp.Right(2).ToLong(&lValue);
		
		if (lValue < iMin_ || lValue > iMax_)
			strBuffer_ = wxString("0") + (wchar_t) cKeycode;
		else 
			strBuffer_ = wxString::Format("%.2d", (int) lValue);

		FlushBuffer();
	}
	
	switch (iKeycode)
	{
		// TAB
		case WXK_TAB:
			FlushBuffer();
			
			if (!event.ShiftDown() )
				iPos_ += 3;
			else
				iPos_ -= 3;
			
			#ifndef __WXMAC__
			if (iPos_ < 0 || iPos_ > 4)
					event.Skip();
			#endif // __WXMAC__

			break;

		// ARROW LEFT
		case WXK_LEFT:
			FlushBuffer();

			iPos_ -= 3;
			if (iPos_ < 0)
				iPos_ = 0;
			break;

		// ARROW RIGHT
		case WXK_RIGHT:
			FlushBuffer();

			iPos_ += 3;
			if (iPos_ > 4) 
				iPos_ = 4;
			break;
						
		case WXK_UP:
			Increment(true);
			FlushBuffer();
			break;
			
		case WXK_DOWN:
			Increment(false);
			FlushBuffer();
			break;
		
		case 65:
		case 97:
		case 80:
		case 112:
			FlushBuffer();
		 	break;
	}
	SelectPart();
}

void BFTimeCtrl::TextCtrl::UpdatePosition()
{
	iPos_ = GetInsertionPoint();
}

void BFTimeCtrl::TextCtrl::SelectPart() 
{
	if (iPos_ < 3) 
	{
		iMin_ = 0;
		iMax_ = 23;
	}
	
	if (iPos_ > 2 && iPos_ < 5)
	{
		iMin_ = 0;
		iMax_ = 59;
	}

	if (iPos_ < 3)
		SetSelection(0, 2);
	
	if (iPos_ > 2 && iPos_ < 5)
		SetSelection(3, 5);	
}

void BFTimeCtrl::TextCtrl::UpdateBuffer()
{
	strBuffer_ = GetStringSelection();
}


void BFTimeCtrl::TextCtrl::FlushBuffer (bool clear /* = true*/)
{
	long lStart;
	long lEnd;

	GetSelection(&lStart, &lEnd);

	Replace(lStart, lEnd, strBuffer_);
}

void BFTimeCtrl::TextCtrl::FixValue(long& rlValue)
{
	if (rlValue < iMin_) 
		rlValue = iMax_;
	
	if (rlValue > iMax_)
		rlValue = iMin_;
}

BEGIN_EVENT_TABLE(BFTimeCtrl::TextCtrl, wxTextCtrl)
	EVT_CHAR(BFTimeCtrl::TextCtrl::OnChar)
END_EVENT_TABLE()	




IMPLEMENT_DYNAMIC_CLASS(BFTimeCtrl::SpinButton, wxSpinButton)

BFTimeCtrl::SpinButton::SpinButton ()
					  : wxSpinButton ()
{	
}


BFTimeCtrl::SpinButton::SpinButton (wxWindow* pParent, BFTimeCtrl* pTimeCtrl)
					  : wxSpinButton (pParent,
									  wxID_ANY,
									  wxDefaultPosition,
									  wxDefaultSize)
{
	pTimeCtrl_ = pTimeCtrl;
}


/*virtual*/ BFTimeCtrl::SpinButton::~SpinButton()
{
}


void BFTimeCtrl::SpinButton::OnSpinUp (wxSpinEvent& event)
{
	pTimeCtrl_->UpdateTextCtrl (true);
}

void BFTimeCtrl::SpinButton::OnSpinDown (wxSpinEvent& event)
{
	pTimeCtrl_->UpdateTextCtrl(false);
}


BEGIN_EVENT_TABLE(BFTimeCtrl::SpinButton, wxSpinButton)
	EVT_SPIN_UP		(wxID_ANY,	BFTimeCtrl::SpinButton::OnSpinUp)
	EVT_SPIN_DOWN	(wxID_ANY,	BFTimeCtrl::SpinButton::OnSpinDown)
END_EVENT_TABLE()



IMPLEMENT_DYNAMIC_CLASS(BFTimeCtrl, wxControl)

BFTimeCtrl::BFTimeCtrl (wxWindow*		pParent		/* = NULL*/,
						wxWindowID		id			/* = wxID_ANY*/,
						int				iHour		/* = 0 */,
						int				iMinute		/* = 0 */,
						const wxPoint&	pos			/* = wxDefaultPosition*/,
						const wxSize&	size		/* = wxDefaultSize*/)
		  : wxBoxSizer(wxHORIZONTAL)
{
	// text ctrl
	pTextCtrl_ = new TextCtrl (pParent, this, wxString::Format("%.2d:%.2d", iHour, iMinute));
	pTextCtrl_->SetWindowStyle (wxTE_PROCESS_TAB | wxTEXT_ALIGNMENT_CENTER);
	pTextCtrl_->SetMaxLength (5);
	pTextCtrl_->SetSize( wxSize( BFMainFrame::GetTextWidth("000:000", pTextCtrl_), -1) );

	// spin button
	pSpinButton_ = new SpinButton (pParent, this);
	pSpinButton_->SetSize ( wxSize(-1, pTextCtrl_->GetSize().GetHeight()) );

	// arrange
	Add(pTextCtrl_,		wxSizerFlags(0).FixedMinSize().Align(wxALIGN_CENTER_VERTICAL));
	Add(pSpinButton_,	wxSizerFlags(0).FixedMinSize().Align(wxALIGN_CENTER_VERTICAL));

	Show(true);
}

/*virtual*/ BFTimeCtrl::~BFTimeCtrl()
{
}


void BFTimeCtrl::UpdateTextCtrl(bool bPositiv)
{
	pTextCtrl_->SetFocus();
	pTextCtrl_->ApplyIncrement(bPositiv);
}

wxString BFTimeCtrl::GetValue() const
{
	return pTextCtrl_->GetValue();
}

wxString BFTimeCtrl::GetValue (int& rHour, int& rMinute) const
{
	wxString str = GetValue();
	long lHour, lMinute;

	str.Before(':').ToLong(&lHour);
	str.After(':').ToLong(&lMinute);

	rHour = (int)lHour;
	rMinute = (int)lMinute;

	return str;
}

bool BFTimeCtrl::SetValue (int iHour, int iMinute)
{
	// hour valid?
	if ( iHour < 0 || iHour > 23 )
		return false;

	// minute valid?
	if ( iMinute < 0 || iMinute > 59 )
		return false;

	pTextCtrl_->SetValue( wxString::Format("%.2d:%.2d", iHour, iMinute) );

	return true;
}

const BFTimeCtrl::TextCtrl* BFTimeCtrl::GetTextControl ()
{
	return pTextCtrl_;
}