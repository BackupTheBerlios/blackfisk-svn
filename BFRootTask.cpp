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

BFRootTaskData::BFRootTaskData ()
              : strName_(_("unnamed")),
                bModified_(false)
{
}


/*virtual*/BFRootTaskData::~BFRootTaskData ()
{
    for (int i = 0; i < vecTasks_.size(); ++i)
        delete vecTasks_[i];
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

bool BFRootTaskData::StoreToFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in BFRootTaskData::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileOutputStream  out(strFilename);
    jbArchive           archive(out);

    return Serialize(archive);
}


bool BFRootTaskData::ReadFromFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in BFRootTaskData::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileInputStream   in(strFilename);
    jbArchive           archive(in);

    return Serialize(archive);
}


bool BFRootTaskData::Serialize (jbArchive& rA)
{
    if ( !(rA.IsOpen()) )
        return false;

    // init
    bool    rc          = true;
    int     iObjCount   = GetTaskCount();

    if( rA.IsStoring() )
    // ** serialize TO file **
    {
        rA << strName_;
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

        broadcastObserver();
    }

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
          : pTotalProgress_(NULL),
            pTaskProgress_(NULL)
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


bool BFRootTask::Run (wxWindow* pParent)
{
    // init
    int                     i;
    wxString                str;
    BFTaskProgressDlg       dlg(pParent, *this);

    // log start
    str = wxString::Format(_("RootTask %s started"), GetName());
    BFSystem::Log(str);

    // prepare for destination directories
    for (i = 0; i < TaskVector().size(); ++i)
    {
        // create directory if needed
        str = TaskVector()[i]->GetDestination();

        if ( !(wxDir::Exists(ReplaceMacros(str))) )
            Core().CreatePath(str);
    }

    // run each task
    for (i = 0; i < TaskVector().size(); ++i)
    {
        dlg.SetCurrentTaskName(TaskVector()[i]->GetName());
        TaskVector()[i]->Run( *(dlg.GetProgressTask()) );
        dlg.GetProgressTotal()->IncrementActual();
    }

    // log finished
    str = wxString::Format(_("RootTask %s finished"), GetName());
    BFSystem::Log(str);

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
