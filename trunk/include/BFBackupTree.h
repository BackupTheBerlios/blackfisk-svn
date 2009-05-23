/**
 * Name:        BFBackupTree.cpp
 * Purpose:     BFBackupTree and BFBackupCtrl class definition
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2006-05-25
 * Copyright:   (c) 2006 Christian Buhtz <blackfisk@web.de>
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

#ifndef BFBACKUPTREE_H
#define BFBACKUPTREE_H

#include <vector>
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/treectrl.h>
#include <wx/dnd.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/checkbox.h>

#include "ObserverPattern.h"
#include "BFMainFrame.h"
#include "BFTask.h"
#include "BFIconTable.h"
#include "ids.h"

#define BFBACKUPCTRL_ID_ADDDESTINATION          1 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_PROJECTSETTINGS         2 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_CREATEDESTINATION       3 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_TASKSETTINGS            4 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_DELETETASK              5 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_COPY_DIR                6 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_COPY_FILE               7 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_SYNC_DIR                8 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_ARCHIVE_DIR             9 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_MODIFYDDESTINATION      10 + BF_BACKUPCTRL_ID_HIGHEST
#define BF_BACKUPTREE_REBUILD                   11 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_CHECK_PLACEHOLDERS      12 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_MODIFY_TO_COPY_DIR      13 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_MODIFY_TO_SYNC_DIR      14 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_MODIFY_TO_ARCHIVE_DIR   15 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_PROJECTPLANNER          16 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_RUNBACKUP               17 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_PROJECTSAVE             18 + BF_BACKUPCTRL_ID_HIGHEST
#define BFBACKUPCTRL_ID_PROJECTSAVEAS           19 + BF_BACKUPCTRL_ID_HIGHEST

typedef std::pair<wxString, wxTreeItemId>   PairVolItemId;
typedef std::vector<PairVolItemId>          VecPairVolItemId;
typedef VecPairVolItemId::iterator          ItVecPairVolItemId;

typedef std::vector<wxTreeItemId>           VectorTreeItemId;
typedef VectorTreeItemId::iterator          ItVectorTreeItemId;

///
class BFBackupTree : public wxTreeCtrl, public Observer
{
    private:
        /** this is the droped filename catched in OnDropFiles()
            to remember it for use in OnBackupCopy() backup-action
            is selected in the context-menu displayed after drop */
        wxString            strDropedFilename_;

        /** this is the current selected destination if there is one */
        wxString            strCurrentDestination_;

        /** the last (by right-click) selected item;
            is normaly set by OnItemMenu() */
        wxTreeItemId        lastItemId_;

        /** the last found child tasks for 'lastItemId_'
            is normaly set by OnItemMenu() */
        VectorTreeItemId    vecLastChildTasks_;

        /** the oid of the currently draging BFTask
            set in OnBeginDrag() and used in OnDropTask() */
        BFoid               oidCurrentDrag_;

        /// view blackfisk placeholders or fill them with data
        bool                bFillBlackfiskPlaceholders_;


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

        /** */
        class BFTaskDropTarget : public wxDropTarget
        {
            private:

            public:
                BFTaskDropTarget ();
                virtual ~BFTaskDropTarget ();
        };

        /// return a pointer to the BFTask object relating to the itemId in the backupTree
        BFTask* GetTaskByItem (wxTreeItemId itemId);
        /// return the path of the specified item
        const wxChar* GetPathByItem (wxTreeItemId itemId);

        /** Search for items by label starting at 'idStart'
            and return the item-ids in a vector.
            'strLabel' can contain wildcards (*, ?).
            The parameter 'bGoDeep' indicate if only child-items in the next layer
            or grand-child-items in all layers will be searched.
            The paramter 'bSearchForAll' indicate if all matching items should be
            searched or just the first match. */
        VectorTreeItemId FindItems (wxTreeItemId idStart,
                                    const wxString& strLabel,
                                    bool bGoDeep = true,
                                    bool bSearchForAll = true);

        /** Did the same as FindItems() but just return the item-id of the first match.
            If nothing is found it return an invalid wxTreeItemId. Please see
            'VectorTreeItemId FindItems (wxTreeItemId, const wxString&, bool)' for more detailes. */
        wxTreeItemId FindItem (wxTreeItemId idStart, const wxString& strLabel, bool bGoDeep = true);

        /** Search the tree-item corrosponding to the specified oid. */
        wxTreeItemId FindItem (wxTreeItemId idStart, BFoid oid);

        /** Search the tree-item corrosponding to the specified path. */
        wxTreeItemId FindItemByPath (wxTreeItemId idStart, const wxString& strPath);

        /** Search for all items corrosponding to a BFTask and return them in a vector.
            If 'bGoDeep' is false only child-items of 'idParent' will be searched.
            Ff 'bGoDeep' is true all child-items of 'idParent' in all layers will be searched. */
        VectorTreeItemId GetTaskItems (wxTreeItemId idParent, bool bGoDeep = true);

        /// open the TaskDialog for the Task specified by 'id'
        void ShowTaskSettings (wxTreeItemId id);

        /** check all placeholders in the tree-ctrl and
            fill them with data if needed. */
        void UpdatePlaceholders ();
        /** Call UpdateItem() for each item. */
        void UpdateItems (VectorTreeItemId &vec);
        /** Update label text and icon of the specified task-item. */
        void UpdateItem (wxTreeItemId id);

    public:
        /** refill filled placeholders */
        void RefreshPlaceholders ();

        /// constructor
        BFBackupTree (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupTree ();

        ///
        static wxMenu* GenerateBackupMenu (wxMenu*& pMenu, bool bOnDir, bool bBackup = true);

        ///
        void Init ();

        ///
        virtual void ValueChanged (Subject* pSender);

        /// used by BFDirCtrl to set the current handled file
        void SetDropedFilename (wxString strDropedFilename);

        /// if the specified item a task it returns 'true'
        bool IsTask (wxTreeItemId itemId);

        /** Check if the specified tree-item corrospont
            to the specified path. */
        bool HasPath (wxTreeItemId itemId, const wxString& strPath);

        /** Check if the specified tree-item corrospond
            to the specified task-oid. */
        bool HasOID (wxTreeItemId itemId, BFoid oid);

        /** Return true if the related tree item has children. */
        bool HasChildren (BFTask* pTask);

        /// add tree items relating to the path to the tree
        wxTreeItemId AddDestination (const wxString& strPath);

        /// add a tree item as a volume and return its item id
        wxTreeItemId AddVolume(wxTreeItemId idParent,
                               const wxString& strVol,
                               const wxString& strAdd);

        /** Add a task to the tree, create all needed items for that,
            but do not check if the specified task realy exists! */
        wxTreeItemId AddTask (BFoid oid,
                              BFTaskType type,
                              const wxString& strName,
                              const wxString& strDestination);

        /// return the iconId of the task
        static BFIconTable::BFFilesystemIconId GetTypeIconId (BFTaskType type);

        ///
        bool GetFillBlackfiskPlaceholders ();
        /** set 'bFillBlackfiskPlaceholders_' and recreate the tree if needed */
        void SetFillBlackfiskPlaceholders (bool bValue);
        ///
        void OnItemActivated (wxTreeEvent& rEvent);
        ///
        void OnItemMenu (wxTreeEvent& rEvent);
        ///
        void OnProjectSettings (wxCommandEvent& rEvent);
        ///
        void OnProjectPlanner (wxCommandEvent& rEvent);
        ///
        void OnBackupRun (wxCommandEvent& rEvent);
        ///
        void OnProjectSave (wxCommandEvent& rEvent);
        ///
        void OnProjectSaveAs (wxCommandEvent& rEvent);
        ///
        void OnAddDestination (wxCommandEvent& rEvent);
        ///
        void OnCreateDestination (wxCommandEvent& rEvent);
        ///
        void OnModifyDestination (wxCommandEvent& rEvent);
        ///
        void OnTaskSettings (wxCommandEvent& rEvent);
        ///
        void OnDeleteTask (wxCommandEvent& rEvent);
        ///
        void OnCreateBackup (wxCommandEvent& rEvent);
        /** called from BFBackupTree::ValueChanged() to eleminate
            some wx-internal crashes */
        void OnRebuild (wxCommandEvent& rEvent);
        ///
        void OnBeginLabelEdit (wxTreeEvent& rEvent);
        ///
        void OnEndLabelEdit (wxTreeEvent& rEvent);
        ///
        bool OnDropFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);
        ///
        bool OnDropTask (wxCoord x, wxCoord y);
        ///
        virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
        ///
        void OnBeginDrag (wxTreeEvent& event);
        ///
        void OnModifyTaskType (wxCommandEvent& rEvent);

    DECLARE_EVENT_TABLE();
};


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
        BFBackupTreeItemData (BFoid oid, const wxString& strPath = wxEmptyString);
        ///
        virtual ~BFBackupTreeItemData ();

        /** return the object identification (OID) of the representated BFTask object
            attation: do not confound with wxTreeItemData:GetId() which returns the tree-item-control */
        BFoid GetOID ();
        /// return the path of the item stored in 'strPath_'
        const wxString& GetPath();

};


/// contains the BFBackupTreeCtrl, toolbar, etc on the left side of the splitter
class BFBackupCtrl : public wxPanel
{
    private:
        ///
        BFBackupTree*   pBackupTree_;
        ///
        wxCheckBox*     pCheckPlaceholders_;

    protected:

    public:
        /// constructor
        BFBackupCtrl (wxWindow* pParent);

        /// virtual destructor
        virtual ~BFBackupCtrl ();

        ///
        BFBackupTree* BackupTree ();

        ///
        void OnCheck_Placeholders (wxCommandEvent& rEvent);

        ///


        DECLARE_EVENT_TABLE();
};

#endif    // BFBACKUPTREE_H
