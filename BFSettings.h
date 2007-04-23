/**
 * BFSettings.h
 * definition of class BFSettings
 * <comment>
 * 2007-04-20
 ***/

#ifndef BFSETTINGS_H
#define BFSETTINGS_H

#include "BFProjectSettings.h"
#include "jbarchive.h"

#define BFSETTINGS_CURRENT_VERSION 1000

///
class BFSettings
{
    private:
        /** project-specific settings by default */
        BFProjectSettings   defaultPrj_;
        /** display backup-tree with replaced activate
            macros ("2000-01-01") or not ("<date>") */
        bool                bReplaceMacros_;
        /** display the filesystem with files
            or just with dirs and volumes */
        bool                bWithFiles_;
        /** max size of the application logfile in KB
            size does not matter if it is '0' */
        long                lMaxLogFileSize_;

        ///
        static BFSettings   sSettings_;

        /// private ctor
        BFSettings ();

    public:
        ///
        static BFSettings& Instance ()
        {
            return sSettings_;
        }

        /// virtual destructor
        virtual ~BFSettings ();

        ///
        const BFProjectSettings& GetDefaultProjectSettings ();
        ///
        void SetDefaultProjectSettings (const BFProjectSettings& rDefaultPrj);
        ///
        bool GetReplaceMacros ();
        ///
        void SetReplaceMacros (bool bReplace);
        ///
        bool GetWithFiles ();
        ///
        void SetWithFiles (bool bWithFiles);
        ///
        long GetMaxLogFileSize ();
        ///
        void SetMaxLogFileSize (long lSize);

        ///
        bool Serialize (jbArchive& rA);
};    // class BFSettings

#endif    // BFSETTINGS_H
