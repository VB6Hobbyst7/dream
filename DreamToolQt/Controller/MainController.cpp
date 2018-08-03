/*
 * MainController.cpp
 *
 * Created: 14 2017 by Ashley
 *
 * Copyright 2017 Octronic. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 */

#include "MainController.h"

#include <QDebug>
#include <QFileDialog>
#include <QErrorMessage>
#include <QModelIndexList>
#include <DreamCore.h>
#include <QOpenGLContext>
#include <QLayout>
#include <QVBoxLayout>
#include <QColorDialog>

#include "../Model/TreeModels/Properties/AssetDefinition/AssetDefinitionPropertiesModel.h"
#include "../Model/TreeModels/Properties/Project/ProjectPropertiesModel.h"
#include "../Model/TreeModels/Properties/SceneObject/SceneObjectPropertiesModel.h"
#include "../Model/TreeModels/Properties/Scene/ScenePropertiesModel.h"

#include <iostream>
#include <spdlog/spdlog.h>

using std::cout;

// Constructors/Destructors =====================================================

MainController::MainController
(MainWindow* parent)
    : QObject(parent),
      mMainWindowHandle(parent),
      mWindowComponentHandle(parent->getWindowComponent()),
      mSelectedProjectDefinitionHandle(nullptr),
      mSelectedAssetDefinitionHandle(nullptr),
      mSelectedSceneDefinitionHandle(nullptr),
      mSelectedSceneObjectDefinitionHandle(nullptr)
{
    auto log = spdlog::get("MainController");

    if (!log)
    {
        log = spdlog::stdout_color_mt("MainController");
    }

    log->info( "Constructing Object");

    mDreamProjectModel.reset
            (
                new DreamProjectModel
                (
                    this,
                    mWindowComponentHandle
                    )
                );

    mMacOsOpenModel.reset
            (
                new MacOSOpenModel
                (
                    &mPreferencesDialogController.getPreferencesModel(),
                    this
                    )
                );

    setupUI();
    setupConnections();
    mScriptEditor.setProjectDirectoryModelHandle(&mProjectDirectoryModel);
}

MainController::~MainController
()
{
    auto log = spdlog::get("MainController");
    log->info( "Destroying Object");
}

// Setup ========================================================================

void
MainController::setupUI
()
{
    setupUI_GLWidgets();
}

void
MainController::setupUI_GLWidgets
()
{
    mGrid.reset(new Grid(this));
    mWindowComponentHandle->setGridHandle(getGridHandle());

    mSelectionHighlighter.reset(new SelectionHighlighter(this));
    mWindowComponentHandle->setSelectionHighlighterHandle(getSelectionHighlighterHandle());

    mRelationshipTree.reset(new RelationshipTree(this));
    mWindowComponentHandle->setRelationshipTreeHandle(getRelationshipTreeHandle());
}

void
MainController::setupUI_AssetDefinitionPropertiesTreeViewModel
(AssetDefinitionTreeItem* item)
{
    auto log = spdlog::get("MainController");
    QTreeView *propertiesTreeView = mMainWindowHandle->getPropertiesTreeView();

    mPropertiesModel.reset(nullptr);

    switch(item->getType())
    {
        case AssetDefinitionTreeItemType::ASSET_DEFINITION:
            log->info( "Selected an asset definition");
            mSelectedAssetDefinitionHandle = static_cast<AssetDefinitionTreeItem*>(item)->getAssetDefinition();
            mPropertiesModel.reset
                    (
                        new AssetDefinitionPropertiesModel
                        (
                            mSelectedAssetDefinitionHandle,
                            &mTemplatesModel,
                            propertiesTreeView
                            )
                        );

            mMainWindowHandle->setPropertiesDockWidgetTitle("Asset Definition Properties");

            // Audio File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_AudioFile(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_AudioFile(IAssetDefinition*))
                        );

            // Font File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_FontFile(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_FontFile(IAssetDefinition*))
                        );

            // Model File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_ModelFile(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_ModelFile(IAssetDefinition*))
                        );

            // Model Additional Files
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_ModelAdditionalFiles(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_ModelAdditionalFiles(IAssetDefinition*))
                        );

            // Remove Files
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveFiles(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_RemoveFiles(IAssetDefinition*))
                        );

            // Edit Script
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_EditScript(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_EditScript(IAssetDefinition*))
                        );

            // Edit Vertex Shader
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_EditShader(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_EditShader(IAssetDefinition*))
                        );

            // Script Template
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyCombo_ScriptTemplateChanged(IAssetDefinition*,QString)),
                        this,
                        SLOT(onAssetDefinitionProperty_ScriptTemplateChanged(IAssetDefinition*,QString))
                        );

            // Shader Template
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyCombo_ShaderTemplateChanged(IAssetDefinition*,QString)),
                        this,
                        SLOT(onAssetDefinitionProperty_ShaderTemplateChanged(IAssetDefinition*,QString))
                        );

            // PhysicsObject Mesh File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_PhysicsBvhTriangleMeshFile(IAssetDefinition*)),
                        this,
                        SLOT(onAssetDefinitionProperty_PhysicsBvhTriangleMeshFile(IAssetDefinition*))
                        );
            break;

        case AssetDefinitionTreeItemType::ASSET_TREE_NODE:
            log->info( "Selected an asset tree node");
            break;
    }

    if (mPropertiesModel)
    {
        propertiesTreeView->setModel(mPropertiesModel.get());
    }
}

