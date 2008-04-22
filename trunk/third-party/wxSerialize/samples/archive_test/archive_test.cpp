//---------------------------------------------------------------------------
// $RCSfile: archive_test.cpp $
// $Source: samples/archive_test/archive_test.cpp $
// $Revision: 1.13 $
// $Date: 31-jul-2007 11:48:37 $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <iostream>

#include <wx/wfstream.h>
#include <wx/app.h>
#include <wx/zstream.h>

#include "wxSerialize.h"

#define FILENAME "ctest.fil"
#define FILENAME_ZLIB "ctest.zlib"

//---------------------------------------------------------------------------

#ifdef __BORLANDC__
	#pragma argsused
#endif

// shortcut macro for messages
#define OKFAILMSG(message, expression) failokstr(message, expression)

// define this constant to use wxStringStream loopback checking. Something is wrong
// with the implementation of wxStringStream, and needs to be checked.
//#define USE_STRINGSTREAM

void failokstr(const wxString &message, bool expression)
{
    wxString outline;

    // add summarizing
    outline << wxT("* ") << message;
	wxString errmsg;

	if(expression)
		errmsg = wxT(" ...    [OK]");
	else
		errmsg = wxT(" ... [FAIL!]");

	int topad = 60 - outline.Len() - errmsg.Len();

	if(topad > 0)
		outline.Pad(topad);
	else
		outline.Remove(60 - errmsg.Len());

	std::cout << outline.mb_str(wxConvUTF8) << errmsg.mb_str(wxConvUTF8) << "\n";
}

char *get_debugchar(char inc)
{
    static char buf[20];

    if(inc > 0x1f)
    {
        buf[0] = inc;
        buf[1] = '\0';
    }
    else
        sprintf(buf, "[%02x]", inc);

    return &buf[0];
}

