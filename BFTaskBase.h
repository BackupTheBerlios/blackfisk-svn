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
        static BFCore& Core ()
        { return BFCore::Instance(); }

    public:
        /// ctor
        BFTaskBase ();
        /// virtual dtor
        virtual ~BFTaskBase ();

        /** search for macros (for example "<date>") in 'rStr'
            and replace them with the needed value */
        static wxString& ReplaceMacros (wxString& rStr);
};  // class BFTaskBase


#endif    // BFTASKBASE_H
