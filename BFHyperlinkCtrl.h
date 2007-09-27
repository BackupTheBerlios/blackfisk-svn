/**
 * Name:        BFHyperlinkCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-09-27
 * Copyright:   (c) 2007 Christian Buhtz <exsudat@gmx.de>
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


#ifndef BFHYPERLINKCTRL_H
#define BFHYPERLINKCTRL_H

#include <wx/hyperlink.h>
#include <wx/thread.h>

///
class BFHyperlinkCtrl : public wxHyperlinkCtrl
{
    private:
        /// private members

    protected:
        /// proteced members

    public:
        /// ctor
        BFHyperlinkCtrl (wxWindow* pParent, wxWindowID id, const wxString& strLabel, const wxString& strUrl);

        /// virtual dtor
        virtual ~BFHyperlinkCtrl ();

        ///
        void OnHyperlink (wxHyperlinkEvent& rEvent);

        DECLARE_EVENT_TABLE();
};

///
class BFThread_LaunchBrowser : public wxThread
{
    private:
        ///
        wxString        strUrl_;

    public:
        ///
        BFThread_LaunchBrowser (const wxString& url);

        // thread execution starts here
        virtual void *Entry();
};

#endif    // BFHYPERLINKCTRL_H
