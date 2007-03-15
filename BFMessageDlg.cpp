/**
 * implementation of class BFMessageDlg
 * 2007-02-23
 ***/

#include "BFMessageDlg.h"
#include "BFSystem.h"
#include "BFMainFrame.h"

//
BFMessageDlg::BFMessageDlg (BFMainFrame* pParent)
            : wxMessageDialog(pParent, wxEmptyString),
              Observer(&(BFSystem::Instance()))
{
}


//
/*virtual*/ BFMessageDlg::~BFMessageDlg ()
{
}


void BFMessageDlg::SetMessage (const wxChar* msg)
{
    if (msg == NULL)
        m_message = wxEmptyString;
    else
        m_message = msg;
}

void BFMessageDlg::SetCaption (const wxChar* cap)
{
    if (cap == NULL)
        m_caption = wxEmptyString;
    else
        m_caption = cap;
}

void BFMessageDlg::SetStyle (long style)
{
    SetMessageDialogStyle(style);
}

/*virtual*/ void BFMessageDlg::ValueChanged (Subject* pSender)
{
    BFSystem* pSys = dynamic_cast<BFSystem*>(pSender);

    // check arguments
    if (pSys == NULL)
        return;

    // check message level
    if (pSys->GetLastType() > pSys->GetMsgLevel())
        return;

    // caption
    SetCaption (BFSystem::GetTypeString(pSys->GetLastType()));

    // icon
    SetStyle (wxOK | BFSystem::GetMsgStyle(pSys->GetLastType()));

    // message and location
    if (pSys->GetLastLocation().Len() == 0)
        SetMessage (pSys->GetLastMessage().c_str());
    else
        SetMessage ((pSys->GetLastMessage() + _T("\nLocation: ") + pSys->GetLastLocation()).c_str());

    /* if you need to check the return value,
       just use GetReturnCode() */
    ShowModal();
}

