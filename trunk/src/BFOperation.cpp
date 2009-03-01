/**
 * Name:        BFOperation.cpp
 * Purpose:     BFOperation class implementation
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2009-01-23
 * Copyright:   (c) 2009 Christian Buhtz <blackfisk@web.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/


#include "BFOperation.h"
#include "BFBackup.h"
#include "BFCore.h"
#include "BFProject.h"
#include "BFundef.h"
#include "BFSystem.h"
#include "BFApp.h"
#include "blackfisk.h"

BFOperation::BFOperation (BFTask* pTask)
           : pTask_(pTask),
             bStopOperation_(false)
{
}

/*virtual*/ BFOperation::~BFOperation ()
{
}


BFTask* BFOperation::Task ()
{
    return pTask_;
}

bool BFOperation::Run (ProgressWithMessage& rProgress)
{
    wxString str = pTask_->GetDestination();

    // create directory if needed
    if ( !(wxDir::Exists(BFBackup::FillBlackfiskPlaceholders(str))) )
        BFCore::Instance().CreatePath(str);

    switch (pTask_->GetType())
    {/*
        case TaskARCHIVE:
            RunForArchive(rProgress);
            break;

        case TaskDIRCOPY:
            RunForDirCopy(rProgress);
            break;

        case TaskFILECOPY:
            RunForFileCopy(rProgress);
            break;
*/
        case TaskSYNC:
            RunForDirSync(rProgress);
            break;

        default:
            break;
    };

    if ( BFCore::IsStop() )
        return false;

    return true;
}

bool BFOperation::RunForDirSync (ProgressWithMessage& rProgress)
{
    // stop?
    if ( BFCore::IsStop() )
        return false;

    // init;
    arrSource_      .Clear();
    arrDestination_ .Clear();

    // helper variables
    wxString strA;
    bool     bA;

    // source
    wxString strSource = pTask_->GetSource();

    // destination
    wxString strDestination = pTask_->GetDestination();

    if ( !(strDestination.EndsWith(wxFILE_SEP_PATH)) )
        strDestination += wxFILE_SEP_PATH;

    strDestination = strDestination + pTask_->GetName();

    // handle placeholders
    BFBackup::FillBlackfiskPlaceholders(strSource);
    BFBackup::FillBlackfiskPlaceholders(strDestination);

    // create destination if needed
    if ( !(wxDir::Exists(strDestination)) )
        BFCore::Instance().CreatePath(strDestination);

    // progress ...
    rProgress.SetLabel ( wxString::Format
    (
        _("synchronise from %s to %s\ncounting directories and files"),
        strSource,
        strDestination
    ) );

    // ** listing source **
    wxArrayString arrExclude = pTask_->GetExclude();
    BFCore::GetDirListing(pTask_->GetSource(), arrSource_, &arrExclude);

    // stop?
    if ( BFCore::IsStop() )
        return false;

    // progress ...
    rProgress.SetActual ( 0 );
    rProgress.SetRange ( arrSource_.GetCount() );
    rProgress.SetLabel ( wxString::Format
    (
        _("synchronise from %s to %s"),
        strSource,
        strDestination
    ) );

    // backup message ...
    BFSystem::Backup ( wxString::Format
    (
        _("synchronise %s with %s (copy-step)"),
        strDestination, strSource
    ) );

    // separate directories and files
    wxArrayString arrSourceDirs;
    wxArrayString arrSourceFiles;

    BFCore::SeparateListingInDirectoriesAndFiles
    (
        arrSource_,
        arrSourceDirs,
        arrSourceFiles
    );

    // ** each source dir **
    for (size_t i = 0; i < arrSourceDirs.GetCount(); i++)
    {
        // stop?
        if ( BFCore::IsStop() )
            return false;

        // create distination string
        strA = strDestination + arrSourceDirs[i].Right(arrSourceDirs[i].Length() - strSource.Length());

        // progress ...
        rProgress.IncrementActualWithMessage(strA);

        // check existence
        if ( !(wxDirExists(strA)) )
            BFCore::CreatePath(strA);

        // check some(!) attributes
        // see VerifyFileAttributes() for more details!
        if ( !(BFCore::VerifyFileAttributes(arrSourceDirs[i], strA)) )
            BFCore::CopyDirAttributes(arrSourceDirs[i], strA);
    }

    // ** each source file **
    for (size_t i = 0; i < arrSourceFiles.GetCount(); i++)
    {
        // stop?
        if ( BFCore::IsStop() )
            return false;

        bA = false; // don't copy

        // create destination string
        strA = strDestination + arrSourceFiles[i].Right(arrSourceFiles[i].Length() - strSource.Length());

        // progress
        rProgress.IncrementActualWithMessage(strA);

        // check existence
        if ( wxFileExists(strA) )
        {
            if ( !(BFCore::VerifyFile(arrSourceFiles[i], strA)) )
            {
                bA = true;  // do copy

                // remove write protection if needed
                if ( BFCore::HasFileAttribute_ReadOnly(strA) )
                    BFCore::SetFileAttribute_ReadOnly(strA, false);

                // remove hidden status if needed
                // because it is not possible to copy a visible file over a hidden file (tested on WinXP)
                if ( BFCore::HasFileAttribute_Hidden(strA) )
                    BFCore::SetFileAttribute_Hidden(strA, false);
            }
        }
        else
        {
            bA = true;  // do copy
        }

        // do the copy?
        if ( bA )
        {
            // copy complete file
            BFCore::CopyFile
            (
                arrSourceFiles[i],      // source
                strA,                   // destination
                true,                   // overwrite
                pTask_->Verify(),       // verify
                pTask_->VerifyContent() // verify content
            );
        }
    }

    // stop?
    if ( BFCore::IsStop() )
        return false;

    // check for deletable files ...
    if ( pTask_->GetRealSync() )
    {
        // progress ...
        rProgress.SetLabel ( _("check for deletable files and directories") );

        // backup message
        BFSystem::Backup ( wxString::Format
        (
            _("synchronise %s with %s (delete-step)"),
            strDestination,
            strSource
        ) );

        // get destination listing
        BFCore::GetDirListing(strDestination, arrDestination_);

        // stop?
        if ( BFCore::IsStop() )
            return false;

        // cut destination and source string from the listings
        wxString strDestinationCommunity    = BFApp::CutCommunity (arrDestination_);
        wxString strSourceCommunity         = BFApp::CutCommunity (arrSource_);

        // remove in-source-existing-files from the destination listing
        BFApp::Remove (arrDestination_, arrSource_);

        // add the destination string to the destination listing
        BFApp::PrependString(arrDestination_, strDestinationCommunity);

        // separate in files and dirs
        wxArrayString arrDestinationDirs;
        wxArrayString arrDestinationFiles;

        BFCore::SeparateListingInDirectoriesAndFiles
        (
            arrDestination_,
            arrDestinationDirs,
            arrDestinationFiles
        );

        // progress ...
        rProgress.SetLabel ( _("delete unexisting files and directories") );

        // delete destination files that doen't exist in the source
        for (size_t i = 0; i < arrDestinationFiles.GetCount(); ++i)
        {
            // stop?
            if ( BFCore::IsStop() )
                return false;

            BFCore::DeleteFile(arrDestinationFiles[i]);
        }

        // delete destination directories that doen't exist in the source
        for (size_t i = arrDestinationDirs.GetCount(); i > 0; --i)
        {
            // stop?
            if ( BFCore::IsStop() )
                return false;

            BFCore::DeleteDir(arrDestinationDirs[i-1]);
        }
    }

    return true;
/*
    return BFCore::Instance().Synchronise
    (
        strSrc,
        strDest,
        pTask_->Verify(),
        pTask_->VerifyContent(),
        pTask_->GetRealSync(),
        &rProgress
    );*/
}