void
MainController::setupUI_ScenegraphPropertiesTreeViewModel
(ScenegraphTreeItem *item)
{
    auto log = spdlog::get("MainController");
    QTreeView *propertiesTreeView = mMainWindowHandle->getPropertiesTreeView();
    SceneObjectRuntime *selectedSceneObjectRuntime = nullptr;

    // Reset the selected object
    if (mSelectionHighlighter)
    {
        mSelectionHighlighter->setSelectedSceneObjectRuntimeHandle(nullptr);
    }

    mPropertiesModel.reset(nullptr);

    switch(item->getType())
    {
        case ScenegraphTreeItemType::SCENEGRAPH_PROJECT:
            log->info( "Selected a project");
            mSelectedProjectDefinitionHandle = mDreamProjectModel->getProject()->getProjectDefinitionHandle();
            mPropertiesModel.reset(new ProjectPropertiesModel(mSelectedProjectDefinitionHandle,propertiesTreeView));
            mMainWindowHandle->setPropertiesDockWidgetTitle("Project Properties");
            break;

        case ScenegraphTreeItemType::SCENEGRAPH_SCENE:
            log->info( "Selected a scene");
            mSelectedSceneDefinitionHandle = static_cast<SceneDefinition*>(static_cast<ScenegraphTreeItem*>(item)->getItem());
            mDreamProjectModel->setSelectedSceneDefinitionHandle(mSelectedSceneDefinitionHandle);

            if (mSelectedSceneDefinitionHandle)
            {
                mPropertiesModel.reset(new ScenePropertiesModel(mSelectedSceneDefinitionHandle,propertiesTreeView));
            }

            mMainWindowHandle->setPropertiesDockWidgetTitle("Scene Properties");

            // Capture Camera Translation
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureCameraTranslation(SceneDefinition*)),
                        this,
                        SLOT(onSceneProperty_CaptureCameraTranslation(SceneDefinition*))
                        );

            // Capture Camera Rotation
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureCameraRotation(SceneDefinition*)),
                        this,
                        SLOT(onSceneProperty_CaptureCameraRotation(SceneDefinition*))
                        );

            // Choose Clear Colour
            connect
                    (
                        mPropertiesModel.get(), SIGNAL(notifyButton_ChooseClearColour(SceneDefinition*)),
                        this, SLOT(onSceneProperty_ChooseClearColour(SceneDefinition*))
                        );
            // Choose Ambient Colour
            connect
                    (
                        mPropertiesModel.get(), SIGNAL(notifyButton_ChooseAmbientColour(SceneDefinition*)),
                        this, SLOT(onSceneProperty_ChooseAmbientColour(SceneDefinition*))
                        );
            break;

        case ScenegraphTreeItemType::SCENEGRAPH_SCENE_OBJECT:
            log->info( "Selected a scene object");
            mSelectedSceneObjectDefinitionHandle = static_cast<SceneObjectDefinition*>(static_cast<ScenegraphTreeItem*>(item)->getItem());
            mPropertiesModel.reset(new SceneObjectPropertiesModel(mSelectedSceneObjectDefinitionHandle,propertiesTreeView));

            mMainWindowHandle->setPropertiesDockWidgetTitle("Scene Object Properties");

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureTranslation(SceneObjectDefinition*)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureTranslation(SceneObjectDefinition*))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureRotation(SceneObjectDefinition*)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureRotation(SceneObjectDefinition*))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureScale(SceneObjectDefinition*)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureScale(SceneObjectDefinition*))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveAsset(SceneObjectDefinition*,IAssetDefinition*)),
                        this,
                        SLOT(onSceneObjectProperty_RemoveAsset(SceneObjectDefinition*,IAssetDefinition*))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveChild(SceneObjectDefinition*,SceneObjectDefinition*)),
                        this,
                        SLOT(onSceneObjectProperty_RemoveChild(SceneObjectDefinition*,SceneObjectDefinition*))
                        );

            if (mSelectionHighlighter)
            {
                if (mDreamProjectModel->getProject()->hasProjectRuntime())
                {
                    ProjectRuntime* prHandle = mDreamProjectModel->getProject()->getProjectRuntimeHandle();
                    if (prHandle->hasActiveSceneRuntime())
                    {
                        selectedSceneObjectRuntime = mDreamProjectModel
                                ->getProject()
                                ->getProjectRuntimeHandle()
                                ->getActiveSceneRuntimeHandle()
                                ->getSceneObjectRuntimeHandleByUuid(mSelectedSceneObjectDefinitionHandle->getUuid());
                        mSelectionHighlighter->setSelectedSceneObjectRuntimeHandle(selectedSceneObjectRuntime);
                    }
                }
            }
            break;

        case ScenegraphTreeItemType::SCENEGRAPH_TREE_NODE:
            log->info( "Selected a tree node");
            break;
    }

    if (mPropertiesModel)
    {
        propertiesTreeView->setModel(mPropertiesModel.get());
    }
}

void
MainController::setupConnections
()
{
    connectMenus();
    connectUI();
}

// Signal/Slot Connectors =======================================================

void
MainController::connectMenus
()
{
    connectPreferences();
    connectFileMenu();
    connectSceneMenu();
    connectAssetMenu();
    connectViewMenu();
    connectDebugMenu();
}

void
MainController::connectPreferences
()
{
    connect
            (
                mMainWindowHandle->getAction_Preferences(),SIGNAL(triggered()),
                this, SLOT(onAction_Preferences())
                );
}

