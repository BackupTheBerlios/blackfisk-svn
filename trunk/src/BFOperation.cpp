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
    {
        case TaskFILECOPY:
            RunForFileCopy(rProgress);
            break;

        case TaskSYNC:
            RunForDirSync(rProgress);
            break;

		case TaskDIRCOPY:
            RunForDirCopy(rProgress);
            break;

		case TaskARCHIVE:
            RunForArchive(rProgress);
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

    // arrays
    wxArrayString arrSourceDirs;
    wxArrayString arrSourceFiles;
    wxArrayString arrDestinationDirs;
    wxArrayString arrDestinationFiles;

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
    rProgress.SetActual ( 0 );
    rProgress.SetLabel ( wxString::Format
    (
        _("synchronise from %s to %s"),
        strSource,
        strDestination
    ) );
    rProgress.SetMessage ( _("calculating directories and files...") );
	BFSystem::Backup ( _("calculating directories and files...") );

    // ** listing source **
    wxArrayString arrExclude = pTask_->GetExclude();
    BFCore::GetDirListing(pTask_->GetSource(), arrSource_, &arrExclude);

    // stop?
    if ( BFCore::IsStop() )
        return false;

    // check for deletable files/dirs (store it in 'arrDestination_') ...
    if ( pTask_->GetRealSync() )
    {
        // get destination listing
        BFCore::GetDirListing(strDestination, arrDestination_);

        // stop?
        if ( BFCore::IsStop() )
            return false;

        // cut destination and source string from the listings
        wxArrayString arrSourceTemp = arrSource_;
		wxString strDestinationCommunity    = strDestination;
		wxString strSourceCommunity         = strSource;
		BFApp::Cut(arrDestination_, strDestination);
		BFApp::Cut(arrSourceTemp, strSource);

        // remove in-source-existing-files from the destination listing
        BFApp::Remove (arrDestination_, arrSourceTemp);

        // add the destination string to the destination listing
        BFApp::PrependString(arrDestination_, strDestinationCommunity);
    }

    // progress ...
    rProgress.SetRange ( arrSource_.GetCount() + arrDestination_.GetCount() );
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

    // check for deletable files/dirs (stored in 'arrDestination_') ...
    if ( arrDestination_.GetCount() > 0 )
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

        // separate in files and dirs
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

            // progress
            rProgress.IncrementActualWithMessage(arrDestinationFiles[i]);

            // delete
            BFCore::DeleteFile(arrDestinationFiles[i]);
        }

        // delete destination directories that doen't exist in the source
        for (size_t i = arrDestinationDirs.GetCount(); i > 0; --i)
        {
            // stop?
            if ( BFCore::IsStop() )
                return false;

            // progress
            rProgress.IncrementActualWithMessage(arrDestinationDirs[i-1]);

            // delete
            BFCore::DeleteDir(arrDestinationDirs[i-1]);
        }
    }

    return true;
}


bool BFOperation::RunForDirCopy (ProgressWithMessage& rProgress)
{
    // stop?
    if ( BFCore::IsStop() )
        return false;

    // init;
    arrSource_      .Clear();
    arrDestination_ .Clear();

    // helper variables
    wxString strA;

    // arrays
    wxArrayString arrSourceDirs;
    wxArrayString arrSourceFiles;
    wxArrayString arrDestinationDirs;
    wxArrayString arrDestinationFiles;

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
	rProgress.SetActual ( 0 );
    rProgress.SetLabel ( wxString::Format
    (
        _("copy directory %s to %s"),
        strSource,
        strDestination
    ) );
    rProgress.SetMessage ( _("calculating directories and files...") );
	BFSystem::Backup ( _("calculating directories and files...") );

    // ** listing source **
    wxArrayString arrExclude = pTask_->GetExclude();
    BFCore::GetDirListing(pTask_->GetSource(), arrSource_, &arrExclude);

    // stop?
    if ( BFCore::IsStop() )
        return false;

    // progress ...    
    rProgress.SetRange ( arrSource_.GetCount() + arrDestination_.GetCount() );
    rProgress.SetLabel ( wxString::Format
    (
        _("copy directory %s to %s"),
        strSource,
        strDestination
    ) );

    // backup message ...
    BFSystem::Backup ( wxString::Format
    (
        _("copy directory %s to %s"),
        strDestination, strSource
    ) );

    // separate directories and files
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

        // create destination string
        strA = strDestination + arrSourceFiles[i].Right(arrSourceFiles[i].Length() - strSource.Length());

        // progress
        rProgress.IncrementActualWithMessage(strA);

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

    // stop?
    if ( BFCore::IsStop() )
        return false;

	return true;
}



bool BFOperation::RunForArchive (ProgressWithMessage& rProgress)
{
    // stop?
    if ( BFCore::IsStop() )
        return false;

    // init;
    arrSource_      .Clear();
    arrDestination_ .Clear();

    // source
    wxString strSource = pTask_->GetSource();

	   // destination
    wxString strDestination = pTask_->GetDestination();

    if ( !(strDestination.EndsWith(wxFILE_SEP_PATH)) )
        strDestination += wxFILE_SEP_PATH;

    strDestination = strDestination + pTask_->GetName() + "." + pTask_->GetArchiveExtension();

    // handle placeholders
    BFBackup::FillBlackfiskPlaceholders(strSource);
    BFBackup::FillBlackfiskPlaceholders(strDestination);

    // progress ...
	rProgress.SetActual ( 0 );
    rProgress.SetLabel ( wxString::Format
    (
        _("compress directory %s to %s"),
        strSource,
        strDestination
    ) );
    rProgress.SetMessage ( _("calculating directories and files") );
	BFSystem::Backup ( _("calculating directories and files...") );

    // ** listing source **
    wxArrayString arrExclude = pTask_->GetExclude();
    BFCore::GetDirListing(pTask_->GetSource(), arrSource_, &arrExclude);

    // stop?
    if ( BFCore::IsStop() )
        return false;

	// progress ...    
    rProgress.SetRange ( arrSource_.GetCount() + arrDestination_.GetCount() );
	if ( pTask_->Verify() )
		rProgress.SetRange( rProgress.GetRange() * 2 );
    rProgress.SetLabel ( wxString::Format
    (
        _("compress directory %s to %s"),
        strSource,
        strDestination
    ) );

    // backup message ...
    BFSystem::Backup ( wxString::Format
    (
        _("compress directory %s to %s"),
        strSource,
		strDestination
    ) );

	// archive format
    switch (pTask_->GetArchiveFormat())
    {
        case CompressZIP:
            BFCore::Instance().CreateArchive
            (
                strDestination,
                arrSource_,                
                &rProgress
            );
        break;

        default:
            BFSystem::Fatal("unknown archive type", "BFOperation::RunForArchive()");
            break;
    }

	if ( pTask_->Verify() )
	{
		BFCore::Instance().VerifyZip (strDestination, arrSource_, &rProgress);
	}

    return true;
}


bool BFOperation::RunForFileCopy(ProgressWithMessage& rProgress)
{
    // stop?
    if ( BFCore::IsStop() )
        return false;

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

	// progress ...
    rProgress.SetActual ( 0 );
    rProgress.SetRange ( 1 );
    rProgress.SetLabel ( wxString::Format
    (
        _("Copy file %s to %s"),
        strSource,
        strDestination
    ) );

    // copy file
    return BFCore::Instance().CopyFile
    (
        strSource,
        strDestination,
        true,
        pTask_->Verify(),
        pTask_->VerifyContent()
    );
}

void BFOperation::StopOperation ()
{
    bStopOperation_ = true;
}
