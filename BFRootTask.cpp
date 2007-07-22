/**
 * implementation of class BFRootTask
 * 2006-05-29
 ***/

#include "BFRootTask.h"

#include <wx/wfstream.h>
#include <wx/tokenzr.h>

#include "BFTaskProgressDlg.h"
#include "BFBackupTree.h"
#include "Progress.h"
#include "BFIconTable.h"
#include "BFBackupLog.h"
#include "blackfisk.h"

#define BFROOTTASK_DEFAULT_NAME _("unnamed")

BFRootTaskData::BFRootTaskData ()
              : strName_(BFROOTTASK_DEFAULT_NAME),
                bModified_(false)
{
}


/*virtual*/BFRootTaskData::~BFRootTaskData ()
{
    ClearTaskVector();
}

void BFRootTaskData::ClearTaskVector ()
{
    for (int i = 0; i < vecTasks_.size(); ++i)
        delete vecTasks_[i];

    vecTasks_.clear();
}

bool BFRootTaskData::Has (BFProjectSettings* pPrjSet)
{
    if (pPrjSet == &projectSettings_)
        return true;

    return false;
}

BFTask* BFRootTaskData::GetTask(BFoid oid)
{
    for (int i = 0; i < TaskVector().size(); ++i)
        if (TaskVector()[i]->GetOID() == oid)
            return TaskVector()[i];

    return NULL;
}

bool BFRootTaskData::HasTask(BFoid oid)
{
    return (GetTask(oid) != NULL);
}

bool BFRootTaskData::IsModified ()
{
    return bModified_;
}

void BFRootTaskData::SetModified (bool bModified /*= true*/)
{
    bModified_ = bModified;
}

bool BFRootTask::StoreToFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in BFRootTask::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileOutputStream  out(strFilename);
    jbArchive           archive(out, BF_PROJECT_CURRENT_VERSION);

    if (Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        return true;
    }

    return false;
}


bool BFRootTask::ReadFromFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        BFSystem::Fatal(_("wrong parameters"), _T("BFRootTask::ReadFromFile()"));
        return false;
    }

    wxFileInputStream   in(strFilename);
    jbArchive           archive(in, BF_PROJECT_CURRENT_VERSION);

    if (Serialize(archive))
    {
        strCurrentFilename_ = strFilename;
        return true;
    }

    return false;
}


bool BFRootTaskData::Serialize (jbArchive& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    // init
    bool    rc          = true;
    int     iObjCount   = GetTaskCount();

    rA.EnterObject();
    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        rA << strName_;
        projectSettings_.Serialize(rA);
        rA << iObjCount;

        for (int i = 0; i < iObjCount; ++i)
        {
            rc = TaskVector()[i]->Serialize(rA);

            if (!rc)
                break;
        }
    }
    else
    // ** serialize FROM file **
    {
        rA >> strName_;
        projectSettings_.Serialize(rA);
        rA >> iObjCount;

        // ** DEBUG **
        for (int i = 0; i < iObjCount; ++i)
        {
            // create a task with an invalid oid
            BFTask* pTask = new BFTask();

            rc = pTask->Serialize(rA);

            if (rc && pTask != NULL)
                TaskVector().push_back(pTask);
            else
                break;
        }

        broadcastObservers();
    }
    rA.LeaveObject();

    SetModified(false);

    return rc;
}


long BFRootTaskData::GetTaskCount ()
{
    return vecTasks_.size();
}

void BFRootTaskData::SetName (const wxChar* strName)
{
    if (strName == NULL)
        return;

    strName_ = strName;

    SetModified();
    //broadcastObserver();
}


const wxChar* BFRootTaskData::GetName ()
{
    return strName_;
}


BFProjectSettings& BFRootTaskData::GetSettings ()
{
    return projectSettings_;
}

BFoid BFRootTaskData::AppendTask (BFTask& rTask)
{
    if ( !(rTask.IsPlausible()) )
        return BFInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return BFInvalidOID;

    // remember the task
    vecTasks_.push_back(&rTask);

    SetModified();
}

BFoid BFRootTaskData::AppendTask (BFTaskType type,
                             const wxChar* strSource,
                             const wxChar* strDestination,
                             const wxChar* strName,
                             bool bVerify,
                             wxArrayString* pArrExclude /*= NULL*/,
                             BFArchiveFormat archive /*= CompressNOTUSED*/)
{
    // check parameters
    if ( strSource == NULL
      || strDestination == NULL
      || strName == NULL
      || (type == TaskARCHIVE && archive == CompressNOTUSED))
        return BFInvalidOID;

    // check exclude vector
    wxArrayString arrExclude;
    if (pArrExclude != NULL)
        arrExclude = *pArrExclude;

    // create new task
    BFTask* pTask = new BFTask
                    (
                        type,
                        strSource,
                        strDestination,
                        strName,
                        bVerify,
                        archive,
                        arrExclude
                    );

    // remember task
    vecTasks_.push_back(pTask);

    SetModified();
    //broadcastObserver();

    return pTask->GetOID();
}



