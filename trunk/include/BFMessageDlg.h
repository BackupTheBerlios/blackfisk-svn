/**
 * Name:        BFMessageDlg.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-05-14
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


#ifndef BFMESSAGEDLG_H
#define BFMESSAGEDLG_H

//
typedef short BFMessageDlg_Type;

#define BF_MSGDLG_QUESTION_YESNO        1
#define BF_MSGDLG_QUESTION_YESNOCANCEL  2
#define BF_MSGDLG_INFO                  3
#define BF_MSGDLG_WARNING               4
#define BF_MSGDLG_ERROR                 5
#define BF_MSGDLG_FATAL                 6
#define BF_MSGDLG_BACKUP_QUESTION       7
#define BF_MSGDLG_NOTUSED               8

#define BF_MSGDLG_ID_OK         wxID_OK
#define BF_MSGDLG_ID_YES        wxID_YES
#define BF_MSGDLG_ID_NO         wxID_NO
#define BF_MSGDLG_ID_CANCEL     wxID_CANCEL
#define BF_MSGDLG_ID_STOPPRJ    1 + wxID_HIGHEST
#define BF_MSGDLG_ID_STOPTSK    2 + wxID_HIGHEST
#define BF_MSGDLG_ID_IGNORE     3 + wxID_HIGHEST
#define BF_MSGDLG_ID_TIMER      4 + wxID_HIGHEST

#include <wx/dialog.h>
#include <wx/timer.h>

#include "BFSystemBase.h"

/** replacement for wxMessageDialog
    Return values for ShowModal() can be
        wxID_OK
        wxID_YES
        wxID_NO
        wxID_CANCEL
        BF_MSGDLG_ID_OK (the same as wxID_OK)
        BF_MSGDLG_ID_YES (the same as wxID_YES)
        BF_MSGDLG_ID_NO (the same as wxID_NO)
        BF_MSGDLG_ID_CANCEL (the same as wxID_CANCEL)
        BF_MSGDLG_ID_STOPPRJ
        BF_MSGDLG_ID_STOPTSK
        BF_MSGDLG_ID_IGNORE
 */
class BFMessageDlg : public wxDialog
{
    private:
        ///
        BFMessageDlg_Type   type_;
        ///
        wxTimer*            pTimer_;
        ///
        long                lTimer_;

        ///
        static wxString GetMessageTypeString (BFMessageDlg_Type type);
        ///
        static const wxBitmap GetMessageTypeBitmap (BFMessageDlg_Type type);
        ///
        static void PlayMessageTypeSound (BFMessageDlg_Type type);
        ///
        const wxBitmap GetButtonBitmap (wxWindowID id);

        ///
        void CreateButtons_Default (wxSizer* pSizer);
        ///
        void CreateButtons_Question (wxSizer* pSizer, bool bWithCancel);
        ///
        void CreateButtons_BackupQuestion (wxSizer* pSizer);

        ///
        void Wrap (wxString& strMyMsg);

        ///
        void SetTitle ();

    public:
        /**
         The parameter 'type' specify the concret type of the message dialog.
         It has effect which buttons and icons are used.
         The parameter 'strMsg' is the message to display.
         The parameter 'strTitle' can be used to set a title. If 'strTitle' is
         empty a default string corrosponding to 'type' will be used.
         The parameter 'type_icon' specify the type of icon to use. This parameter
         can be used to set a icon different from the first parameter 'type'.
         For more detailes on 'lTimer' please look at the member 'lTimer_'. */
        BFMessageDlg (BFMessageDlg_Type type,
                      const wxString& strMsg,
                      const wxString& strTitle = wxEmptyString,
                      BFMessageDlg_Type type_icon = BF_MSGDLG_NOTUSED,
                      long lTimer = 0);

        /// virtual dtor
        virtual ~BFMessageDlg ();

        ///
        void OnButton (wxCommandEvent& rEvent);
        ///
        void OnTimer (wxTimerEvent& rEvent);

        ///
        static BFMessageDlg_Type GetDlgType (BFMessageType type);

    DECLARE_EVENT_TABLE()
};

#endif    // BFMESSAGEDLG_H
