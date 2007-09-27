/**
 * Name:        BFHyperlinkCtrl.cpp
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


#include "BFHyperlinkCtrl.h"

BEGIN_EVENT_TABLE(BFHyperlinkCtrl, wxHyperlinkCtrl)
  EVT_HYPERLINK ( 1, BFHyperlinkCtrl::OnHyperlink )
END_EVENT_TABLE()

//
BFHyperlinkCtrl::BFHyperlinkCtrl (wxWindow* pParent,
                                  wxWindowID id,
                                  const wxString& strLabel,
                                  const wxString& strUrl)
               : wxHyperlinkCtrl (pParent, 1, strLabel, strUrl)
{
}


//
/*virtual*/ BFHyperlinkCtrl::~BFHyperlinkCtrl ()
{
}

void BFHyperlinkCtrl::OnHyperlink (wxHyperlinkEvent& rEvent)
{
    //wxLaunchDefaultBrowser(GetURL());
    new BFThread_LaunchBrowser(GetURL());
}

BFThread_LaunchBrowser::BFThread_LaunchBrowser (const wxString& url)
                      : strUrl_(url)
{
    Create();
    Run();
}

/*virtual*/ void* BFThread_LaunchBrowser::Entry()
{
    wxLaunchDefaultBrowser(strUrl_);
    return NULL;
}

