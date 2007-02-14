/**
 * OBTaskBase.h
 * definition of class OBTaskBase
 * <comment>
 * 2006-05-29
 ***/

#ifndef OBTASKBASE_H
#define OBTASKBASE_H

#include "OBCore.h"
#include "wxArchive\include\wx\wxArchive.h"

typedef wxArchive jbArchive;

#define OBTASK_MACRO_DATE   _T("<date>")

/// lowest base class for all Task classes
class OBTaskBase
{
    private:

    protected:
        /// reference to the OBCore instance
        OBCore& Core ()
        { return OBCore::Instance(); }

        /** search for macros (for example "<date>") in 'rStr'
            and replace them with the needed value */
        wxString& ReplaceMacros (wxString& rStr);

    public:
        /// ctor
        OBTaskBase ();
        /// virtual dtor
        virtual ~OBTaskBase ();
};  // class OBTaskBase


#endif    // OBTASKBASE_H
