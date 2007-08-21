/**
 * Name:        Progress.h
 * Purpose:     Progress, ProgressWithMessage, ProgressTotal class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2002-05-28
 * Copyright:   (c) 2002 Christian Buhtz <blackfisk@web.de>
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

#ifndef PROGRESS_H
#define PROGRESS_H

#include "ObserverPattern.h"
#include <wx/string.h>

/** represent a progress (range, current value) on data layer */
class Progress : public Subject
{
	private:
        ///
		const long      lBEGIN_;
        ///
		long            end_;
        ///
		long            actual_;

	public:
        ///
		Progress();
        ///
		virtual ~Progress();

        ///
        virtual bool IncrementActual ()
        { return SetActual(GetActual() + 1); }
        ///
		bool SetActual (long actual);
        /// from 0 to 'end'
		void SetRange (long end);
        ///
        void DoEnd ();
        ///
        bool IsEnd ();
        ///
		long GetActual (void);
        ///
        long GetRange ();

        ///
		unsigned short GetProgress ();
};  // class Progress


/// a progress including a message (for example: currently copied file)
class ProgressWithMessage : public Progress
{
    private:
        /// static label (for example "working on file...")
        wxString        strLabel_;
        /// the message that can be updated (for example "/home/user/file")
        wxString        strMessage_;

    public:
        ///
        ProgressWithMessage ();
        ///
        virtual ~ProgressWithMessage ();

        ///
        bool IncrementActualWithMessage (const wxChar* message);
        ///
        void SetLabel (const wxChar* label);
        ///
        const wxChar* GetMessage ();
        ///
        const wxChar* GetLabel ();
};  // class ProgressWithMessage


/** a progress that represant a couple of progresses
    it is a subject AND a observer */
class ProgressTotal : public Progress, public Observer
{
	private:
        /// number of sub-progress
        long        lProgressCount_;
        /// current sub-progress
        long        lProgressCurrent_;

        ///
        ProgressTotal();

	public:
        /** pSubProgress in one (observed) progress that represent all
            sub progresses. It just start new if it reaged its end. */
		ProgressTotal(long lProgressCount, Progress* pSubProgress);
		///
		virtual ~ProgressTotal();

        /// just increment 'lProgressCurrent_'
        virtual bool IncrementActual ();
		///
		virtual void ValueChanged (Subject* pSender);

};  // class ProgressTotal


#endif	// PROGRESS_H
