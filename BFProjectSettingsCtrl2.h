/**
 * Name:        BFProjectSettingsCtrl2.h
 * Purpose:     BFProjectSettingsCtrl2 class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-04-20
 * Copyright:   (c) 2007 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFPROJECTSETTINGSCTRL2_H
#define BFPROJECTSETTINGSCTRL2_H

#include <wx/panel.h>

///
class BFProjectSettingsCtrl2 : public wxPanel
{
    private:

    protected:
        /// proteced members

    public:
        /// constructor
        BFProjectSettingsCtrl2 (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFProjectSettingsCtrl2 ();


    DECLARE_EVENT_TABLE();
};    // class BFProjectSettingsCtrl2

#endif    // BFPROJECTSETTINGSCTRL_H
