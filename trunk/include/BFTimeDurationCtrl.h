/**
 * Name:        BFTimeDurationCtrl.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-02-26
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


#ifndef BFTIMEDURATIONCTRL_H
#define BFTIMEDURATIONCTRL_H

#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/datetime.h>
#include "ObserverPattern.h"

//
class Progress;

///
class BFTimeDurationCtrl : public Observer, public wxGridBagSizer
{
    private:
        ///
        wxWindow*           pParent_;
        ///
        wxStaticText*       pElapsedTime_;
        ///
        wxStaticText*       pRemainingTime_;

        ///
        wxDateTime          timeBegin_;

    protected:
        /// proteced members

    public:
        /// ctor
        BFTimeDurationCtrl (wxWindow* pParent,
                            Progress* pProgressToObserve);

        /// virtual dtor
        virtual ~BFTimeDurationCtrl ();

        ///
        virtual void ValueChanged (Subject* pSender);
};

#endif    // BFTIMEDURATIONCTRL_H
