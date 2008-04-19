/**
 * Name:        ObserverPattern.h
 * Purpose:     Observer and Subject class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2003-05-12
 * Copyright:   (c) 2003 Christian Buhtz <blackfisk@web.de>
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

#ifndef OBSERVER_PATTERN_H
#define OBSERVER_PATTERN_H

class Subject;


///
class Observer
{
	private:
        ///
		Subject* subject_;
		/** the subject order the observers on their priority
            default is 1 */
		long     priority_;
        ///
		Observer ();

    protected:
        ///
        const Subject* GetSubject () { return subject_; }
        ///
        long GetPriority ();

	public:
        ///
		Observer (Subject* subject, long priority = 100);
		/// deregister itself from the subject
		virtual ~Observer ();

        /// set 'subject_', (de)register if needed
        void SetSubject (Subject* subject);

        ///
		virtual void ValueChanged (Subject* pSender) = 0;

        ///
        bool operator < (const Observer& o) const
        {
            // the priority is descendening (higher first, lower last)
            return this->priority_ > o.priority_;
        }
};	// class Observer


/**
 * SUBJECT
 ***/

#include <vector>

typedef std::vector<Observer*> VecObserver;
typedef VecObserver::iterator ItVecObserver;


///
class Subject
{
    private:
        ///
		VecObserver vecObserver_;

	public:
        ///
		Subject ();
		///
		virtual ~Subject ();

        ///
		void registerObserver (Observer* observer);
		///
		void deregisterObserver (Observer* observer);
		/// tell all observers that the subject changed
		void broadcastObservers ();

        ///
        long GetObserverCount ();
};	// class Subject

#endif	// OBSERVER_PATTERN_H
