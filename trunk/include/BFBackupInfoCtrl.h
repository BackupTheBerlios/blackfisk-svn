/**
 * Name:        BFBackupInfoCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2007-11-13
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


#ifndef BFBACKUPINFOCTRL_H
#define BFBACKUPINFOCTRL_H

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include "ObserverPattern.h"

/** this control display the number of messages
    while a backup */
class BFBackupInfoCtrl : public wxPanel, public Observer
{
    private:
        ///
        wxTextCtrl*         pLogCtrl_;
        ///
        wxStaticText*       pWarningsCounter_;
        ///
        wxStaticText*       pErrorsCounter_;
        ///
        wxStaticText*       pFatalErrorsCounter_;
        ///
        long                lCountWarnings_;
        ///
        long                lCountErrors_;
        ///
        long                lCountFatal_;
        ///
        void Init();
        ///
        void RefreshCounterCtrls();

    protected:
        /// proteced members

    public:
        /// ctor
        BFBackupInfoCtrl (wxWindow* pParent);

        /// virtual dtor
        virtual ~BFBackupInfoCtrl ();

        ///
        virtual void ValueChanged (Subject* pSender);
};

#endif    // BFBACKUPINFOCTRL_H
