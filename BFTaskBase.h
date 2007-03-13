/**
 * BFTaskBase.h
 * definition of class BFTaskBase
 * <comment>
 * 2006-05-29
 ***/

#ifndef BFTASKBASE_H
#define BFTASKBASE_H

#include "BFCore.h"
#include "wxArchive\include\wx\wxArchive.h"

typedef wxArchive jbArchive;

#define BFTASK_MACRO_DATE   _T("<date>")

/// lowest base class for all Task classes
class BFTaskBase
{
    private:

    protected:
        /// reference to the BFCore instance
        BFCore& Core ()
        { return BFCore::Instance(); }

        /** search for macros (for example "<date>") in 'rStr'
            and replace them with the needed value */
        wxString& ReplaceMacros (wxString& rStr);

    public:
        /// ctor
        BFTaskBase ();
        /// virtual dtor
        virtual ~BFTaskBase ();
};  // class BFTaskBase


#endif    // BFTASKBASE_H
