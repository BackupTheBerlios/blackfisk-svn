/**
 * implementation of class BFTaskBase
 * 2006-05-29
 ***/

#include "BFTaskBase.h"
#include <wx/wx.h>

BFTaskBase::BFTaskBase ()
{
}


/*virtual*/ BFTaskBase::~BFTaskBase ()
{
}


wxString& BFTaskBase::ReplaceMacros (wxString& rStr)
{
    // BFTASK_MACRO_DATE
    rStr.Replace(BFTASK_MACRO_DATE, Core().GetDateString());

    return rStr;
}
