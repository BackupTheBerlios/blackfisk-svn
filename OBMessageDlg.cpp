/**
 * implementation of class OBMessageDlg
 * 2007-02-23
 ***/

#include "OBMessageDlg.h"
#include "OBSystem.h"
#include "OBMainFrame.h"

//
OBMessageDlg::OBMessageDlg (OBMainFrame* pParent)
            : wxMessageDialog(pParent, wxEmptyString),
              Observer(&(OBSystem::Instance()))
{
}


//
/*virtual*/ OBMessageDlg::~OBMessageDlg ()
{
}


void OBMessageDlg::SetMessage (const wxChar* msg)
{
    if (msg == NULL)
        m_message = wxEmptyString;
    else
        m_message = msg;
}

void OBMessageDlg::SetCaption (const wxChar* cap)
{
    if (cap == NULL)
        m_caption = wxEmptyString;
    else
        m_caption = cap;
}

void OBMessageDlg::SetStyle (long style)
{
    SetMessageDialogStyle(style);
}

/*virtual*/ void OBMessageDlg::ValueChanged (Subject* pSender)
{
    OBSystem* pSys = dynamic_cast<OBSystem*>(pSender);

    // check arguments
    if (pSys == NULL)
        return;

    // check message level
    if (pSys->GetLastType() > pSys->GetMsgLevel())
        return;

    // caption
    SetCaption (OBSystem::GetTypeString(pSys->GetLastType()));

    // icon
    SetStyle (wxOK | OBSystem::GetMsgStyle(pSys->GetLastType()));

    // message and location
    if (pSys->GetLastLocation().Len() == 0)
        SetMessage (pSys->GetLastMessage());
    else
        SetMessage (pSys->GetLastMessage() + _T("\nLocation: ") + pSys->GetLastLocation());

    /* if you need to check the return value,
       just use GetReturnCode() */
    ShowModal();
}

