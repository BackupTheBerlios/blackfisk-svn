/**
 * BFBackupTree.h
 * definition of class BFBackupTree
 * <comment>
 * 2006-05-25
 ***/

#ifndef BFBACKUPTREE_H
#define BFBACKUPTREE_H

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/treectrl.h>
#include <wx/dnd.h>
#include <wx/panel.h>
#include "ObserverPattern.h"
#include "BFMainFrame.h"
#include "BFTask.h"


#define BFID_BACKUPCTRL_ADDDESTINATION      1 + wxID_HIGHEST
#define BFID_BACKUPCTRL_COPY_DIR            2 + wxID_HIGHEST
#define BFID_BACKUPCTRL_COPY_FILE           3 + wxID_HIGHEST

///
class BFBackupTree : public wxTreeCtrl, public Observer
{
    private:
        /** this is the droped filename catched in OnDropFiles()
            to remember it for use in OnBackupCopy() backup-action
            is selected in the context-menu displayed after drop */
        wxString        strDropedFilename_;
        /** this is the current selected destination if there is one */
        wxString        strCurrentDestination_;

        /** this private class just wrappes the drop-events from the
            dropTarget to the affeceted treeCtrl
            it is a workaround for a missdesign in wxWidgets */
        class BFBackupDropTarget : public wxFileDropTarget
        {
            private:
                ///
                BFBackupTree* pBackupTree_;


            public:
                BFBackupDropTarget (BFBackupTree* pBackupTree);

                virtual ~BFBackupDropTarget ();

                virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
                virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
        };    // private class BFBackupDropTarget

        /// return a pointer to the BFTask object relating to the itemId in the backupTree
        BFTask* GetTaskByItem (wxTreeItemId itemId);

    protected:
        /// proteced members

    public:
        /// constructor
        BFBackupTree (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupTree ();

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
};    // class BFBackupTree


///
class BFBackupTreeItemData : public wxTreeItemData
{
    private:
        /// object identification of the referenced BFTask object
        BFoid       oid_;
        /// hold the complete path to this item, if it is a volume or dir
        wxString    strPath_;

        ///
        BFBackupTreeItemData ();

    public:
        ///
        BFBackupTreeItemData (BFoid oid, const wxChar* strPath = NULL);
        ///
        virtual ~BFBackupTreeItemData ();

        /** return the object identification (OID) of the representated BFTask object
            attation: do not confound with wxTreeItemData:GetId() which returns the tree-item-control */
        BFoid GetOID ();
        /// return the path of the item stored in 'strPath_'
        const wxChar* GetPath();

};  // class BFBackupTreeItem


/// contains the BFBackupTreeCtrl, toolbar, etc on the left side of the splitter
class BFBackupCtrl : public wxPanel
{
    private:
        ///
        BFBackupTree*   pBackupTree_;

    protected:

    public:
        /// constructor
        BFBackupCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupCtrl ();

        ///
        BFBackupTree* BackupTree ();
};    // class BFBackupCtrl

#endif    // BFBACKUPTREE_H
