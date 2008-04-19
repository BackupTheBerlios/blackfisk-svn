/**
 * Name:        ObserverPattern.cpp
 * Purpose:     Observer and Subject class implementation
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

#include "ObserverPattern.h"
#include <algorithm>

Observer::Observer ()
        : subject_(NULL),
          priority_(100)
{}

Observer::Observer (Subject* subject, long priority /*= 100*/)
        : subject_(NULL),
          priority_(priority)
{
    SetSubject(subject);
}

/*virtual*/ Observer::~Observer ()
{
    if (subject_ != NULL)
        subject_->deregisterObserver (this);
}

long Observer::GetPriority ()
{
    return priority_;
}

void Observer::SetSubject (Subject* subject)
{
    if (subject_ != subject)
    {
        if (subject_ != NULL)
            subject_->deregisterObserver(this);
    }

    subject_ = subject;

    if (subject_ != NULL)
        subject_->registerObserver(this);
}


/* virtual ValueChanged (Subject* pSender) = 0 */

/**
 * class Subject
 ***/

Subject::Subject ()
{
}

/*virtual*/ Subject::~Subject ()
{
    /* NULLing an observer will cause that
       the Observer derigister itself */
    while (vecObserver_.size() > 0)
        vecObserver_[0]->SetSubject(NULL);
}


long Subject::GetObserverCount ()
{
    return vecObserver_.size();
}

void Subject::registerObserver (Observer* observer)
{
	vecObserver_.push_back (observer);
	std::sort(vecObserver_.begin(), vecObserver_.end());
}

void Subject::deregisterObserver (Observer* observer)
{
	ItVecObserver itObserver;

    // search for the observer how wants to be deregistered
	for (itObserver = vecObserver_.begin();
		itObserver != vecObserver_.end();
		++itObserver)
	{
		if ((*itObserver) == observer)
		{
		    // deregister the found observer
			vecObserver_.erase (itObserver);
			break;
		}
	}
}

void Subject::broadcastObservers ()
{
	ItVecObserver itObserver;

	for (itObserver = vecObserver_.begin();
		itObserver != vecObserver_.end();
		++itObserver)
	{
		(*itObserver)->ValueChanged (this);
	}
}
