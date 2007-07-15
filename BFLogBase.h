/**
 * BFLogBase.h
 * definition of class BFLogBase
 * <comment>
 * 2007-02-21
 ***/

#ifndef BFLOGBASE_H
#define BFLOGBASE_H

#include <wx/file.h>
#include "ObserverPattern.h"


/// base class for a log-file class
class BFLogBase : public Observer
{
    private:
        /** max size of a file in bytes
            it is handled in CareSize()
            set it to '0' if the size does not matter */
        long        lMaxSize_;

    protected:
        /// the name of the logfile
        wxString    strLogFileName_;

        /// direct interface to the log file
        wxFile      fileLog_;

        /** take care that size of the logfile is not to big */
        void CareSize ();

        /// return the maximum size in bytes
        long GetMaxSize ();
        ///
        void SetMaxSize (long lMaxSizeInBytes);

        /// the ctor set its 'subject' by itself
        BFLogBase (const wxChar* strLogFileName, long lMaxSize = 0);

    public:
        /// virtual destructor
        virtual ~BFLogBase ();
};    // class BFLogBase

#endif    // BFLOGBASE_H