int main(int argc, char* argv[])
{
	bool abool;
	wxFloat64 adouble;
	wxUint8 abyte;
	wxUint16 ashort;
	wxUint32 along;
	wxUint64 aquad;
	wxString astr;
	int negint;


	// check block for wxArchive outa
	std::cout << " >> WRITING TO A FILE \n";
	{
		// first create an output stream (should be in current dir
		wxFileOutputStream outfile(wxT(FILENAME));

		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), outfile.Ok());
		if(!outfile.Ok())
			return 0;

		// hook up our archive
		wxSerialize outa(outfile, 1000, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), outa.IsOk());

		if(outa.IsOk())
		{
			OKFAILMSG(wxT("Writing an Uint8"), outa.WriteUint8(0xef));
			OKFAILMSG(wxT("Writing an Uint16"), outa.WriteUint16(0x4321));
			OKFAILMSG(wxT("Writing an Uint32"), outa.WriteUint32(0x4DD43D40));
			OKFAILMSG(wxT("Writing an Uint64"), outa.WriteUint64(0x123456789abcdef0ULL));
			OKFAILMSG(wxT("Writing a string"), outa.WriteString(wxT("This is a string")));
			OKFAILMSG(wxT("Writing a bool"), outa.WriteBool(true));
			OKFAILMSG(wxT("Writing a wxFloat64"), outa.WriteDouble(123.678));

			wxArrayString arr;
			arr.Add(wxT("One"));
			arr.Add(wxT("Three"));
			arr.Add(wxT("Four"));
			arr.Add(wxT("Five"));
			OKFAILMSG(wxT("Writing an array string"), outa.WriteArrayString(arr));


			OKFAILMSG(wxT("Writing a negative int"), outa.WriteInt(-1));
			OKFAILMSG(wxT("Writing a negative int (max8)"), outa.WriteInt(-127));
			if(sizeof(int) >= 2)
				OKFAILMSG(wxT("Writing a negative int (max16)"), outa.WriteInt(-32767));

			OKFAILMSG(wxT("IsStoring() should be true"), outa.IsStoring());
			OKFAILMSG(wxT("Eof() should be false"), !outa.Eof());
			OKFAILMSG(wxT("GetHeaderName() should be header read back"), outa.GetHeaderString().IsSameAs(wxT("SIMPLEHEADER")));
			OKFAILMSG(wxT("GetVersion() should be 1000"), outa.GetVersion() == 1000);
			OKFAILMSG(wxT("IsFullSerialize() should be true"), outa.IsFullSerialize() == true);
			OKFAILMSG(wxT("IsLoading() should be false"), outa.IsLoading() == false);

			if(sizeof(int) >= 4)
				OKFAILMSG(wxT("Writing a negative int (max32)"), outa.WriteInt(-2147483647));

            wxMemoryBuffer buf;
            buf.AppendByte('h');
            buf.AppendByte('e');
            buf.AppendByte('l');
            buf.AppendByte('l');
            buf.AppendByte('o');
            OKFAILMSG(wxT("Writing a record"), outa.Write(buf));
		}
	}

	// now create a wxSerialize with reading purposes first we go too high on number
	// then wrong header, then right header

	std::cout << " >> TEST WRONG VERSION \n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));

		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong version!)
		wxSerialize ina(infile, 800, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be NOT OK"), !ina.IsOk());
	}

	// test wrong header
	std::cout << " >> TEST WRONG HEADER \n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));

		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize ina(infile, 1000, wxT("WRONGHEADER"));

		OKFAILMSG(wxT("Our archive should be NOT OK"), !ina.IsOk());
	}

	// test reading back of written data
	printf(" >> READING BACK \n");
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));

		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize ina(infile, 2000, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), ina.IsOk());
		OKFAILMSG(wxT("Our archive should have version 1000"), ina.GetVersion() == 1000);

		OKFAILMSG(wxT("Reading an Uint8"), ina.ReadUint8(abyte, 0));
		OKFAILMSG(wxT("Reading an Uint16"), ina.ReadUint16(ashort, 0));
		OKFAILMSG(wxT("Reading an Uint32"), ina.ReadUint32(along, 0));
		OKFAILMSG(wxT("Reading an Uint64"), ina.ReadUint64(aquad, 0));

		OKFAILMSG(wxT("Reading a string"), ina.ReadString(astr));
		OKFAILMSG(wxT("Reading a bool"), ina.ReadBool(abool, false));
		OKFAILMSG(wxT("Reading a wxFloat64"), ina.ReadDouble(adouble, 0));

		wxArrayString arr;
		OKFAILMSG(wxT("Reading an array string"), ina.ReadArrayString(arr));

		OKFAILMSG(wxT("Array string must have 4 elements"), arr.Count() == 4);
		if(arr.Count() == 4)
		{
			OKFAILMSG(wxT("Checking Array string element 1"), arr[0].IsSameAs(wxT("One")));
			OKFAILMSG(wxT("Checking Array string element 2"), arr[1].IsSameAs(wxT("Three")));
			OKFAILMSG(wxT("Checking Array string element 3"), arr[2].IsSameAs(wxT("Four")));
			OKFAILMSG(wxT("Checking Array string element 4"), arr[3].IsSameAs(wxT("Five")));
		}

		OKFAILMSG(wxT("Checking Uint8 value"), abyte  == 0xef);
		OKFAILMSG(wxT("Checking Uint16 value"), ashort == 0x4321);
		OKFAILMSG(wxT("Checking Uint32 value"), along  == 0x4DD43D40);
		OKFAILMSG(wxT("Checking Uint64 value"), aquad  == 0x123456789abcdef0ULL);
		OKFAILMSG(wxT("Checking string value"), astr.IsSameAs(wxT("This is a string")));
		OKFAILMSG(wxT("Checking bool value"), abool);
		OKFAILMSG(wxT("Checking wxFloat64 value"), adouble == 123.678);

		OKFAILMSG(wxT("Reading an negative int (-1)"), ina.ReadInt(negint, 0));
		OKFAILMSG(wxT("Checking negative value"), negint == -1);

		OKFAILMSG(wxT("Reading an negative int (max8)"), ina.ReadInt(negint, 0));
		OKFAILMSG(wxT("Checking negative value"), negint == -127);

		OKFAILMSG(wxT("IsStoring() should be false"), !ina.IsStoring());
		OKFAILMSG(wxT("Eof() should be false"), !ina.Eof());
		OKFAILMSG(wxT("GetHeaderName() should be header read back"), ina.GetHeaderString().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("GetVersion() should be 1000"), ina.GetVersion() == 1000);
		OKFAILMSG(wxT("IsFullSerialize() should be true"), ina.IsFullSerialize() == true);
		OKFAILMSG(wxT("IsLoading() should be true"), ina.IsLoading());

		if(sizeof(int) >= 2)
		{
			OKFAILMSG(wxT("Reading an negative int (max16)"), ina.ReadInt(negint, 0));
			OKFAILMSG(wxT("Checking negative value"), negint == -32767);
		}

		if(sizeof(int) >= 4)
		{
			OKFAILMSG(wxT("Reading an negative int (max32)"), ina.ReadInt(negint, 0));
			OKFAILMSG(wxT("Checking negative value"), negint == -2147483647);
		}

        wxMemoryBuffer buf;
        OKFAILMSG(wxT("Reading a record"), ina.Read(buf));
        OKFAILMSG(wxT("Should be 5 bytes"), buf.GetDataLen() == 5);

        if(buf.GetDataLen() == 5)
        {
            char *ptr = (char *) buf.GetData();
            OKFAILMSG(wxT("Verifying data"), ptr[0] == 'h' && ptr[1] == 'e' && ptr[2] == 'l' &&
                                             ptr[3] == 'l' && ptr[4] == 'o');
        }

		// archive status should report neutral stuff
		OKFAILMSG(wxT("ArchiveStatus reports proper header"), ina.GetArchiveStatus().GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), ina.GetArchiveStatus().GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports no data loss"), !ina.GetArchiveStatus().NewDataLoss());
	}

