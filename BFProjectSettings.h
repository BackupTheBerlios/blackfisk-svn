/**
 * BFProjectSettings.h
 * definition of class BFProjectSettings
 * <comment>
 * 2007-04-20
 ***/

#ifndef BFPROJECTSETTINGS_H
#define BFPROJECTSETTINGS_H

#include <wx/string.h>
#include "jbarchive.h"

enum BF_VerboseLevel
{
    BFVERBOSE_WARNING,
    BFVERBOSE_ERROR,
    BFVERBOSE_FATAL
};

enum BF_StopLevel
{
    BFDO_STOPPRJ,
    BFDO_STOPTSK,
    BFDO_ASK,
    BFDO_IGNORE
};

///
class BFProjectSettings
{
    private:
        ///
        BF_VerboseLevel     iVerboseLevel_;
        /// what is to do on a fatal error
        BF_StopLevel        iStopLevelOnFatal_;
        /// what is to do on a normal error
        BF_StopLevel        iStopLevelOnError_;
        /// what is to do on a warning
        BF_StopLevel        iStopLevelOnWarning_;
        /// path to the location where the backup-process is logged
        wxString            strBackupLogLocation_;

        ///
        void SetStopLevel (BF_StopLevel& iTarget, BF_StopLevel iStopLevel);

    protected:
        /// proteced members

    public:
        /// constructor
        BFProjectSettings ();

        /// virtual destructor
        virtual ~BFProjectSettings ();

        /// set all values to default
        void SetDefault();

        ///
        void SetVerboseLevel (BF_VerboseLevel iLevel);
        ///
        BF_VerboseLevel GetVerboseLevel ();
        ///
        void SetStopLevelOnFatal (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnFatal ();
        ///
        void SetStopLevelOnError (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnError ();
        ///
        void SetStopLevelOnWarning (BF_StopLevel iLevel);
        ///
        BF_StopLevel GetStopLevelOnWarning ();
        ///
        const wxString& GetBackupLogLocation ();
        ///
        void SetBackupLogLocation (const wxChar* strFile);

        ///
        bool Serialize (jbArchive& rA);
};    // class BFProjectSettings

#endif    // BFPROJECTSETTINGS_H