void
MainController::connectFileMenu
()
{
    // actionNew
    connect
            (
                mMainWindowHandle->getAction_File_New(), SIGNAL(triggered()),
                this, SLOT(onAction_File_New())
                );

    // actionOpen
    connect
            (
                mMainWindowHandle->getAction_File_Open(), SIGNAL(triggered()),
                this, SLOT(onAction_File_Open())
                );

    // actionSave
    connect
            (
                mMainWindowHandle->getAction_File_Save(), SIGNAL(triggered()),
                this, SLOT(onAction_File_Save())
                );

    // actionCloseProject
    connect
            (
                mMainWindowHandle->getAction_File_CloseProject(), SIGNAL(triggered()),
                this, SLOT(onAction_File_Close())
                );
}

void
MainController::connectSceneMenu
()
{
    // actionPlay
    connect
            (
                mMainWindowHandle->getAction_Scene_Play(), SIGNAL(triggered()),
                this, SLOT(onAction_Scene_Play())
                );

    // actionReload
    connect
            (
                mMainWindowHandle->getAction_Scene_Reload(), SIGNAL(triggered()),
                this, SLOT(onAction_Scene_Reload())
                );

    // actionStop
    connect
            (
                mMainWindowHandle->getAction_Scene_Stop(), SIGNAL(triggered()),
                this, SLOT(onAction_Scene_Stop())
                );

    // New Scene
    connect
            (
                mMainWindowHandle->getAction_Scene_NewScene(),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Scene_NewScene())
                );

    // New Scene Object
    connect
            (
                mMainWindowHandle->getAction_Scene_NewSceneObject(),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Scene_NewSceneObject())

                );
}

void
MainController::connectAssetMenu
()
{
    // Assets Menu -> Add To Selected SceneObject
    connect
            (
                mMainWindowHandle->getAction_Asset_AddToSelectedSceneObject(),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_AddToSelectedSceneObjectDefinition())
                );

    // Asset Menu -> New Definition -> Animation
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::ANIMATION),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Animation())
                );

    // Asset Menu -> New Definition -> Audio
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::AUDIO),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Audio())
                );

    // Asset Menu -> New Definition -> Font
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::FONT),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Font())
                );

    // Asset Menu -> New Definition -> Light
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::LIGHT),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Light())
                );

    // Asset Menu -> New Definition -> Model
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::MODEL),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Model())
                );

    // Asset Menu -> New Definition -> Physics Object
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::PHYSICS_OBJECT),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_PhysicsObject())
                );

    // Asset Menu -> New Definition -> Script
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::SCRIPT),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Script())
                );

    // Asset Menu -> New Definition -> Shader
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::SHADER),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Shader())
                );

    // Asset Menu -> New Definition -> Sprite
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::SPRITE),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Sprite())
                );
}

void
MainController::connectViewMenu
()
{
    // Action Toggle Grid
    connect
            (
                mMainWindowHandle->getAction_View_ToggleGrid(),SIGNAL(triggered(bool)),
                this,SLOT(onAction_View_ToggleGrid(bool))
                );

    // Action Toggle Debug
    connect
            (
                mMainWindowHandle->getAction_View_ToggleDebug(),SIGNAL(triggered(bool)),
                this,SLOT(onAction_View_ToggleDebug(bool))
                );

    // Action Toggle Physics Debug
    connect
            (
                mMainWindowHandle->getAction_View_TogglePhysicsDebug(),SIGNAL(triggered(bool)),
                this,SLOT(onAction_View_TogglePhysicsDebug(bool))
                );

}

void
MainController::connectDebugMenu
()
{
    connect
            (
                mMainWindowHandle->getAction_Debug_DumpProjectDefinitionJson(),
                SIGNAL(triggered(bool)),
                this,
                SLOT(onAction_Debug_DumpProjectDefinitionJson(bool))
                );
}

void
MainController::connectUI
()
{
    // Status Bar
    connect
            (
                this, SIGNAL(notifyStatusBarProjectLoaded(QString)),
                mMainWindowHandle, SLOT(showStatusBarMessage(QString))
                );
    // Project Directory Changed
    connect
            (
                this, SIGNAL(notifyProjectDirectoryChanged(QString)),
                mMainWindowHandle, SLOT(setWindowTitle(QString))
                );
    // Valid Scene Selected
    connect
            (
                mDreamProjectModel.get(), SIGNAL(notifySelectedSceneChanged(SceneDefinition*)),
                this, SLOT(onUI_SelectedSceneChanged(SceneDefinition*))
                );
    // Invalid Project Directory
    connect
            (
                this, SIGNAL(notifyInvalidProjectDirectory(QString)),
                mMainWindowHandle, SLOT(onInvalidProjectDirectory(QString))
                );
    // No Scene Selected
    connect
            (
                this,SIGNAL(notifyNoSceneSelected()),
                mMainWindowHandle, SLOT(onNoSceneSelected())
                );
}

void
MainController::connectUI_TreeViewModels
()
{
    // projectTreeView
    connect
            (
                mMainWindowHandle->getScenegraphTreeView()->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                this, SLOT(onUI_ScenegraphTreeViewSelectionChanged(const QItemSelection&,const QItemSelection&))
                );

    // assetDefinitionTreeView
    connect
            (
                mMainWindowHandle->getAssetDefinitionTreeView()->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                this, SLOT(onUI_AssetDefinitionTreeViewSelectionChanged(const QItemSelection&,const QItemSelection&))
                );

    mMainWindowHandle->getScenegraphTreeView()->expandAll();
    //mMainWindowHandle->getAssetDefinitionTreeView()->expandAll();
}

void
MainController::updateWindowTitle
(QString msg)
{
    emit notifyProjectDirectoryChanged("Dream Tool :: " + msg);
}

void
MainController::setActionsEnabled_ValidProject
(bool enabled)
{
    mMainWindowHandle->setActionsEnabled_Scene_Playback(enabled);
    mMainWindowHandle->setActionsEnabled_Scene_Modification(enabled);
    mMainWindowHandle->setActionEnabled_File_Save(enabled);
}