void BFOperation::StopOperation ()
{
    bStopOperation_ = true;
}

/*
bool BFOperation::RunForArchive (ProgressWithMessage& rProgress)
{
    wxString strDest, strSrc;

    // source
    strSrc  = GetSource();

    // destination
    strDest = strDest + GetDestination();

    if ( !(strDest.EndsWith(wxFILE_SEP_PATH)) )
        strDest = strDest + wxFILE_SEP_PATH;

    strDest =  strDest + GetName() + "." + GetArchiveExtension();

    // handle placeholders
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // exclude something?
    wxArrayString* pArr = NULL;
    if (GetExclude().Count() > 0)
        pArr = new wxArrayString(GetExclude());

    switch (GetArchiveFormat())
    {
        case CompressZIP:
            BFCore::Instance().CreateZipFromDir
            (
                strDest.c_str(),
                strSrc.c_str(),
                pArr,
                Verify(),
                &rProgress
            );
        break;

        default:
            BFSystem::Fatal("unknown archive type", "BFOperation::RunForArchive()");
            break;
    }

    // delete temporary exclude array
    if (pArr != NULL)
        delete pArr;

    return true;
}


bool BFOperation::RunForDirCopy (ProgressWithMessage& rProgress)
{
    wxString strDest, strSrc;

    // source
    strSrc  = GetSource();

    // destination
    strDest = strDest + GetDestination();

    if ( !(strDest.EndsWith(wxFILE_SEP_PATH)) )
        strDest = strDest + wxFILE_SEP_PATH;

    strDest = strDest + GetName();

    // handle placeholders
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // copy dir
    return BFCore::Instance().CopyDir
    (
        strSrc.c_str(),
        strDest.c_str(),
        Verify(),
        VerifyContent(),
        &rProgress
    );
}


bool BFOperation::RunForFileCopy(ProgressWithMessage& rProgress)
{
    wxString strDest, strSrc;

    // source
    strSrc  = GetSource();

    // destination
    strDest = strDest + GetDestination();

    if ( !(strDest.EndsWith(wxFILE_SEP_PATH)) )
        strDest = strDest + wxFILE_SEP_PATH;

    strDest = strDest + GetName();

    // handle placeholders
    FillBlackfiskPlaceholders(strDest);
    FillBlackfiskPlaceholders(strSrc);

    // copy file
    return BFCore::Instance().CopyFile
    (
        strSrc.c_str(),
        strDest.c_str(),
        true,
        Verify(),
        VerifyContent()
    );
}*/
