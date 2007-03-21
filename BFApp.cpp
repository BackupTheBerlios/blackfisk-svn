/**
 * implementation of class BFApp
 * 2006-04-05
 ***/

#include "BFApp.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/sound.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "blackfisk.h"
#include "BFMainFrame.h"
#include "BFTaskProgressDlg.h"
#include "BFTaskDlg.h"
#include "BFRootTask.h"
#include "Progress.h"


IMPLEMENT_APP(BFApp);


BFMainFrame* BFApp::spMainFrame_ = NULL;

/*static*/ BFMainFrame* BFApp::MainFrame ()
{
    return spMainFrame_;
}

/*static*/ void BFApp::SetMainFrame (BFMainFrame* pMainFrame)
{
    spMainFrame_ = pMainFrame;
}

BFApp::BFApp ()
{
}

/*virtual*/ BFApp::~BFApp ()
{
    BFSystem::Log(wxString::Format(_("%s closed"), BF_PRGNAME).c_str());
}

bool BFApp::OnInit()
{
    BFSystem::Log(wxString::Format(_("%s %s by %s started"), BF_PRGNAME, BF_VERSION, BF_AUTHOR).c_str());

    // available languages
    // TODO just look at the locals directory to know what is available
    static const wxLanguage langIds[] =
    {
        wxLANGUAGE_DEFAULT,
        wxLANGUAGE_FRENCH,
        wxLANGUAGE_GERMAN,
        wxLANGUAGE_ENGLISH
    };

    locale_.AddCatalogLookupPathPrefix(_T(".\\locale"));
    locale_.Init( langIds[2] );
    locale_.AddCatalog(_T("ob"));

    /* init the main frame
       'BFApp::spMainFrame_' is set by the ctor of BFMainFrame itself */
    new BFMainFrame(*this);

    // DEBUG
    Backup();

    return TRUE;
}

bool BFApp::OpenProject (const wxChar* filename)
{
    return BFRootTask::Instance().ReadFromFile(filename);
}

bool BFApp::SaveProject (const wxChar* filename)
{
    return BFRootTask::Instance().StoreToFile(filename);
}

bool BFApp::IsProjectModified ()
{
    return BFRootTask::Instance().IsModified();
}

void BFApp::Test ()
{
    /*
    wxFFileOutputStream out(_T("C:\\x.zip"));
    wxZipOutputStream   zip(out);
    wxFFileInputStream* pIn             = NULL;
    wxZipEntry*         pEntry          = NULL;

    // create zip-entry
    pEntry = new wxZipEntry(_T("autoexec.bat"));
    pIn = new wxFFileInputStream(_T("C:\\autoexec.bat"), _T("rb"));
    zip.PutNextEntry(pEntry);
    zip.Write(*pIn);
    delete pIn;

    // create empty zip-dir
    wxFileName fn(_T("C:\\windows"));
    zip.PutNextDirEntry(_T("ordner"), fn.GetModificationTime());

    // create zip-entry
    pEntry = new wxZipEntry(_T("scandisk.log"));
    pIn = new wxFFileInputStream(_T("C:\\scandisk.log"), _T("rb"));
    zip.PutNextEntry(pEntry);
    zip.Write(*pIn);
    delete pIn;*/
}