#ifdef USE_STRINGSTREAM
	wxString outstring;
	std::cout << " >> USING wxStringOutputStream \n";
	{
		// first create an output stream (should be in current dir
		wxStringOutputStream outfile(&outstring);
		wxSerialize outa(outfile, 1000, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), outa.IsOk());

		if(outa.IsOk())
		{
			outa << abyte << ashort << along << aquad << astr << abool;

			OKFAILMSG(wxT("Writing some vars"), outa.IsOk());

			outa << (int)1 << (wxUint32)12345678 << true;

			OKFAILMSG(wxT("Writing some constants"), outa.IsOk());

			std::cout << "Size of string stream is " << outstring.Len() << " bytes!\n";
		}
	}

	std::cout << " >> READING FROM wxStringInputStream \n";
	{
		for(int i = 0; i < outstring.Len(); i++)
            std::cout << get_debugchar(outstring.GetChar(i)) << ", ";

		std::cout << "\n";

		wxStringInputStream infile(outstring);

		OKFAILMSG(wxT("StringStream OK?"), infile.IsOk());
		wxSerialize outa(infile, 1000, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), outa.IsOk());

		//if(outa.IsOk())
		//{
			bool abool2;
			wxUint8 abyte2 = 0;
			wxUint16 ashort2 = 0;
			wxUint32 along2 = 0;
			wxUint64 aquad2 = 0;
			wxString astr2;
			int aint = -1;

			outa >> abyte2 >> ashort2 >> along2 >> aquad2 >> astr2 >> abool2;

			OKFAILMSG(wxT("Reading back some vars"), outa.IsOk());

			OKFAILMSG(wxT("Checking a byte"), abyte2 == abyte);
			OKFAILMSG(wxT("Checking a short"), ashort2 == ashort);
			OKFAILMSG(wxT("Checking a long"), along2 == along);

			OKFAILMSG(wxT("Checking a quad"), aquad2 == aquad);
			OKFAILMSG(wxT("Checking a string"), astr2 == astr);
			OKFAILMSG(wxT("Reading a bool"), abool2 == abool);

			abool2 = false;
			along2 = 0;
			aint = 123;

			outa >> aint >> along2 >> abool2;

			OKFAILMSG(wxT("Checking a constant int"), aint == 1);
			OKFAILMSG(wxT("Checking a constant long"), along2 == 12345678);
			OKFAILMSG(wxT("Checking a constant bool"), abool2 == true);

			OKFAILMSG(wxT("Checking OK at end of stream"), outa.IsOk());
		//}
	}
