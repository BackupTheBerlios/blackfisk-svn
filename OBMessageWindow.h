/**
 * OBMessageWindow.h
 * definition of class OBMessageWindow
 * <comment>
 * 2006-04-06
 ***/

#ifndef OBMESSAGEWINDOW_H
#define OBMESSAGEWINDOW_H

#include <wx/wx.h>
#include <wx/window.h>


///
class OBMessageWindow : public wxWindow
{
    private:
        ///
        wxListBox*      pListBox_;

    protected:
        /// proteced members

    public:
        /// constructor
        OBMessageWindow (wxWindow* pParent);

        /// virtual destructor
        virtual ~OBMessageWindow ();

        ///
        void Add (wxString strMsg);

};    // class OBMessageWindow

#endif    // OBMESSAGEWINDOW_H
