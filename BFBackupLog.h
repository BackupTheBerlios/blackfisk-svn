/**
 * BFBackupLog.h
 * definition of class BFBackupLog
 * <comment>
 * 2007-04-20
 ***/

#ifndef BFBACKUPLOG_H
#define BFBACKUPLOG_H

#include "ObserverPattern.h"
#include <wx/datetime.h>
#include <wx/arrstr.h>
#include <vector>
#include "BFTask.h"

///
class BFTaskLog
{
    private:
        /// task start
        wxDateTime          timeStart_;
        /// task end
        wxDateTime          timeEnd_;
        ///
        wxArrayString       arrLog_;
        ///
        long                lCountWarnings_;
        ///
        long                lCountErrors_;
        ///
        long                lCountFatal_;
        ///
        bool                bNormalEnd_;
        ///
        BFTask&             rTask_;

    public:
        ///
        BFTaskLog (BFTask& rTask);
        ///
        virtual ~BFTaskLog ();

        ///
        void Started ();
        ///
        void Finished ();

        ///
        void Message (BFMessageType type,
                      const wxDateTime& timestamp,
                      const wxChar* strMessage,
                      const wxChar* strLocation);
        ///
        bool Write ();

        ///
        wxString GetSummary ();
        ///
        wxString GetFileName ();
};

///
typedef std::vector<BFTaskLog*> BFTaskLogVector;

///
class BFBackupLog : public Observer
{
    private:
        /// backup start
        wxDateTime      timeStart_;
        /// backup end
        wxDateTime      timeEnd_;
        ///
        BFTaskLogVector vecTaskLogs_;
        ///
        bool            bIsSaved_;

        ///
        bool WhileBackup ();
        /// write all informations about the backup and its task to the log-files
        bool Write ();

    protected:
        /// proteced members

    public:
        /// constructor
        BFBackupLog ();
        /// virtual destructor
        virtual ~BFBackupLog ();

        /// mark the beginning of a backup process
        void BackupStarted ();
        /// mark the end of a backup process
        void BackupFinished ();

        ///
        void TaskStarted (BFTask& rTask);
        ///
        void TaskFinished ();

        ///
        virtual void ValueChanged (Subject* pSender);

};    // class BFBackupLog

#endif    // BFBACKUPLOG_H
