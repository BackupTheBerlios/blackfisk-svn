/**
 * Name:        BFTaskListCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-08
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


#ifndef BFTASKLISTCTRL_H
#define BFTASKLISTCTRL_H

#include <wx/sizer.h>
#include "BFTaskBase.h"
#include "ObserverPattern.h"

// forwarde declarations
class BFTask;

///
class BFTaskListCtrl : public wxStaticBoxSizer, public Observer
{
    ///
    typedef std::pair<BFoid, wxStaticText*>     Item;
    ///
    typedef std::vector<Item>                   ItemVector;

    private:
        ///
        wxWindow*                           pParent_;
        ///
        ItemVector                          vecItems_;
        ///
        BFoid                               oidActive_;
        ///
        bool                                bActiveTaskSucceeded_;

        ///
        wxStaticText* FindItem (BFoid oid);

    public:
        /// ctor
        BFTaskListCtrl (wxWindow* pParent);

        /// virtual dtor
        virtual ~BFTaskListCtrl ();

        ///
        virtual void ValueChanged (Subject* pSender);

        ///
        void Append (const BFTask* pTask);

        ///
        bool SetTask_Active (BFoid oid);
        ///
        bool SetTask_Succeeded (BFoid oid);
        ///
        bool SetTask_Warning (BFoid oid);
        ///
        bool SetTask_Errror (BFoid oid);
};

#endif    // BFTASKLISTCTRL_H
