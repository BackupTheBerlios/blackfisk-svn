#ifndef OBSERVER_PATTERN_H
#define OBSERVER_PATTERN_H

class Subject;

/**
 * OBSERVER
 ***/


///
class Observer
{
	private:
        ///
		Subject* subject_;
        ///
		Observer ();

    protected:
        ///
        const Subject* GetSubject () { return subject_; }

	public:
        ///
		Observer (Subject* subject);
		/// deregister itself from the subject
		virtual ~Observer ();

        /// set 'subject_', (de)register if needed
        void SetSubject (Subject* subject);

        ///
		virtual void ValueChanged (Subject* pSender) = 0;

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
		void broadcastObserver ();
		/// just a synonym
		void broadcastObservers ();

        ///
        long GetObserverCount ();
};	// class Subject

#endif	// OBSERVER_PATTERN_H