QStringListModel*
MainController::getSceneNamesListModel
(vector<SceneDefinition*> sceneList)
{
    QStringList sceneNameList;
    mSceneListModel.reset(new QStringListModel(mMainWindowHandle));

    for (auto scene : sceneList)
    {
        sceneNameList.push_back(QString::fromStdString(scene->getName()));
    }

    mSceneListModel->setStringList(sceneNameList);
    return mSceneListModel.get();
}

void
MainController::onAction_Scene_Play
()
{
    auto log = spdlog::get("MainController");
    log->info( "onReloadProject Clicked");
    SceneDefinition *sceneDefinitionHandle = mDreamProjectModel->getSelectedSceneDefinitionHandle();
    if (sceneDefinitionHandle)
    {
        mDreamProjectModel->startSceneRuntimeFromDefinition(sceneDefinitionHandle);
    }
    else
    {
        emit notifyNoSceneSelected();
    }
}

void
MainController::onAction_Scene_Reload
()
{
    onAction_Scene_Stop();
    onAction_Scene_Play();
}

void
MainController::onAction_Scene_Stop
()
{
    mSelectionHighlighter->setSelectedSceneObjectRuntimeHandle(nullptr);
    mDreamProjectModel->stopActiveSceneRuntime();
}

void
MainController::onUI_ScenegraphTreeViewSelectionChanged
(const QItemSelection& selected,const QItemSelection& deselected)
{
    Q_UNUSED(deselected)
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0)
    {
        ScenegraphTreeItem *selected = static_cast<ScenegraphTreeItem*>(indexes.at(0).internalPointer());
        setupUI_ScenegraphPropertiesTreeViewModel(selected);
        mMainWindowHandle->getPropertiesTreeView()->expandAll();
    }
}

void
MainController::onUI_AssetDefinitionTreeViewSelectionChanged
(const QItemSelection& selected,const QItemSelection& deselected)
{
    Q_UNUSED(deselected)
    QModelIndexList indexes = selected.indexes();
    if (indexes.size() > 0)
    {
        AssetDefinitionTreeItem *selected = static_cast<AssetDefinitionTreeItem*>(indexes.at(0).internalPointer());
        setupUI_AssetDefinitionPropertiesTreeViewModel(selected);
        mMainWindowHandle->getPropertiesTreeView()->expandAll();
    }
}

// Menu Action Handlers =========================================================

// File Menu

void
MainController::onAction_File_New ()
{
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::Directory);
    openDialog.setDirectory(mPreferencesDialogController.getPreferencesModel().getDefaultProjectDirectory());

    if(openDialog.exec())
    {
        mProjectDirectory = openDialog.selectedFiles().first();

        if (mProjectDirectory.size() == 0)
        {
            return;
        }

        bool createResult = mProjectDirectoryModel.createNewProjectTree(mProjectDirectory);
        ProjectDefinition* newDef = Project::createNewProjectDefinition(mProjectDirectoryModel.getProjectDirectoryName().toStdString());
        mProjectDirectoryModel.setProjectDefinitionHandle(newDef);
        bool projectFileResult = mProjectDirectoryModel.writeProjectFile();

        if (createResult && projectFileResult)
        {
            openProject();
        }
    }
}

void
MainController::onAction_File_Open
()
{
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::Directory);
    openDialog.setDirectory(mPreferencesDialogController.getPreferencesModel().getDefaultProjectDirectory());
    openDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(openDialog.exec())
    {
        mProjectDirectory = openDialog.selectedFiles().first();
    }

    if (mProjectDirectory.size() == 0)
    {
        return;
    }

    openProject();
}

void
MainController::onAction_File_Save
()
{
    bool saveResult = mProjectDirectoryModel.writeProjectFile();
    if(saveResult)
    {
        emit notifyStatusBarProjectLoaded(
                    QString::fromStdString(
                        "Successfuly Saved Project: " +
                        mDreamProjectModel->getProject()->getProjectDefinitionHandle()->getNameAndUuidString()
                        )
                    );
    }
    else
    {
        emit notifyStatusBarProjectLoaded(
                    QString::fromStdString(
                        "Error Saving Project: " +
                        mDreamProjectModel->getProject()->getProjectDefinitionHandle()->getNameAndUuidString()
                        )
                    );
    }
}

void
MainController::onAction_File_Close
()
{
    mSelectedProjectDefinitionHandle = nullptr;
    mSelectedAssetDefinitionHandle = nullptr;
    mSelectedSceneDefinitionHandle = nullptr;
    mSelectedSceneObjectDefinitionHandle = nullptr;

    mSelectionHighlighter->setSelectedSceneObjectRuntimeHandle(nullptr);

    mMainWindowHandle->getScenegraphTreeView()->setModel(nullptr);
    mScenegraphTreeModel.reset();

    mMainWindowHandle->getAssetDefinitionTreeView()->setModel(nullptr);
    mAssetDefinitionTreeModel.reset();

    mMainWindowHandle->getPropertiesTreeView()->setModel(nullptr);

    mDreamProjectModel->closeProject();

    setActionsEnabled_ValidProject(false);
}

// View Menu

void
MainController::onAction_View_ToggleGrid
(bool enabled)
{
    mWindowComponentHandle->setGridEnabled(enabled);
}

void
MainController::onAction_View_ToggleDebug
(bool enabled)
{
    mDreamProjectModel->setDebug(enabled);
}

void
MainController::onAction_View_TogglePhysicsDebug
(bool enable)
{
    mDreamProjectModel->setPhysicsDebug(enable);
}