BFRootTask BFRootTask::sRootTask_;

BFRootTask& BFRootTask::Instance ()
{
    return sRootTask_;
}

BFRootTask::BFRootTask()
          : oidLast_(BFInvalidOID),
            bStopProject_(false),
            bStopTask_(false),
            pRunningTask_(NULL)
{
}

/*virtual*/ BFRootTask::~BFRootTask()
{
}

BFoid BFRootTask::CreateOID ()
{
    ++oidLast_;

    if (HasTask(oidLast_) || oidLast_ == BFInvalidOID)
        return CreateOID();

    return oidLast_;
}

BFCore& BFRootTask::Core ()
{
    return BFCore::Instance();
}

void BFRootTask::Close ()
{
    ClearTaskVector();
    SetName(BFROOTTASK_DEFAULT_NAME);
    GetSettings().SetDefault();
    SetModified(false);
    strCurrentFilename_ = wxEmptyString;
    oidLast_ = BFInvalidOID;
    bStopProject_ = false;
    bStopTask_ = false;
    pRunningTask_ = NULL;
    broadcastObservers();
}

bool BFRootTask::Run (wxWindow* pParent)
{
    // init
    int                     i;
    wxString                str;
    BFBackupLog             log;
    BFTaskProgressDlg       dlg(pParent, *this);

    // mark the backup start in the logfile
    log.BackupStarted();
    // the core need to create backup messages
    BFCore::Instance().BackupStarted();

    // prepare for destination directories
    for (i = 0; i < TaskVector().size(); ++i)
    {
        // create directory if needed
        str = TaskVector()[i]->GetDestination();

        if ( !(wxDir::Exists(ReplaceMacros(str))) )
            Core().CreatePath(str);
    }

    if (!bStopProject_)
    {
        // run each task
        for (i = 0; i < TaskVector().size() && !bStopProject_; ++i)
        {
            pRunningTask_ = TaskVector()[i];
            log.TaskStarted(*pRunningTask_);
            dlg.SetCurrentTaskName(pRunningTask_->GetName());
            pRunningTask_->Run( *(dlg.GetProgressTask()) );
            dlg.GetProgressTotal()->IncrementActual();
            log.TaskFinished();
        }
    }

    // mark the backup end in the log files
    log.BackupFinished();
    // there is no need to create backup messages
    BFCore::Instance().BackupEnded();

    return true;
}

void BFRootTask::InitThat (wxListBox& rListBox)
{
    for (int i = 0; i < GetTaskCount(); ++i)
        rListBox.Append(TaskVector()[i]->GetName(), TaskVector()[i]);
}


void BFRootTask::InitThat (BFBackupTree& rBackupTree)
{
    // add root
    rBackupTree.AddRoot(GetName(), BFIconTable::logo);

    // iterate throug the tasks
    for (int i = 0; i < TaskVector().size(); ++i)
    {
        // create all for the task needed items in the tree
        rBackupTree.AddTask
                    (
                        TaskVector()[i]->GetOID(),
                        TaskVector()[i]->GetType(),
                        TaskVector()[i]->GetName(),
                        TaskVector()[i]->GetDestination()
                    );
    }
}

wxArrayString BFRootTask::GetDestinations ()
{
    wxArrayString arr;

    // iterate throug the tasks
    for (int i = 0; i < TaskVector().size(); ++i)
        arr.Add(TaskVector()[i]->GetDestination());

    return arr;
}

void BFRootTask::StopCurrentTask ()
{
    BFSystem::Backup(_("try to stop the current running task"));

    bStopTask_ = true;
    bStopProject_ = false;

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

void BFRootTask::StopProject ()
{
    BFSystem::Backup(_("try to stop the current running project"));

    bStopTask_ = true;
    bStopProject_ = true;

    if (pRunningTask_ != NULL)
        pRunningTask_->StopTask();
}

const wxString& BFRootTask::GetCurrentFilename ()
{
    return strCurrentFilename_;
}

void BFRootTask::ClearLastLogFiles ()
{
    arrLastLogFiles_.Clear();
}

void BFRootTask::SetProjectLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Insert(strFilename, 0);
}

void BFRootTask::AddTaskLogFile (wxString& strFilename)
{
    arrLastLogFiles_.Add(strFilename);
}

const wxArrayString& BFRootTask::GetLastLogFiles ()
{
    return arrLastLogFiles_;
}
