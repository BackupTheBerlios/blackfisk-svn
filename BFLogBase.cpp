/**
 * implementation of class BFLogBase
 * 2007-04-20
 ***/

#include "BFLogBase.h"
#include "blackfisk.h"
#include "BFCore.h"

BFLogBase::BFLogBase (const wxChar* strLogFileName, long lMaxSizeInBytes /*= 0*/ )
     : Observer(&(BFSystem::Instance())),
       fileLog_(strLogFileName, wxFile::write_append),
       strLogFileName_(strLogFileName),
       lMaxSize_(lMaxSizeInBytes)
{
    CareSize();
}

/*virtual*/ BFLogBase::~BFLogBase ()
{
}

long BFLogBase::GetMaxSize ()
{
    return lMaxSize_;
}

void BFLogBase::SetMaxSize (long lMaxSizeInBytes)
{
    lMaxSize_ = lMaxSizeInBytes;
}

void BFLogBase::CareSize ()
{
    // the size does not matter
    if (GetMaxSize() <= 0)
        return;

    if ( !(fileLog_.IsOpened()) )
        return;

    // check log-file size
    if (fileLog_.Length() > GetMaxSize())
    {
        // close the file
        fileLog_.Close();
        // backup the file
        BFCore::Instance().MoveFile(strLogFileName_, strLogFileName_ + BF_LOGFILE_BAKSUFFIX, true);
        // create a new empty file
        fileLog_.Open(strLogFileName_, wxFile::write_append);
    }
}
