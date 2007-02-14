/**
 * OBBackupTree.h
 * definition of class OBBackupTree
 * <comment>
 * 2006-05-25
 ***/

#ifndef OBBACKUPTREE_H
#define OBBACKUPTREE_H

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/treectrl.h>
#include <wx/dnd.h>
#include "ObserverPattern.h"
#include "OBMainFrame.h"
#include "OBTask.h"

#define OBICON_PROJECT      0
#define OBICON_VOLUME       1
#define OBICON_DIR          2
#define OBICON_TASKDC       3
#define OBICON_TASKFC       4
#define OBICON_TASKZIP      5

#define OBID_BACKUPCTRL_ADDDESTINATION      1 + wxID_HIGHEST
#define OBID_BACKUPCTRL_COPY_DIR            2 + wxID_HIGHEST
#define OBID_BACKUPCTRL_COPY_FILE           3 + wxID_HIGHEST

///
class OBBackupTree : public wxTreeCtrl, public Observer
{
    private:
        /// application global icons
        wxImageList     imageList_;
        /** this is the droped filename catched in OnDropFiles()
            to remember it for use in OnBackupCopy() backup-action
            is selected in the context-menu displayed after drop */
        wxString        strDropedFilename_;
        /** this is the current selected destination if there is one */
        wxString        strCurrentDestination_;

        /** this private class just wrappes the drop-events from the
            dropTarget to the affeceted treeCtrl
            it is a workaround for a missdesign in wxWidgets */
        class OBBackupDropTarget : public wxFileDropTarget
        {
            private:
                ///
                OBBackupTree* pBackupTree_;


            public:
                OBBackupDropTarget (OBBackupTree* pBackupTree);

                virtual ~OBBackupDropTarget ();

                virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
                virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
        };    // private class OBBackupDropTarget

        /// return a pointer to the OBTask object relating to the itemId in the backupTree
        OBTask* GetTaskByItem (wxTreeItemId itemId);

    protected:
        /// proteced members

    public:
        /// constructor
        OBBackupTree (wxWindow* pParent);

        /// virtual destructor
        virtual ~OBBackupTree ();

        ///
        void Init ();

        ///
        virtual void ValueChanged (Subject* pSender);

        /** search for an item by its label and return the item-id
            for the first match. if nothing is found it returns -1 */
        wxTreeItemId FindItem (const wxChar* label);
        /** search for an item by its label starting at 'idStart'
            and return the item-id for the first match.
            if 'bGoDeep' is false only child-items in the next layer will be searched
            if 'bGoDeep' is true grand-child-items in all layers will be searched
            if nothing is found can be checked be wxTreeItemId::IsOk() */
        wxTreeItemId FindItem (wxTreeItemId idStart, const wxChar* label, bool bGoDeep = true);

        /// add tree items relating to the path to the tree
        wxTreeItemId AddDestination (wxString strPath);


        ///
        void OnItemActivated (wxTreeEvent& event);
        ///
        void OnItemMenu (wxTreeEvent& event);
        ///
        void OnAddDestination (wxCommandEvent& event);
        ///
        void OnBackupCopy (wxCommandEvent& event);
        ///
        void OnBeginLabelEdit (wxTreeEvent& event);
        ///
        void OnEndLabelEdit (wxTreeEvent& event);
        ///
        bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);
        ///
        virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);

        DECLARE_EVENT_TABLE();
};    // class OBBackupTree


///
class OBBackupTreeItemData : public wxTreeItemData
{
    private:
        /// object identification of the referenced OBTask object
        OBoid       oid_;
        /// hold the complete path to this item, if it is a volume or dir
        wxString    strPath_;

        ///
        OBBackupTreeItemData ();

    public:
        ///
        OBBackupTreeItemData (OBoid oid, const wxChar* strPath = NULL);
        ///
        virtual ~OBBackupTreeItemData ();

        /** return the object identification (OID) of the representated OBTask object
            attation: do not confound with wxTreeItemData:GetId() which returns the tree-item-control */
        OBoid GetOID ();
        /// return the path of the item stored in 'strPath_'
        const wxChar* GetPath();

};  // class OBBackupTreeItem
#endif    // OBBACKUPTREE_H