void
MainController::onAction_View_ToggleSelectionHighlighter
(bool enabled)
{
    mWindowComponentHandle->setSelectionHighlighterEnabled(enabled);
}

// Scene Menu

void
MainController::onAction_Scene_NewScene
()
{
    auto log = spdlog::get("MainController");
    log->info( "onScenegraphAddSceneAction");
    Project* pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectDefinition* pdHandle = pHandle->getProjectDefinitionHandle();
        if (pdHandle)
        {
            SceneDefinition *sdHandle = pdHandle->createNewSceneDefinition();
            if (sdHandle)
            {
                onUI_ScenegraphUpdated();
            }
        }
    }
}

void
MainController::onAction_Scene_NewSceneObject
()
{
    auto log = spdlog::get("MainController");
    log->info( "onScenegraphAddSceneObjectAction");
    if (mSelectedSceneObjectDefinitionHandle)
    {
        mSelectedSceneObjectDefinitionHandle->createNewChildSceneObjectDefinition();
        onUI_ScenegraphUpdated();

    }
}

// Asset Menu

void
MainController::onAction_Asset_AddToSelectedSceneObjectDefinition
()
{
    auto log = spdlog::get("MainController");
    log->info( "onAction_Asset_AddToSelectedSceneObjectDefinition ");
    if (mSelectedSceneObjectDefinitionHandle && mSelectedAssetDefinitionHandle)
    {
        mSelectedSceneObjectDefinitionHandle->addAssetDefinitionUuidToLoadQueue
                (
                    mSelectedAssetDefinitionHandle->getUuid()
                    );
    }
}

void
MainController::onAction_Asset_NewDefinition_Animation
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Animation Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(ANIMATION))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Audio
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Audio Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(AUDIO))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Font
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Font Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(FONT))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Light
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Light Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(LIGHT))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Model
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Model Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(MODEL))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_PhysicsObject
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Physics Object Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(PHYSICS_OBJECT))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Script
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Script Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(SCRIPT))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Shader
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Shader Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(SHADER))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAction_Asset_NewDefinition_Sprite
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Sprite Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(SPRITE))
    {
        onUI_AssetDefinitionsUpdated();
    }
}

void
MainController::onAssetDefinitionProperty_ModelFile
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(QDir::home());

    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);

        log->info( "Using Assimp file {}", sourceFilePath.toStdString());

        bool fileExists = mProjectDirectoryModel.assetMainFileExists(adHandle);

        if (fileExists)
        {
            QString fileName = QFileInfo(sourceFile).fileName();
            auto result = QMessageBox::question
                    (
                        mMainWindowHandle,
                        "Overwrite Existing File?",
                        "An asset file all ready exists. Do you want to replace it?"
                        );
            if (result != QMessageBox::Yes)
            {
                log->info( "Copy of {} was cancelled", fileName.toStdString());
                return;
            }

            if (!mProjectDirectoryModel.deleteMainAssetFile(adHandle))
            {
                log->info( "Error, unable to delete main asset file for {}",
                           adHandle->getNameAndUuidString());
                return;
            }
        }

        bool copyResult = mProjectDirectoryModel.copyMainAssetFile(adHandle,sourceFile);

        log->info( "Copy {} "
                   , (copyResult ? "Success":"Failed"));
    }
}

void
MainController::onAssetDefinitionProperty_PhysicsBvhTriangleMeshFile
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(QDir::home());

    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);

        log->info( "Using BvhTriangleMesh file {}", sourceFilePath.toStdString());

        bool fileExists = mProjectDirectoryModel.assetMainFileExists(adHandle);

        if (fileExists)
        {
            QString fileName = QFileInfo(sourceFile).fileName();
            auto result = QMessageBox::question
                    (
                        mMainWindowHandle,
                        "Overwrite Existing File?",
                        "An asset file all ready exists. Do you want to replace it?"
                        );
            if (result != QMessageBox::Yes)
            {
                log->info( "Copy of {} was cancelled." , fileName.toStdString());
                return;
            }

            if (!mProjectDirectoryModel.deleteMainAssetFile(adHandle))
            {
                log->info( "Error, unable to delete main asset file for {}",
                           adHandle->getNameAndUuidString());
                return;
            }
        }

        bool copyResult = mProjectDirectoryModel.copyMainAssetFile(adHandle,sourceFile);

        log->info( "Copy {}", (copyResult ? "Success":"Failed"));
    }
}

void
MainController::onSceneProperty_CaptureCameraTranslation
(SceneDefinition* sdHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureCameraTranslation");
    Project *pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectRuntime *prHandle = pHandle->getProjectRuntimeHandle();
        if (prHandle)
        {
            sdHandle->getCameraTransform().setTranslation
                    (
                        prHandle->getCameraHandle()->getTranslation()
                        );
            return;
        }
        log->info( "CaptureCameraTranslation - No ProjectRuntime");
        return;
    }
    log->info( "CaptureCameraTranslation - No Project");
    return;
}

void
MainController::onSceneProperty_CaptureCameraRotation
(SceneDefinition* sdHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureCameraRotation");
    Project *pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectRuntime *prHandle = pHandle->getProjectRuntimeHandle();
        if (prHandle)
        {
            sdHandle->getCameraTransform().setRotation
                    (
                        prHandle->getCameraHandle()->getRotation()
                        );
            return;
        }
        log->info( "CaptureCameraRotation - No ProjectRuntime");
        return;
    }
    log->info( "CaptureCameraRotation - No Project");
    return;


}