void BFApp::Backup()
{
    return;

    // init
    BFRootTask& rRoot = BFRootTask::Instance();
    wxArrayString arrExclude;

    rRoot.SetName(_T("NAME ME"));

    /** prepare for BACKUP **
    rRoot.AppendTask(TaskDIRCOPY, _T("D:\\_new"), _T("X:\\<date>_MUSIC"), _T("_new"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("D:\\_torename"), _T("X:\\<date>_MUSIC"), _T("_torename"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("D:\\CD_MP3"), _T("X:\\<date>_MUSIC"), _T("CD_MP3"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("D:\\mp3"), _T("X:\\<date>_MUSIC"), _T("mp3"), true);

    rRoot.AppendTask(TaskDIRCOPY, _T("E:\\Data"), _T("F:\\<date>_PRG"), _T("Data"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("E:\\Dokumente"), _T("F:\\<date>_PRG"), _T("Dokumente"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("E:\\TD"), _T("F:\\<date>_PRG"), _T("TD"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("E:\\Garage"), _T("F:\\<date>_PRG"), _T("Garage"), true);

    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\40tude Dialog"),    _T("F:\\<date>_PRG"), _T("40tude Dialog"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\40tude Dialog II"), _T("F:\\<date>_PRG"), _T("40tude Dialog II"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\MIRANDA IM"),       _T("F:\\<date>_PRG"), _T("MIRANDA IM"), true);

    arrExclude.Clear();
    arrExclude.Add(_T("C:\\Programme\\Opera\\profile\\cache4"));
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\Opera\\profile"), _T("F:\\<date>_PRG"), _T("OperaProfile_<date>"), true, &arrExclude);

    rRoot.AppendTask(TaskFILECOPY,
                        _T("C:\\Programme\\Microsoft Visual Studio\\Common\\MSDev98\\Bin\\autoexp.dat"),
                        _T("F:\\<date>_PRG\\MSVC"),
                        _T("autoexp.dat"),
                        true);

    rRoot.AppendTask(TaskZIP, _T("C:\\codeblocks-head"), _T("F:\\<date>_PRG"), _T("codeblocks-head"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\CodeBlocks"), _T("F:\\<date>_PRG\\"), _T("CodeBlocks"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\OpenOffice.org 2.0"), _T("F:\\<date>_PRG\\OpenOffice"), _T("OpenOffice.org 2.0"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\WINDOWS\\Anwendungsdaten\\OpenOffice.org2"), _T("F:\\<date>_PRG\\OpenOffice"), _T("Anwendungsdaten"), true);
    rRoot.AppendTask(TaskFILECOPY, _T("F:\\feedreader.opml"), _T("F:\\<date>_PRG\\FeedReader30"), _T("feedreader.opml"), true);
    rRoot.AppendTask(TaskFILECOPY, _T("C:\\Programme\\FeedReader30\\feedreader.ini"), _T("F:\\<date>_PRG\\FeedReader30"), _T("feedreader.ini"), true);
    rRoot.AppendTask(TaskFILECOPY, _T("C:\\Programme\\FeedReader30\\data\\RSSENGINE.FDB"), _T("F:\\<date>_PRG\\FeedReader30\\data"), _T("RSSENGINE.FDB"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("F:\\TheBat"), _T("F:\\<date>_PRG"), _T("The Bat"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\Square Soft, Inc\\Final Fantasy VII\\save"), _T("F:\\<date>_PRG\\GameSave"), _T("FFVII"), true);
    rRoot.AppendTask(TaskZIP, _T("C:\\Programme\\THQ\\Baphomets Fluch - Der schlafende Drache\\saves"), _T("F:\\<date>_PRG\\GameSave"), _T("BF3"), true);
    rRoot.AppendTask(TaskFILECOPY, _T("C:\\Programme\\WS_FTP\\WS_FTP.ini"), _T("F:\\<date>_PRG\\WS_FTP"), _T("WS_FTP.ini"), true);

    rRoot.AppendTask(TaskDIRCOPY, _T("X:\\Applications"), _T("F:\\<date>_Archiv"), _T("Applications"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("X:\\Graphic"), _T("F:\\<date>_Archiv"), _T("Graphic"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("X:\\Sound"), _T("F:\\<date>_Archiv"), _T("Sound"), true);
    rRoot.AppendTask(TaskDIRCOPY, _T("X:\\Stuff"), _T("F:\\<date>_Archiv"), _T("Stuff"), true);

    rRoot.AppendTask(TaskDIRCOPY, _T("F:\\<date>_PRG"), _T("X:\\_BACKUP"), _T("<date>_PRG"), true);

    rRoot.AppendTask(TaskARCHIVE, _T("C:\\wxWindows"), _T("F:\\<date>_PRG"), _T("wxWindows"), true);


    rRoot.AppendTask(TaskARCHIVE,    _T("C:\\OBtest\\tozip"),        _T("F:\\OBtest\\<date>"),   _T("tozip"),        true, NULL, CompressZIP);
    rRoot.AppendTask(TaskDIRCOPY,    _T("C:\\OBtest\\tocopy"),       _T("F:\\OBtest\\<date>"),   _T("tocopy"),       true);
*/
    rRoot.AppendTask(TaskFILECOPY,   _T("C:\\OBtest\\CONTACT.odt"),  _T("F:\\OBtest\\<date>"),   _T("CONtACT.odt"),  true);
    rRoot.broadcastObserver();

    // ** RUN **
    //if (rootTask.Run(pMainFrame_))
        wxSound(_T("sound\\finish.wav")).Play();
/*    }
    else
    {
        MainFrame()->Message(MsgFATAL, _T("there was an ERROR while Backup"));
    }
*/
}