#endif
	
	std::cout << " >> WRITE to wxZLibOutputStream -> wxFileOutputStream \n";
	{
		// first create an output stream (should be in current dir
		wxFileOutputStream outfile(wxT(FILENAME_ZLIB));

		OKFAILMSG(wxT("Opening the file '" FILENAME_ZLIB "'"), outfile.Ok());
		if(!outfile.Ok())
			return 0;

		// point a wxZLIBOutputStream to the file
		wxZlibOutputStream zstr(outfile);

		// hook up our archive to the ZLIB
		wxSerialize outa(zstr, 1000, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), outa.IsOk());

		if(outa.IsOk())
		{
			outa << abyte << ashort << along << aquad << astr << abool;

			OKFAILMSG(wxT("Writing some vars"), outa.IsOk());

			// krank up the stream size
			for(size_t i = 0; i < 1000; i++)
				outa << (wxUint32)0;

			outa << wxT("This is fun! Our stream is hardly readable now!");

			// resolve bug with ZLIB stream not reporting OK at end, so write one more entry but do not read back
			outa << (int)0;
		}
	}

	std::cout << " >> READ from wxFileInputStream -> wxZLibInputStream ->  \n";
	{
		wxString str;

		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME_ZLIB));

		OKFAILMSG(wxT("Opening the file '" FILENAME_ZLIB "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// point a wxZLIBOutputStream to the file
		wxZlibInputStream zstr(infile);

		// hook up our archive to the ZLIB
		wxSerialize ina(zstr, 1010, wxT("SIMPLEHEADER"));

		OKFAILMSG(wxT("Our archive should be OK"), ina.IsOk());

		if(ina.IsOk())
		{
			ina >> abyte >> ashort >> along >> aquad >> astr >> abool;

			OKFAILMSG(wxT("Reading some vars"), ina.IsOk());

			// krank up the stream size
			for(size_t i = 0; i < 1000; i++)
				ina >> along;

			OKFAILMSG(wxT("Read a block of 1000 0's"), ina.IsOk());

			ina >> str;
			//ina.ReadString(str);

			std::cout << "Last string read: " << str.mb_str(wxConvUTF8) << "\n";

			OKFAILMSG(wxT("All done!"), ina.IsOk());
		}
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT \n";
	{
		// first create an output stream (should be in current dir
		wxFileOutputStream outfile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), outfile.Ok());
		if(!outfile.Ok())
			return 0;

		// hook up our archive
		wxSerialize a(outfile, 1000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		if(a.IsOk())
		{
			a.EnterObject();
			OKFAILMSG(wxT("Writing an Uint8"), a.WriteUint8(0xef));
			OKFAILMSG(wxT("Writing an Uint16"), a.WriteUint16(0x4321));
			OKFAILMSG(wxT("Writing an Uint32"), a.WriteUint32(0x4DD43D40));
			a.LeaveObject();
			OKFAILMSG(wxT("Writing an Uint32"), a.WriteUint32(0x11111111));

		}
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (reading back in sync)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 0));
		OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 0));
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);

		OKFAILMSG(wxT("ArchiveStatus reports proper header"), a.GetArchiveStatus().GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), a.GetArchiveStatus().GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports no data loss"), !a.GetArchiveStatus().NewDataLoss());
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (simulating older code with newer stream)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 1));
		std::cout << "Skipping int 16 .... \n";
		std::cout << "Skipping int 32 .... \n";
		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);

		wxSerializeStatus status = a.GetArchiveStatus();
		OKFAILMSG(wxT("ArchiveStatus reports proper header"), status.GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), status.GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports DATA LOSS!"), status.NewDataLoss());

		std::cout << "Resetting flag DATALOSS\n";
		status.OverwriteNew();
		OKFAILMSG(wxT("ArchiveStatus reports no data loss"), !status.NewDataLoss());
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (simulating newer code with older stream)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

		OKFAILMSG(wxT("Newer data ... Reading an Uint8"), !a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Newer data ... Reading an Uint16"), !a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Newer data ... Reading an Uint32"), !a.ReadUint32(along, 3));
		OKFAILMSG(wxT("Newer data ... Reading an Uint64"), !a.ReadUint64(aquad, 4));
		OKFAILMSG(wxT("Newer data ... Reading a bool"), !a.ReadBool(abool, false));
		OKFAILMSG(wxT("Newer data ... Reading a wxFloat64"), !a.ReadDouble(adouble, 1000.0001));

		OKFAILMSG(wxT("Checking Uint8 value"), abyte  == 1);
		OKFAILMSG(wxT("Checking Uint16 value"), ashort == 2);
		OKFAILMSG(wxT("Checking Uint32 value"), along  == 3);
		OKFAILMSG(wxT("Checking Uint64 value"), aquad  == 4);
		OKFAILMSG(wxT("Checking bool value"), !abool);
		OKFAILMSG(wxT("Checking wxFloat64 value"), adouble == 1000.0001);
		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);

		wxSerializeStatus status = a.GetArchiveStatus();
		OKFAILMSG(wxT("ArchiveStatus reports proper header"), status.GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), status.GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports NO no data loss"), !status.NewDataLoss());
	}

	// check block for wxSerialize outa
	printf(" >> TRYING MULTI SKIP ENTER OBJECT - LEAVE OBJECT \n");
	{
		// first create an output stream (should be in current dir
		wxFileOutputStream outfile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), outfile.Ok());
		if(!outfile.Ok())
			return 0;

		// hook up our archive
		wxSerialize a(outfile, 1000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		if(a.IsOk())
		{
			a.EnterObject();
			OKFAILMSG(wxT("Writing an Uint8"), a.WriteUint8(0xef));
			OKFAILMSG(wxT("Writing an Uint16"), a.WriteUint16(0x4321));
			OKFAILMSG(wxT("Writing an Uint32"), a.WriteUint32(0x4DD43D40));

				// another level
				a.EnterObject();
				OKFAILMSG(wxT("Writing an Uint8"), a.WriteUint8(1));
				OKFAILMSG(wxT("Writing an Uint16"), a.WriteUint16(2));
				OKFAILMSG(wxT("Writing an Uint32"), a.WriteUint32(3));
				a.LeaveObject();

			a.LeaveObject();
			OKFAILMSG(wxT("Writing an Uint32"), a.WriteUint32(0x11111111));
		}
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (reading back in sync)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 0));
		OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 0));
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

				// another level
				a.EnterObject();
				OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 0));
				OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 0));
				OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
				a.LeaveObject();

		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (simulating older code with newer stream)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 1));
		std::cout << "Skipping int 16 .... \n";
		std::cout << "Skipping int 32 .... \n";
		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);

		wxSerializeStatus status = a.GetArchiveStatus();
		OKFAILMSG(wxT("ArchiveStatus reports proper header"), status.GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), status.GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports DATA LOSS"), status.NewDataLoss());
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (simulating newer code with older stream)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

		OKFAILMSG(wxT("Newer data ... Reading an Uint8"), !a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Newer data ... Reading an Uint16"), !a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Newer data ... Reading an Uint32"), !a.ReadUint32(along, 3));
		OKFAILMSG(wxT("Newer data ... Reading an Uint64"), !a.ReadUint64(aquad, 4));
		OKFAILMSG(wxT("Newer data ... Reading a bool"), !a.ReadBool(abool, false));
		OKFAILMSG(wxT("Newer data ... Reading a wxFloat64"), !a.ReadDouble(adouble, 1000.0001));

		OKFAILMSG(wxT("Checking Uint8 value"), abyte  == 1);
		OKFAILMSG(wxT("Checking Uint16 value"), ashort == 2);
		OKFAILMSG(wxT("Checking Uint32 value"), along  == 3);
		OKFAILMSG(wxT("Checking Uint64 value"), aquad  == 4);
		OKFAILMSG(wxT("Checking bool value"), !abool);
		OKFAILMSG(wxT("Checking wxFloat64 value"), adouble == 1000.0001);

				// another level
				a.EnterObject();
				OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 0));
				OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 0));
				OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
				a.LeaveObject();

		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);

		wxSerializeStatus status = a.GetArchiveStatus();
		OKFAILMSG(wxT("ArchiveStatus reports proper header"), status.GetHeader().IsSameAs(wxT("SIMPLEHEADER")));
		OKFAILMSG(wxT("ArchiveStatus reports proper version"), status.GetVersion() == 1000);
		OKFAILMSG(wxT("ArchiveStatus reports NO no data loss"), !status.NewDataLoss());
	}

	// check block for wxSerialize outa
	std::cout << " >> TRYING ENTER OBJECT - LEAVE OBJECT (simulating newer (in second block) code with older stream)\n";
	{
		// first create an output stream (should be in current dir
		wxFileInputStream infile(wxT(FILENAME));
		OKFAILMSG(wxT("Opening the file '" FILENAME "'"), infile.Ok());
		if(!infile.Ok())
			return 0;

		// hook up our archive (this is the wrong header!)
		wxSerialize a(infile, 2000, wxT("SIMPLEHEADER"));
		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());

		a.EnterObject();
		OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

		OKFAILMSG(wxT("Newer data ... Reading an Uint8"), !a.ReadUint8(abyte, 1));
		OKFAILMSG(wxT("Newer data ... Reading an Uint16"), !a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Newer data ... Reading an Uint32"), !a.ReadUint32(along, 3));
		OKFAILMSG(wxT("Newer data ... Reading an Uint64"), !a.ReadUint64(aquad, 4));
		OKFAILMSG(wxT("Newer data ... Reading a bool"), !a.ReadBool(abool, false));
		OKFAILMSG(wxT("Newer data ... Reading a wxFloat64"), !a.ReadDouble(adouble, 1000.0001));

		OKFAILMSG(wxT("Checking Uint8 value"), abyte  == 1);
		OKFAILMSG(wxT("Checking Uint16 value"), ashort == 2);
		OKFAILMSG(wxT("Checking Uint32 value"), along  == 3);
		OKFAILMSG(wxT("Checking Uint64 value"), aquad  == 4);
		OKFAILMSG(wxT("Checking bool value"), !abool);
		OKFAILMSG(wxT("Checking wxFloat64 value"), adouble == 1000.0001);

			std::cout << "Entering second object \n";
			a.EnterObject();

			OKFAILMSG(wxT("Reading an Uint8"), a.ReadUint8(abyte, 0));
			OKFAILMSG(wxT("Reading an Uint16"), a.ReadUint16(ashort, 0));
			OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));

			OKFAILMSG(wxT("Newer data ... Reading an Uint8"), !a.ReadUint8(abyte, 1));
			OKFAILMSG(wxT("Newer data ... Reading an Uint16"), !a.ReadUint16(ashort, 2));
			OKFAILMSG(wxT("Newer data ... Reading an Uint32"), !a.ReadUint32(along, 3));
			OKFAILMSG(wxT("Newer data ... Reading an Uint64"), !a.ReadUint64(aquad, 4));
			OKFAILMSG(wxT("Newer data ... Reading a bool"), !a.ReadBool(abool, false));
			OKFAILMSG(wxT("Newer data ... Reading a wxFloat64"), !a.ReadDouble(adouble, 1000.0001));

			OKFAILMSG(wxT("Checking Uint8 value"), abyte  == 1);
			OKFAILMSG(wxT("Checking Uint16 value"), ashort == 2);
			OKFAILMSG(wxT("Checking Uint32 value"), along  == 3);
			OKFAILMSG(wxT("Checking Uint64 value"), aquad  == 4);
			OKFAILMSG(wxT("Checking bool value"), !abool);
			OKFAILMSG(wxT("Checking wxFloat64 value"), adouble == 1000.0001);

			a.LeaveObject();
			std::cout << "Leaving second object \n";

		OKFAILMSG(wxT("Newer data ... Reading an Uint16"), !a.ReadUint16(ashort, 2));
		OKFAILMSG(wxT("Newer data ... Reading an Uint32"), !a.ReadUint32(along, 3));
		OKFAILMSG(wxT("Checking Uint16 value"), ashort == 2);
		OKFAILMSG(wxT("Checking Uint32 value"), along  == 3);

		a.LeaveObject();

		OKFAILMSG(wxT("Our archive should be OK"), a.IsOk());
		OKFAILMSG(wxT("Reading an Uint32"), a.ReadUint32(along, 0));
		OKFAILMSG(wxT("The last int should be 0x11111111"), along == 0x11111111);
	}


	return 0;
}
//---------------------------------------------------------------------------