void MainController::onSceneProperty_ChooseAmbientColour(SceneDefinition* sceneDef)
{
    auto log = spdlog::get("MainController");
    log->info( "Choose Ambient Colour");

    QColor currentColour;

    auto colour = sceneDef->getAmbientColour();
    currentColour.setRgbF(
                static_cast<double>(colour[0]), // r
            static_cast<double>(colour[1]), // g
            static_cast<double>(colour[2]), // b
            static_cast<double>(colour[3])  // a

            );

    auto chosenColour = QColorDialog::getColor(currentColour,mMainWindowHandle,"Ambient Colour",QColorDialog::ShowAlphaChannel);

    if (chosenColour.isValid())
    {
        sceneDef->setAmbientColourR(static_cast<float>(chosenColour.redF()));
        sceneDef->setAmbientColourG(static_cast<float>(chosenColour.greenF()));
        sceneDef->setAmbientColourB(static_cast<float>(chosenColour.blueF()));
        sceneDef->setAmbientColourA(static_cast<float>(chosenColour.alphaF()));
    }

}

void MainController::onSceneProperty_ChooseClearColour(SceneDefinition* sceneDef)
{
    auto log = spdlog::get("MainController");
    log->info( "Choose Clear Colour");
    QColor currentColour;
    auto colour = sceneDef->getClearColour();
    currentColour.setRgbF(
                static_cast<double>(colour[0]), // r
            static_cast<double>(colour[1]), // g
            static_cast<double>(colour[2]) // b
            );

    auto chosenColour = QColorDialog::getColor(currentColour,mMainWindowHandle,"Clear Colour");

    if (chosenColour.isValid())
    {
        sceneDef->setClearColourR(static_cast<float>(chosenColour.redF()));
        sceneDef->setClearColourG(static_cast<float>(chosenColour.greenF()));
        sceneDef->setClearColourB(static_cast<float>(chosenColour.blueF()));
    }
}

void
MainController::onAssetDefinitionProperty_ModelAdditionalFiles
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFiles);
    openDialog.setDirectory(QDir::home());

    if(openDialog.exec())
    {
        for (QString sourceFilePath : openDialog.selectedFiles())
        {
            QFile sourceFile(sourceFilePath);

            log->info( "Using Additional file {}",
                       sourceFilePath.toStdString());

            bool copyResult = mProjectDirectoryModel.copyAdditionalFile(adHandle,sourceFile);

            log->info( "Copy ",
                       (copyResult ? "Success":"Failed"));
        }
    }
}

// UI Signal/Slot Handlers ======================================================
void
MainController::onUI_SelectedSceneChanged
(SceneDefinition *scene)
{
    auto log = spdlog::get("MainController");
    mDreamProjectModel->setSelectedSceneDefinitionHandle(scene);
    mMainWindowHandle->showStatusBarMessage(
                QString("Selected Scene: %1").
                arg(QString::fromStdString(scene->getName())
                    )
                );
}

void
MainController::onUI_ScenegraphUpdated
()
{
    auto log = spdlog::get("MainController");
    log->info( "updating scenegraph tree model");
    mScenegraphTreeModel->setupModelData();
    mMainWindowHandle->getScenegraphTreeView()->expandAll();
}

void
MainController::onUI_AssetDefinitionsUpdated
()
{
    auto log = spdlog::get("MainController");
    log->info( "updating scenegraph tree model");
    mAssetDefinitionTreeModel->setupModelData();
    //mMainWindowHandle->getAssetDefinitionTreeView()->expandAll();
}

void
MainController::onUI_ProjectNameChanged
(QString name)
{
    auto log = spdlog::get("MainController");
    log->info( "Name set to {}" , name.toStdString());
    mDreamProjectModel->setProjectName(name.toStdString());
}

void
MainController::onUI_ProjectAuthorChanged
(QString author)
{
    auto log = spdlog::get("MainController");
    log->info( "Author set to " , author.toStdString());
    mDreamProjectModel->setProjectAuthor(author.toStdString());
}

void
MainController::onUI_ProjectDescriptionChanged
(QString desc)
{
    auto log = spdlog::get("MainController");
    log->info( "Description set to " , desc.toStdString());
    mDreamProjectModel->setProjectDescription(desc.toStdString());
}

void
MainController::onUI_ProjectWindowWidthChanged
(QString width)
{
    auto log = spdlog::get("MainController");
    log->info( "Window Width set to " , width.toStdString());
    mDreamProjectModel->setProjectWindowWidth(width.toInt());
}

void
MainController::onUI_ProjectWindowHeightChanged
(QString height)
{
    auto log = spdlog::get("MainController");
    log->info( "Window Height set to {}" , height.toStdString());
    mDreamProjectModel->setProjectWindowHeight(height.toInt());
}

void
MainController::onUI_ProjectStartupSceneChanged
(QString startupSceneIndex)
{
    auto log = spdlog::get("MainController");
    string sceneName = getSceneNameFromModelIndex(startupSceneIndex.toInt());
    log->info( "startupScene set to {} {}" , startupSceneIndex.toStdString() ,  sceneName);
    SceneDefinition* sceneHandle = mDreamProjectModel->getProject()
            ->getProjectDefinitionHandle()
            ->getSceneDefinitionHandleByUuid(sceneName);
    mDreamProjectModel->setProjectStartupSceneByUuid(sceneHandle->getUuid());
}

// Object Handle Getters ========================================================

RelationshipTree*
MainController::getRelationshipTreeHandle
()
{
    return mRelationshipTree.get();
}

void
MainController::onAction_Preferences
()
{
    mPreferencesDialogController.showDialog();
}

Grid*
MainController::getGridHandle
()
{
    return mGrid.get();
}

SelectionHighlighter*
MainController::getSelectionHighlighterHandle
()
{
    return mSelectionHighlighter.get();
}

// Misc =========================================================================

