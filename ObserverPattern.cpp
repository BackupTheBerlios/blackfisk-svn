#include "ObserverPattern.h"

/**
 * class Observer
 ***/

Observer::Observer ()
        : subject_(NULL)
{}

Observer::Observer (Subject* subject)
        : subject_(NULL)
{
    SetSubject(subject);
}

/*virtual*/ Observer::~Observer ()
{
    if (subject_ != NULL)
        subject_->deregisterObserver (this);
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
}

void Subject::deregisterObserver (Observer* observer)
{
	ItVecObserver itObserver;

    // search for the observer how wants to be deregistered
	for (itObserver = vecObserver_.begin();
		itObserver != vecObserver_.end();
		itObserver++)
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
		itObserver++)
	{
		(*itObserver)->ValueChanged (this);
	}
}
