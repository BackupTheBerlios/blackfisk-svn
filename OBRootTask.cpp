/**
 * implementation of class OBRootTask
 * 2006-05-29
 ***/

#include <wx/wfstream.h>
#include <wx/tokenzr.h>
#include "OBRootTask.h"
#include "OBTaskProgressDlg.h"
#include "OBBackupTree.h"
#include "Progress.h"

OBRootTaskData::OBRootTaskData ()
              : strName_(_("unnamed")),
                bModified_(false)
{
}


/*virtual*/OBRootTaskData::~OBRootTaskData ()
{
    for (int i = 0; i < vecTasks_.size(); ++i)
        delete vecTasks_[i];
}

OBTask* OBRootTaskData::GetTask(OBoid oid)
{
    for (int i = 0; i < TaskVector().size(); ++i)
        if (TaskVector()[i]->GetOID() == oid)
            return TaskVector()[i];

    return NULL;
}

bool OBRootTaskData::HasTask(OBoid oid)
{
    return (GetTask(oid) != NULL);
}

bool OBRootTaskData::IsModified ()
{
    return bModified_;
}

void OBRootTaskData::SetModified (bool bModified /*= true*/)
{
    bModified_ = bModified;
}

bool OBRootTaskData::StoreToFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in OBRootTaskData::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileOutputStream  out(strFilename);
    jbArchive           archive(out);

    return Serialize(archive);
}


bool OBRootTaskData::ReadFromFile (const wxChar* strFilename)
{
    if (strFilename == NULL)
    {
        wxLogError (_("wrong parameters in OBRootTaskData::StoreToFile (const wxChar*)"));
        return false;
    }

    wxFileInputStream   in(strFilename);
    jbArchive           archive(in);

    return Serialize(archive);
}


bool OBRootTaskData::Serialize (jbArchive& rA)
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
            OBTask* pTask = new OBTask();

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


long OBRootTaskData::GetTaskCount ()
{
    return vecTasks_.size();
}

void OBRootTaskData::SetName (const wxChar* strName)
{
    if (strName == NULL)
        return;

    strName_ = strName;

    SetModified();
    //broadcastObserver();
}


const wxChar* OBRootTaskData::GetName ()
{
    return strName_;
}


OBoid OBRootTaskData::AppendTask (OBTask& rTask)
{
    if ( !(rTask.IsPlausible()) )
        return OBInvalidOID;

    if ( HasTask(rTask.GetOID()) )
        return OBInvalidOID;

    // remember the task
    vecTasks_.push_back(&rTask);

    SetModified();
}

OBoid OBRootTaskData::AppendTask (OBTaskType type,
                             const wxChar* strSource,
                             const wxChar* strDestination,
                             const wxChar* strName,
                             bool bVerify,
                             wxArrayString* pArrExclude /*= NULL*/,
                             OBArchiveFormat archive /*= CompressNOTUSED*/)
{
    // check parameters
    if ( strSource == NULL
      || strDestination == NULL
      || strName == NULL
      || (type == TaskARCHIVE && archive == CompressNOTUSED))
        return OBInvalidOID;

    // check exclude vector
    wxArrayString arrExclude;
    if (pArrExclude != NULL)
        arrExclude = *pArrExclude;

    // create new task
    OBTask* pTask = new OBTask
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



OBRootTask OBRootTask::sRootTask_;

OBRootTask& OBRootTask::Instance ()
{
    return sRootTask_;
}

OBRootTask::OBRootTask()
          : pTotalProgress_(NULL),
            pTaskProgress_(NULL)
{
}

/*virtual*/ OBRootTask::~OBRootTask()
{
}

OBoid OBRootTask::CreateOID ()
{
    ++oidLast_;

    if (HasTask(oidLast_) || oidLast_ == OBInvalidOID)
        return CreateOID();

    return oidLast_;
}

OBCore& OBRootTask::Core ()
{
    return OBCore::Instance();
}


bool OBRootTask::Run (wxWindow* pParent)
{
    // init
    int                     i;
    wxString                str;
    OBTaskProgressDlg       dlg(pParent, *this);

    // log start
    str = wxString::Format(_("RootTask %s started"), GetName());
    Core().Log(LogINFO, str);

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
    Core().Log(LogINFO, str);

    return true;
}

void OBRootTask::InitThat (wxListBox& rListBox)
{
    for (int i = 0; i < GetTaskCount(); ++i)
        rListBox.Append(TaskVector()[i]->GetName(), TaskVector()[i]);
}


void OBRootTask::InitThat (OBBackupTree& rBackupTree)
{
    // init
    wxTreeItemId    idCurr;

    // add root
    wxTreeItemId    idRoot = rBackupTree.AddRoot(GetName(), OBICON_PROJECT);

    // iterate throug the tasks
    for (int i = 0; i < TaskVector().size(); ++i)
    {
        idCurr = rBackupTree.AddDestination(TaskVector()[i]->GetDestination());

        int iIconId;

        // add the task identifier itself
        switch (TaskVector()[i]->GetType())
        {
            case TaskARCHIVE:
                iIconId = OBICON_TASKZIP;
                break;

            case TaskDIRCOPY:
                iIconId = OBICON_TASKDC;
                break;

            case TaskFILECOPY:
                iIconId = OBICON_TASKFC;
                break;
        };  // switch(GetType)

        rBackupTree.AppendItem
                    (
                        idCurr,
                        TaskVector()[i]->GetName(),
                        iIconId,
                        -1,
                        new OBBackupTreeItemData ( TaskVector()[i]->GetOID() )
                    );
    }   // for (tasks)
}