void
MainController::openProject
()
{
    auto log = spdlog::get("MainController");
    bool loadResult = mDreamProjectModel->loadProject(mProjectDirectory);

    mProjectDirectoryModel.setProjectDefinitionHandle
            (
                mDreamProjectModel->getProject()->getProjectDefinitionHandle()
                );

    mProjectDirectoryModel.inflateFromDirectory(mProjectDirectory);

    log->info( "Load Project Result {}", loadResult);
    if (!loadResult)
    {
        emit notifyInvalidProjectDirectory(mProjectDirectory);
        emit notifyProjectWidgetsEnabledChanged(false);
        return;
    }

    mMainWindowHandle->clearOpenGLComponentRuntime();

    updateWindowTitle(mProjectDirectory);

    ProjectDefinition *currentProject = mDreamProjectModel->getProject()->getProjectDefinitionHandle();

    emit notifyProjectNameChanged(QString::fromStdString(currentProject->getName()));
    emit notifyProjectAuthorChanged(QString::fromStdString(currentProject->getAuthor()));
    emit notifyProjectDescriptionChanged(QString::fromStdString(currentProject->getDescription()));
    emit notifyProjectWindowWidthChanged(currentProject->getWindowWidth());
    emit notifyProjectWindowHeightChanged(currentProject->getWindowHeight());
    emit notifyProjectSceneListChanged(getSceneNamesListModel(currentProject->getSceneDefinitionsHandleList()));
    emit notifyProjectWidgetsEnabledChanged(true);

    mScenegraphTreeModel.reset(new ScenegraphTreeModel(currentProject,mMainWindowHandle->getScenegraphTreeView()));
    mMainWindowHandle->getScenegraphTreeView()->setModel(mScenegraphTreeModel.get());

    mAssetDefinitionTreeModel.reset(new AssetDefinitionTreeModel(currentProject,mMainWindowHandle->getAssetDefinitionTreeView()));
    mMainWindowHandle->getAssetDefinitionTreeView()->setModel(mAssetDefinitionTreeModel.get());

    emit notifyStatusBarProjectLoaded(
                QString::fromStdString(
                    "Successfuly Loaded Project: " +
                    currentProject->getName() + " (" +
                    currentProject->getUuid() + ")"
                    )
                );

    setActionsEnabled_ValidProject(true);
    connectUI_TreeViewModels();
}

string
MainController::getSceneNameFromModelIndex
(int index)
{
    return mSceneListModel->stringList().at(index).toStdString();
}

void
MainController::onAction_Debug_DumpProjectDefinitionJson
(bool toggled)
{
    Q_UNUSED(toggled)
    cout << mDreamProjectModel
            ->getProject()
            ->getProjectDefinitionHandle()
            ->getJson().dump(1) << endl;
    ;
}

void
MainController::onAssetDefinitionProperty_AudioFile
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(QDir::home());

    QStringList filters;
    filters << "*.wav" << "*.ogg";
    openDialog.setNameFilters(filters);

    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);

        log->info( "Using Audio file", sourceFilePath.toStdString());

        bool fileExists = mProjectDirectoryModel.assetMainFileExists(adHandle);

        if (fileExists)
        {
            QString fileName = QFileInfo(sourceFile).fileName();
            auto result = QMessageBox::question
                    (
                        mMainWindowHandle,
                        "Overwrite Existing File?",
                        "An asset file all ready exists. Do you want to replace it?"
                        );
            if (result != QMessageBox::Yes)
            {
                log->info( "Copy of {} was cancelled" , fileName.toStdString());
                return;
            }

            if (!mProjectDirectoryModel.deleteMainAssetFile(adHandle))
            {
                log->info( "Error, unable to delete main asset file for {}",
                           adHandle->getNameAndUuidString());
                return;
            }
        }

        bool copyResult = mProjectDirectoryModel.copyMainAssetFile(adHandle,sourceFile);

        log->info( "Copy ",
                   (copyResult ? "Success":"Failed"));

        showImportResultDialog(copyResult, adHandle, sourceFile.fileName());

    }
}

void
MainController::showImportResultDialog(bool success, IAssetDefinition* adHandle, QString source)
{
    if (!success)
    {
        ;
        QMessageBox::critical
                (
                    mMainWindowHandle,
                    QString("Import Failed"),
                    QString("Unable to import\n\n%1\n\nnto\n\n%2")
                    .arg(source,QString::fromStdString(adHandle->getAssetPath()))
                    );
    }
    else
    {
        QMessageBox::information
                (
                    mMainWindowHandle,
                    QString("Import Success"),
                    QString("Successfuly imported\n\n%1\n\nto\n\n%2")
                    .arg(source,QString::fromStdString(adHandle->getAssetPath()))
                    );
    }
}

void
MainController::onAssetDefinitionProperty_FontFile
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(QDir::home());

    QStringList filters;
    filters << "*.ttf";
    openDialog.setNameFilters(filters);


    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);

        log->info( "Using Font file {}", sourceFilePath.toStdString());

        bool fileExists = mProjectDirectoryModel.assetMainFileExists(adHandle);

        if (fileExists)
        {
            QString fileName = QFileInfo(sourceFile).fileName();
            auto result = QMessageBox::question
                    (
                        mMainWindowHandle,
                        "Overwrite Existing File?",
                        "An asset file all ready exists. Do you want to replace it?"
                        );
            if (result != QMessageBox::Yes)
            {
                log->info( "Copy of {} was cancelled" , fileName.toStdString());
                return;
            }

            if (!mProjectDirectoryModel.deleteMainAssetFile(adHandle))
            {
                log->info( "Error, unable to delete main asset file for {}"
                           ,adHandle->getNameAndUuidString()
                           );
                return;
            }
        }

        bool copyResult = mProjectDirectoryModel.copyMainAssetFile(adHandle,sourceFile);

        log->info( "Copy {} ",
                   (copyResult ? "Success":"Failed")
                   );
    }
}

