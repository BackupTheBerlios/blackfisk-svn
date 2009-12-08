/**
 * Name:        BFEnvironment.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-11-25
 * Copyright:   (c) 2008 Christian Buhtz <blackfisk@web.de>
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


#ifndef BFENVIRONMENT_H
#define BFENVIRONMENT_H

#include <wx/string.h>
#include <wx/cmdargs.h>

///
class BFEnvironment
{
    private:
        /// ctor
        BFEnvironment ();

        /// the path to the binary
        static wxString     strApplicationDir_;
		/// the binary itself
		static wxString		strApplicationBinary_;

        /** If 'true' the current backup project is opened
            with the option '--run'. Normaly it is done by
            schedulers. */
        static bool         bProjectScheduled_;

		/** Return the application config dir for the current user.
			If it doesn't exists it will create it. */
		static wxString GetUserConfigDir ();

    public:
        /// virtual dtor
        virtual ~BFEnvironment ();

        //        static void RememberApplicationDirectoryAndName (const wxCmdLineArgsArray& argv);

        /// Please see 'strApplicationDir_' for more details.
        static const wxString& GetApplicationDirectory ();
		/// Please see 'strApplicationBinary_' for more details.
		static const wxString& GetApplicationBinary ();
		/// Please see 'strApplicationDir_' and 'strApplicationBinary_' for more details.
		static wxString GetApplicationPath ();
		///
		static wxString GetDocumentOpenCommand ();
        ///
        static const wxString GetLogFileName ();
        ///
        static const wxString GetGraphicDir ();
        ///
        static const wxString GetSoundDir ();
        ///
        static const wxString GetSettingsFileName ();
        ///
        static const wxString GetInBuildCrontabFileName ();

        ///
        static void SetProjectScheduled (bool bScheduled);
        ///
        static bool IsProjectScheduled ();

};

#endif
