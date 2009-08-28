/**
 * Name:        BFTimeCtrl.h
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

#ifndef BFTIMECTRL_H
#define BFTIMECTRL_H

#include <wx/wx.h>
#include <wx/spinbutt.h>
#include <wx/sizer.h>


/** */
class BFTimeCtrl : public wxBoxSizer
{
	private:
		/** This control is the text part of the time-picker. */
		class TextCtrl : public wxTextCtrl
		{
			private:
				///
				BFTimeCtrl*		pTimeCtrl_;
				///
				wxString		strBuffer_;
				///
				int				iPos_;
				///
				int				iMin_;
				///
				int				iMax_;

				/**
				 * This event handler is called when a key is pressed in the text ctrl
				 *
				 * @param event
				 */
				void OnChar (wxKeyEvent& event);

				/**
				 * This funciton updates the caret position
				 */
				void UpdatePosition ();
				
				/**
				 * This function sets the selection to the appropriate time part
				 */
				void SelectPart ();
				
				/**
				 * This function updates the buffer
				 */
				void UpdateBuffer ();
				
				/**
				 * This function updates the current time part (hour, minute, second or
				 * am/pm) based on the type of the change
				 *
				 * @param type
				 */
				void Increment (bool bPositiv);

				/**
				 * Flush the buffer
				 */
				void FlushBuffer (bool clear = true);
				 
				/**
				 * This function checks whether the valuefalls within the limit
				 *
				 * @param value
				 */
				void FixValue (long& rlValue);
				
				
				DECLARE_DYNAMIC_CLASS (TextCtrl)
				DECLARE_EVENT_TABLE ()

			public:
				/// ctor
				TextCtrl ();				
				/// ctor
				TextCtrl (wxWindow* pParent, BFTimeCtrl* pTimeCtrl,
						 const wxString& strValue = wxEmptyString);
				  
				/// dtor
				virtual ~TextCtrl ();
				
				/**
				 * This function is called from the spin control to 
				 * increment the value
				 *
				 * @param IncrementType type
				 */
				void ApplyIncrement (bool bPositiv);				
		};	// class BFTimeCtrl::TextCtrl


		/** This control is the spin button of the time picker. */
		class SpinButton : public wxSpinButton
		{
			private:
				///
 				BFTimeCtrl*		pTimeCtrl_;

				DECLARE_DYNAMIC_CLASS (SpinButton)
				DECLARE_EVENT_TABLE ()

			public:
 				/// ctor
 				SpinButton ();
			 	/// ctor 				
 				SpinButton (wxWindow* pParent, BFTimeCtrl* pTimeCtrl);
			 	
 				/// dtor
 				virtual ~SpinButton();
			 	
 				/**
 				 * These functions are called when the spin button is pressed
 				 *
 				 * @param wxSpinEvent&
 				 */
 				void OnSpinUp (wxSpinEvent& event);
			 	
 				/**
 				 * @see @ref #OnSpinUp(wxSpinEvent& event)
 				 */
 				void OnSpinDown (wxSpinEvent& event);			 	
		};	// class BFTimeCtrl::SpinButton

	private:
		///
		TextCtrl*		pTextCtrl_;
		///
		SpinButton*		pSpinButton_;
		
		DECLARE_DYNAMIC_CLASS(BFTimeCtrl)

	public:
		/// ctor
		BFTimeCtrl (wxWindow*		pParent		= NULL,
				    wxWindowID		id			= wxID_ANY,
					int				iHour		= 0,
					int				iMinute		= 0,
				    const wxPoint&	pos			= wxDefaultPosition,
				    const wxSize&	size		= wxDefaultSize);
			
		// dtor
		virtual ~BFTimeCtrl ();
		
		/**
		 * This function triggers an update on the text control part, indicating
		 * an increment or a decrement with the parameter @p type.
		 *
		 * @param type
		 */
		void UpdateTextCtrl (bool bPositiv);
		
		/**
		 * This function returns the value of the time control.
		 *
		 * @return Time, const wxString&
		 */
		wxString GetValue () const;

		///
		const TextCtrl* GetTextControl ();
};


#endif // BFTIMECTRL_H