void
MainController::onAssetDefinitionProperty_RemoveFiles
(IAssetDefinition* adHandle)
{
    auto result = QMessageBox::question
            (
                mMainWindowHandle,
                "Remove Asset Data?",
                "Are you sure you want to delete the asset data?"
                );

    if (result == QMessageBox::Yes)
    {
        mProjectDirectoryModel.deleteAssetDataDirectory(adHandle);
        return;
    }
}

void
MainController::onAssetDefinitionProperty_EditScript
(IAssetDefinition* adHandle)
{
    mScriptEditor.openScript(dynamic_cast<ScriptDefinition*>(adHandle));
}

void
MainController::onAssetDefinitionProperty_EditShader
(IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "EditShader");
    mScriptEditor.openShader(dynamic_cast<ShaderDefinition*>(adHandle));
}

void
MainController::onAssetDefinitionProperty_ScriptTemplateChanged
(IAssetDefinition* adHandle, const QString& templateName)
{
    auto log = spdlog::get("MainController");
    log->info( "Script Template Changed");

    auto result = QMessageBox::question
            (
                mMainWindowHandle,
                "Load Script Template",
                "Are you sure you want to load this template? Any existing data will be lost."
                );

    if (result == QMessageBox::Yes)
    {
        QString scriptContent = mTemplatesModel.getScriptTemplate(templateName);
        mProjectDirectoryModel.writeAssetData(scriptContent,adHandle);
        return;
    }
}

void
MainController::onAssetDefinitionProperty_ShaderTemplateChanged
(IAssetDefinition* adHandle, const QString& templateName)
{
    auto log = spdlog::get("MainController");
    log->info( "Shader Template Changed");

    QString fragmentContent = mTemplatesModel.getShaderTemplate
            (
                templateName,
                QString::fromStdString(Constants::SHADER_FRAGMENT_FILE_NAME)
                );

    mProjectDirectoryModel.writeAssetData
            (
                fragmentContent,
                adHandle,
                QString::fromStdString(Constants::SHADER_FRAGMENT_FILE_NAME),
                true
                );

    QString vertexContent = mTemplatesModel.getShaderTemplate
            (
                templateName,
                QString::fromStdString(Constants::SHADER_VERTEX_FILE_NAME)
                );

    mProjectDirectoryModel.writeAssetData
            (
                vertexContent,
                adHandle,
                QString::fromStdString(Constants::SHADER_VERTEX_FILE_NAME),
                true
                );
}

void
MainController::onSceneObjectProperty_CaptureTranslation
(SceneObjectDefinition* sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureTranslation");
    Project *pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectRuntime *prHandle = pHandle->getProjectRuntimeHandle();
        if (prHandle)
        {
            SceneRuntime *srHandle = prHandle->getActiveSceneRuntimeHandle();
            if (srHandle)
            {
                SceneObjectRuntime *sorHandle;
                sorHandle = srHandle->getSceneObjectRuntimeHandleByUuid(sodHandle->getUuid());
                if (sorHandle)
                {
                    sodHandle->getTransform().setTranslation(sorHandle->getTranslation());
                }
            }
        }
    }
}

void
MainController::onSceneObjectProperty_CaptureRotation
(SceneObjectDefinition* sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureRotation");
    Project *pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectRuntime *prHandle = pHandle->getProjectRuntimeHandle();
        if (prHandle)
        {
            SceneRuntime *srHandle = prHandle->getActiveSceneRuntimeHandle();
            if (srHandle)
            {
                SceneObjectRuntime *sorHandle;
                sorHandle = srHandle->getSceneObjectRuntimeHandleByUuid(sodHandle->getUuid());
                if (sorHandle)
                {
                    sodHandle->getTransform().setRotation(sorHandle->getRotation());
                }
            }
        }
    }
}

void
MainController::onSceneObjectProperty_CaptureScale
(SceneObjectDefinition* sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureScale");
    Project *pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        ProjectRuntime *prHandle = pHandle->getProjectRuntimeHandle();
        if (prHandle)
        {
            SceneRuntime *srHandle = prHandle->getActiveSceneRuntimeHandle();
            if (srHandle)
            {
                SceneObjectRuntime *sorHandle;
                sorHandle = srHandle->getSceneObjectRuntimeHandleByUuid(sodHandle->getUuid());
                if (sorHandle)
                {
                    sodHandle->getTransform().setScale(sorHandle->getScale());
                }
            }
        }
    }

}

void
MainController::onSceneObjectProperty_RemoveAsset
(SceneObjectDefinition* sodHandle,IAssetDefinition* adHandle)
{
    auto log = spdlog::get("MainController");
    log->info(
                "RemoveAsset {} from {}",
                adHandle->getNameAndUuidString(),
                sodHandle->getNameAndUuidString()
                );
    sodHandle->removeAssetDefinitionFromLoadQueue(adHandle);
}

void
MainController::onSceneObjectProperty_RemoveChild
(SceneObjectDefinition* sodHandle, SceneObjectDefinition* sodChildHandle)
{
    auto log = spdlog::get("MainController");
    log->info(
                "RemoveChild {} from {}",
                sodChildHandle->getNameAndUuidString(),
                sodHandle->getNameAndUuidString()
                );
    sodHandle->removeChildSceneObjectDefinition(sodChildHandle);
    onUI_ScenegraphUpdated();
}
