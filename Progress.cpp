/**
 * Progress.h
 ***/

#include <wx/wx.h>
#include <wx/wxchar.h>
#include "Progress.h"


//const long Progress::lBEGIN_ = 0;

Progress::Progress()
        : lBEGIN_(0),
          end_(100),
          actual_(0)
{
}


/*virtual*/ Progress::~Progress()
{
}


void Progress::SetRange (long end)
{
    end_ = end;

	//SetActual(lBEGIN_);
}


bool Progress::SetActual (long actual)
{
	if (actual > end_ || actual < lBEGIN_)
        return false;

	actual_ = actual;
    broadcastObservers();

	return true;
}


bool Progress::IsEnd ()
{
    if (GetActual() >= end_)
        return true;
    else
        return false;
}


void Progress::DoEnd ()
{
    if (IsEnd())
        return;

    SetActual (end_);
}


long Progress::GetActual (void)
{
	return actual_;
}


long Progress::GetRange ()
{
    return end_ - lBEGIN_;
}


unsigned short Progress::GetProgress ()
{
	double x;

	x = end_ - lBEGIN_;
	x = 100 / x;

	return ( (unsigned short) (x * actual_) );
}



ProgressWithMessage::ProgressWithMessage ()
{
}


/*virtual*/ ProgressWithMessage::~ProgressWithMessage ()
{
}


bool ProgressWithMessage::IncrementActualWithMessage (const wxChar* message)
{
    if (message != NULL)
        strMessage_ = message;

    return Progress::IncrementActual();
}


const wxChar* ProgressWithMessage::GetMessage ()
{
    return strMessage_;
}


const wxChar* ProgressWithMessage::GetLabel ()
{
    return strLabel_;
}


void ProgressWithMessage::SetLabel (const wxChar* label)
{
    if (label == NULL)
        return;

    strLabel_ = label;
}


ProgressTotal::ProgressTotal(long lProgressCount,
                             Progress* pSubProgress)
             : Observer(pSubProgress),
               lProgressCount_(lProgressCount),
               lProgressCurrent_(1)
{
}


/*virtual*/ ProgressTotal::~ProgressTotal()
{
}


/*virtual*/ bool ProgressTotal::IncrementActual ()
{
    ++lProgressCurrent_;

    if (lProgressCurrent_ > lProgressCount_)
        DoEnd();

    return true;
}


/*virtual*/ void ProgressTotal::ValueChanged (Subject* pSender)
{
    // init
    Progress* pP = dynamic_cast<Progress*>(pSender);

    if (pP == NULL)
        return;

    // calculate new range
    SetRange(pP->GetRange() * lProgressCount_);

    // set new position in that range
    SetActual
    (
        ( pP->GetRange() * (lProgressCurrent_-1) ) + pP->GetActual()
    );
}


