/**
 * implementation of class OBTaskBase
 * 2006-05-29
 ***/

#include "OBTaskBase.h"
#include <wx/wx.h>

OBTaskBase::OBTaskBase ()
{
}


/*virtual*/ OBTaskBase::~OBTaskBase ()
{
}


wxString& OBTaskBase::ReplaceMacros (wxString& rStr)
{
    // OBTASK_MACRO_DATE
    rStr.Replace(OBTASK_MACRO_DATE, Core().GetDateString());

    return rStr;
}
