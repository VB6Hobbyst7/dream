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

#define _USE_MATH_DEFINES
#include <math.h>

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
#include <spdlog/sinks/stdout_color_sinks.h>

using std::cout;

// Constructors/Destructors =====================================================

MainController::MainController
(MainWindowController* parent)
    : QObject(parent),
      mMainWindowHandle(parent),
      mWindowComponent(parent->getWindowComponent()),
      mLastDirectory(QDir::home())
{
    auto log = spdlog::get("MainController");

    if (!log)
    {
        log = spdlog::stdout_color_mt("MainController");
    }

    log->info( "Constructing Object");

    mDreamProjectModel.reset(new DreamProjectModel(this,mWindowComponent));

    mMacOsOpenModel.reset(
        new MacOSOpenModel(
            &mPreferencesDialogController.getPreferencesModel(),this
        )
    );

    setupUI();
    setupConnections();

    mScriptEditor.setProjectDirectoryModelHandle(&mProjectDirectoryModel);
    mScriptEditor.setTemplatesModelHandle(&mTemplatesModel);
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
    connect(
        mMainWindowHandle,
        SIGNAL(notifyCreateNewAssetDefinition(QString)),
        this,
        SLOT(onCreateNewAssetDefinition(QString))
    );
    connect(
        mMainWindowHandle,
        SIGNAL(notifyScenegraphTreeDataChanged()),
        this,
        SLOT(onScenegraphTreeDataChanged())
    );

     connect(
        mMainWindowHandle,
        SIGNAL(notifyAssetDefinitionTreeDataChanged()),
        this,
        SLOT(onAssetDefinitionTreeDataChanged())
    );
    connect(
        mMainWindowHandle,
        SIGNAL(notifyPropertiesTreeDataChanged()),
        this,
        SLOT(onPropertiesTreeDataChanged())
    );
    connect(
        this, SIGNAL(notifyProjectDefinitionChanged(shared_ptr<ProjectDefinition>)),
        mMainWindowHandle, SLOT(onProjectDefinitionChanged(shared_ptr<ProjectDefinition>))
    );
    connect
    (
        mMainWindowHandle, SIGNAL(notifyMainVolumeChanged(int)),
        this, SLOT(onMainVolumeChanged(int))
    );
    // Path List Form
    connectPathEditorSignals();
}

void MainController::connectPathEditorSignals()
{
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifyTableChanged()),
        mPathPointViewer.get(),
        SLOT(onUpdateRequested())
    );
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifySelectedRowChanged(int)),
        mPathPointViewer.get(),
        SLOT(onSelectionChanged(int))
    );
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifyCloseEvent()),
        this,
        SLOT(onPathEditorClosed())
    );
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifyPathVisibilityChanged(bool)),
        this,
        SLOT(onPathVisibilityChanged(bool))
    );
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifyTangentVisibilityChanged(bool)),
        mPathPointViewer.get(),
        SLOT(onTangentVisibilityChanged(bool))
    );
    connect
    (
        &mPathEditorFormController,
        SIGNAL(notifyTangentIndexChanged(int)),
        mPathPointViewer.get(),
        SLOT(onTangentIndexChanged(int))
    );
    connect(
        mPathPointViewer.get(),
        SIGNAL(notifyNumberOfTangentsChanged(int)),
        &mPathEditorFormController,
        SLOT(onNumberOfTangentsChanged(int))
    );
}

void
MainController::setupUI_GLWidgets
()
{
    mGrid.reset(new Grid(this));
    mWindowComponent->setGridHandle(getGridHandle());

    mSelectionHighlighter.reset(new SelectionHighlighter(this));
    mWindowComponent->setSelectionHighlighterHandle(getSelectionHighlighterHandle());

    mRelationshipTree.reset(new RelationshipTree(this));
    mWindowComponent->setRelationshipTreeHandle(getRelationshipTreeHandle());

    mPathPointViewer.reset(new PathPointViewer(this));
    mWindowComponent->setPathPointViewerHandle(getPathPointViewerHandle());
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
            mPropertiesModel.reset(
                new AssetDefinitionPropertiesModel(
                    mSelectedAssetDefinitionHandle,
                    propertiesTreeView
                )
            );

            // Audio File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_AudioFile(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_AudioFile(shared_ptr<IAssetDefinition> ))
                        );

            // Font File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_FontFile(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_FontFile(shared_ptr<IAssetDefinition> ))
                        );

            // Model File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_ModelFile(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_ModelFile(shared_ptr<IAssetDefinition> ))
                        );

            // Model Additional Files
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_ModelAdditionalFiles(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_ModelAdditionalFiles(shared_ptr<IAssetDefinition> ))
                        );

            // Remove Files
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveFiles(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_RemoveFiles(shared_ptr<IAssetDefinition> ))
                        );
            // Model Material/Shader Map
            connect
            (
                mPropertiesModel.get(),
                SIGNAL(notifyButton_ModelMaterialShaderMap(shared_ptr<IAssetDefinition> )),
                this,
                SLOT(onAssetDefinitionProperty_ModelMaterialShaderMap(shared_ptr<IAssetDefinition> ))
            );

            // Edit Script
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_EditScript(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_EditScript(shared_ptr<IAssetDefinition> ))
                        );

            // Edit Vertex Shader
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_EditShader(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_EditShader(shared_ptr<IAssetDefinition> ))
                        );

            // Script Template
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyCombo_ScriptTemplateChanged(shared_ptr<IAssetDefinition> ,QString)),
                        this,
                        SLOT(onAssetDefinitionProperty_ScriptTemplateChanged(shared_ptr<IAssetDefinition> ,QString))
                        );

            // Shader Template
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyCombo_ShaderTemplateChanged(shared_ptr<IAssetDefinition> ,QString)),
                        this,
                        SLOT(onAssetDefinitionProperty_ShaderTemplateChanged(shared_ptr<IAssetDefinition> ,QString))
                        );

            // PhysicsObject Mesh File
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_PhysicsBvhTriangleMeshFile(shared_ptr<IAssetDefinition> )),
                        this,
                        SLOT(onAssetDefinitionProperty_PhysicsBvhTriangleMeshFile(shared_ptr<IAssetDefinition> ))
                        );

            // Light Colour
            connect
            (
                mPropertiesModel.get(),
                SIGNAL(notifyButton_LightChooseAmbient(shared_ptr<IAssetDefinition> )),
                this,
                SLOT(onAssetDefinitionProperty_LightChooseAmbient(shared_ptr<IAssetDefinition> ))
            );
            connect
            (
                mPropertiesModel.get(),
                SIGNAL(notifyButton_LightChooseDiffuse(shared_ptr<IAssetDefinition> )),
                this,
                SLOT(onAssetDefinitionProperty_LightChooseDiffuse(shared_ptr<IAssetDefinition> ))
            );
            connect
            (
                mPropertiesModel.get(),
                SIGNAL(notifyButton_LightChooseSpecular(shared_ptr<IAssetDefinition> )),
                this,
                SLOT(onAssetDefinitionProperty_LightChooseSpecular(shared_ptr<IAssetDefinition> ))
            );
            // Path List
            connect
            (
                mPropertiesModel.get(),
                SIGNAL(notifyButton_PathList(shared_ptr<IAssetDefinition> )),
                this,
                SLOT(onAssetDefinitionProperty_PathList(shared_ptr<IAssetDefinition> ))
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

    mPropertiesModel.reset(nullptr);

    switch(item->getType())
    {
        case ScenegraphTreeItemType::SCENEGRAPH_PROJECT:
            log->info( "Selected a project");
            mSelectedProjectDefinitionHandle = mDreamProjectModel->getProject()->getProjectDefinition();
            mPropertiesModel.reset(new ProjectPropertiesModel(mSelectedProjectDefinitionHandle,propertiesTreeView));
            emit notifyProjectDefinitionChanged(mSelectedProjectDefinitionHandle);
            break;

        case ScenegraphTreeItemType::SCENEGRAPH_SCENE:
            log->info( "Selected a scene");
            mSelectedSceneDefinitionHandle = dynamic_pointer_cast<SceneDefinition>(
                    static_cast<ScenegraphTreeItem*>(item)
                        ->getItem()
            );
            mDreamProjectModel->setSelectedSceneDefinition(mSelectedSceneDefinitionHandle);

            if (mSelectedSceneDefinitionHandle != nullptr)
            {
                mPropertiesModel.reset(new ScenePropertiesModel(mSelectedSceneDefinitionHandle,propertiesTreeView));
            }


            // Capture Camera Translation
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureCameraTranslation(shared_ptr<SceneDefinition> )),
                        this,
                        SLOT(onSceneProperty_CaptureCameraTranslation(shared_ptr<SceneDefinition> ))
                        );

            // Capture Camera Orientation
            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureCameraLookAt(shared_ptr<SceneDefinition> )),
                        this,
                        SLOT(onSceneProperty_CaptureCameraLookAt(shared_ptr<SceneDefinition> ))
                        );

            // Choose Clear Colour
            connect
                    (
                        mPropertiesModel.get(), SIGNAL(notifyButton_ChooseClearColour(shared_ptr<SceneDefinition> )),
                        this, SLOT(onSceneProperty_ChooseClearColour(shared_ptr<SceneDefinition> ))
                        );
            // Choose Ambient Colour
            connect
                    (
                        mPropertiesModel.get(), SIGNAL(notifyButton_ChooseAmbientColour(shared_ptr<SceneDefinition> )),
                        this, SLOT(onSceneProperty_ChooseAmbientColour(shared_ptr<SceneDefinition> ))
                        );
            break;

        case ScenegraphTreeItemType::SCENEGRAPH_SCENE_OBJECT:
            log->info( "Selected a scene object");
            mSelectedSceneObjectDefinitionHandle = dynamic_pointer_cast<SceneObjectDefinition>(
                static_cast<ScenegraphTreeItem*>(item)->getItem()
            );

            mPropertiesModel.reset(
                new SceneObjectPropertiesModel(mSelectedSceneObjectDefinitionHandle,propertiesTreeView)
            );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureTranslation(shared_ptr<SceneObjectDefinition>)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureTranslation(shared_ptr<SceneObjectDefinition>))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureOrientation(shared_ptr<SceneObjectDefinition>)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureOrientation(shared_ptr<SceneObjectDefinition>))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_CaptureScale(shared_ptr<SceneObjectDefinition>)),
                        this,
                        SLOT(onSceneObjectProperty_CaptureScale(shared_ptr<SceneObjectDefinition>))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveAsset(shared_ptr<SceneObjectDefinition>,shared_ptr<IDefinition> )),
                        this,
                        SLOT(onSceneObjectProperty_RemoveAsset(shared_ptr<SceneObjectDefinition>,shared_ptr<IDefinition> ))
                        );

            connect
                    (
                        mPropertiesModel.get(),
                        SIGNAL(notifyButton_RemoveChild(shared_ptr<SceneObjectDefinition>,shared_ptr<IDefinition>)),
                        this,
                        SLOT(onSceneObjectProperty_RemoveChild(shared_ptr<SceneObjectDefinition>,shared_ptr<IDefinition>))
                        );

            if (mSelectionHighlighter)
            {
                if (mDreamProjectModel->getProject()->hasProjectRuntime())
                {
                    shared_ptr<ProjectRuntime> prHandle = mDreamProjectModel->getProject()->getProjectRuntime();
                    if (prHandle != nullptr && prHandle->hasActiveSceneRuntime())
                    {
                        shared_ptr<SceneRuntime> sceneRuntime = mDreamProjectModel
                                ->getProject()
                                ->getProjectRuntime()
                                ->getActiveSceneRuntime();

                        if (sceneRuntime != nullptr)
                        {
                            auto selected = sceneRuntime->getSceneObjectRuntimeByUuid(mSelectedSceneObjectDefinitionHandle->getUuid());
                            mSelectionHighlighter->setSelectedSceneObjectRuntimeHandle(selected);
                        }
                    }
                }
            }
            else
            {
                log->error("Selection Highlighter is nullptr");
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
    // Connect Control Scene
    connect
    (
        mMainWindowHandle->getAction_ControlScene(),
        SIGNAL(toggled(bool)),
        this,
        SLOT(onAction_ControlSceneTriggered(bool))
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

    // Asset Menu -> New Definition -> Path
    connect
            (
                mMainWindowHandle->getAction_Asset_NewDefinition(AssetType::PATH),
                SIGNAL(triggered()),
                this,
                SLOT(onAction_Asset_NewDefinition_Path())
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
                mDreamProjectModel.get(), SIGNAL(notifySelectedSceneChanged(shared_ptr<SceneDefinition> )),
                this, SLOT(onUI_SelectedSceneChanged(shared_ptr<SceneDefinition> ))
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
     connect(
        mScenegraphTreeModel.get(),
        SIGNAL(notifyExpandRequested()),
        mMainWindowHandle,
        SLOT(onScenegraphTreeExpandRequested())
    );

    // assetDefinitionTreeView
    connect
            (
                mMainWindowHandle->getAssetDefinitionTreeView()->selectionModel(),
                SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                this, SLOT(onUI_AssetDefinitionTreeViewSelectionChanged(const QItemSelection&,const QItemSelection&))
                );

    mMainWindowHandle->getScenegraphTreeView()->expandAll();
    mMainWindowHandle->getAssetDefinitionTreeView()->expandAll();
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
(vector<shared_ptr<SceneDefinition>> sceneList)
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
    shared_ptr<SceneDefinition> sceneDefinitionHandle = mDreamProjectModel->getSelectedSceneDefinition();
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
    mDreamProjectModel->stopActiveSceneRuntime();
}

void MainController::onAction_ControlSceneTriggered(bool activated)
{
    auto log = spdlog::get("MainController");
    log->critical("Scene Control Enabled {}",activated);
   mWindowComponent->setControlScene(activated);
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
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::Directory);
    openDialog.setLabelText(QFileDialog::Accept, "Create Project");
    openDialog.setDirectory(mPreferencesDialogController.getPreferencesModel().getDefaultProjectDirectory());

    if(openDialog.exec())
    {
        mProjectDirectory = openDialog.selectedFiles().first();
        mLastDirectory = (mProjectDirectory);

        if (mProjectDirectory.size() == 0)
        {
            return;
        }

        if(!QDir(mProjectDirectory).isEmpty())
        {
            auto result = QMessageBox::question
            (
                mMainWindowHandle,
                "Directory not empty",
                "The chosen directory is not empty.\n\n Are you sure you want to create a project here?"
            );

            if (result != QMessageBox::Yes)
            {
                log->info( "Project creation was was cancelled for ", mProjectDirectory.toStdString());
                return;
            }
        }

        bool createResult = mProjectDirectoryModel.createNewProjectTree(mProjectDirectory);
        shared_ptr<ProjectDefinition> newDef = Project::createNewProjectDefinition(mProjectDirectoryModel.getProjectDirectoryName().toStdString());
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
    openDialog.setLabelText(QFileDialog::Accept, "Open Project");
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

    mLastDirectory = QDir(mProjectDirectory);

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
                        mDreamProjectModel->getProject()->getProjectDefinition()->getNameAndUuidString()
                        )
                    );
    }
    else
    {
        emit notifyStatusBarProjectLoaded(
                    QString::fromStdString(
                        "Error Saving Project: " +
                        mDreamProjectModel->getProject()->getProjectDefinition()->getNameAndUuidString()
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
    mWindowComponent->setGridEnabled(enabled);
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
    mWindowComponent->setSelectionHighlighterEnabled(enabled);
}

// Scene Menu

void
MainController::onAction_Scene_NewScene
()
{
    auto log = spdlog::get("MainController");
    log->info( "onScenegraphAddSceneAction");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        shared_ptr<ProjectDefinition> pdHandle = pHandle->getProjectDefinition();
        if (pdHandle)
        {
            shared_ptr<SceneDefinition> sdHandle = pdHandle->createNewSceneDefinition();
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
MainController::onAction_Asset_NewDefinition_Path
()
{
    auto log = spdlog::get("MainController");
    log->info( "Creating new Path Asset Definition");
    if (mDreamProjectModel->createNewAssetDefinition(PATH))
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
(shared_ptr<IAssetDefinition> adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(mLastDirectory);

    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);
        mLastDirectory.setPath(sourceFilePath);

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
        dynamic_pointer_cast<ModelDefinition>(adHandle)->clearMaterialShaderList();

        log->info( "Copy {} ", (copyResult ? "Success":"Failed"));
    }
}

void
MainController::onAssetDefinitionProperty_PhysicsBvhTriangleMeshFile
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(mLastDirectory);

    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);
        mLastDirectory.setPath(sourceFilePath);

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
(shared_ptr<SceneDefinition>  sdHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureCameraTranslation");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();

    if (pHandle != nullptr)
    {
        shared_ptr<ProjectRuntime> prHandle = pHandle->getProjectRuntime();
        if (prHandle != nullptr)
        {
            auto camera = prHandle->getCamera();
            if ( camera == nullptr)
            {
                return;
            }

            auto translation = camera->getTranslation();
            sdHandle->setCameraTranslation(translation);
            mMainWindowHandle->showStatusBarMessage(
                QString("Captured Camera Translation x:%1, y:%2, z:%3")
                        .arg(QString::number(translation.x, 'G', 5))
                        .arg(QString::number(translation.y, 'G', 5))
                        .arg(QString::number(translation.z, 'G', 5))
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
MainController::onSceneProperty_CaptureCameraLookAt
(shared_ptr<SceneDefinition>  sdHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureCameraLookAt");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();

    if (pHandle != nullptr)
    {
        shared_ptr<ProjectRuntime> prHandle = pHandle->getProjectRuntime();
        if (prHandle == nullptr)
        {
            auto camera = prHandle->getCamera();
            if ( camera == nullptr)
            {
                return;
            }
            auto pitch = camera->getPitch();
            auto yaw   = camera->getYaw();

            sdHandle->setCameraPitch(pitch);
            sdHandle->setCameraYaw(yaw);
            mMainWindowHandle->showStatusBarMessage(
                QString("Captured Camera LookAt pitch: %1, yaw:%2")
                        .arg(QString::number(pitch, 'G', 5))
                        .arg(QString::number(yaw, 'G', 5))
            );
            return;
        }
        log->info( "CaptureCameraLookAt - No ProjectRuntime");
        return;
    }
    log->info( "CaptureCameraLookAt - No Project");
    return;
}



void MainController::onSceneProperty_ChooseAmbientColour(shared_ptr<SceneDefinition>  sceneDef)
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

void MainController::onSceneProperty_ChooseClearColour(shared_ptr<SceneDefinition>  sceneDef)
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
MainController::onCreateNewAssetDefinition
(QString type)
{
    auto log = spdlog::get("MainController");
    log->info("Creating new asset definition {}",type.toStdString());
    AssetType assetType = Constants::getAssetTypeEnumFromString(type.toStdString());

    switch(assetType)
    {
       case PATH:
            onAction_Asset_NewDefinition_Path();
            break;
        case AUDIO:
            onAction_Asset_NewDefinition_Audio();
            break;
        case FONT:
            onAction_Asset_NewDefinition_Font();
            break;
        case LIGHT:
            onAction_Asset_NewDefinition_Light();
            break;
        case MODEL:
            onAction_Asset_NewDefinition_Model();
            break;
        case PHYSICS_OBJECT:
            onAction_Asset_NewDefinition_PhysicsObject();
            break;
        case SCRIPT:
            onAction_Asset_NewDefinition_Script();
            break;
        case SHADER:
            onAction_Asset_NewDefinition_Shader();
            break;
        case SPRITE:
            onAction_Asset_NewDefinition_Sprite();
            break;
        case NONE:
            break;
    }
}

void MainController::onMainVolumeChanged(int vol)
{
   auto log = spdlog::get("MainController");
   if (mDreamProjectModel != nullptr)
   {
       auto project = mDreamProjectModel->getProject();
       if (project != nullptr)
       {
           shared_ptr<ProjectRuntime> runtime = project->getProjectRuntime();
           if (runtime != nullptr)
           {
               shared_ptr<AudioComponent> audio = runtime->getAudioComponent();
               if (audio != nullptr)
               {
                   auto volume = vol/100.0f;
                   log->info("Setting volume to {}",volume);
                   audio->setVolume(volume);
               }
           }
       }
   }
}

void MainController::onPathEditorClosed()
{
    mPathPointViewer->setPathDefinition(nullptr);
}

void MainController::onPathVisibilityChanged(bool visible)
{
   mPathPointViewer->setPathVisible(visible);
}

void
MainController::onAssetDefinitionProperty_ModelAdditionalFiles
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFiles);
    openDialog.setDirectory(mLastDirectory);

    if(openDialog.exec())
    {
        for (QString sourceFilePath : openDialog.selectedFiles())
        {
            QFile sourceFile(sourceFilePath);
            mLastDirectory.setPath(sourceFilePath);

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
(shared_ptr<SceneDefinition> scene)
{
    auto log = spdlog::get("MainController");
    mDreamProjectModel->setSelectedSceneDefinition(scene);
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
    mMainWindowHandle->getAssetDefinitionTreeView()->expandAll();
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
    shared_ptr<SceneDefinition>  sceneHandle = mDreamProjectModel->getProject()
            ->getProjectDefinition()
            ->getSceneDefinitionByUuid(sceneName);
    mDreamProjectModel->setProjectStartupSceneByUuid(sceneHandle->getUuid());
}

// Object Handle Getters ========================================================

RelationshipTree*
MainController::getRelationshipTreeHandle
()
{
    return mRelationshipTree.get();
}

PathPointViewer* MainController::getPathPointViewerHandle()
{
   return mPathPointViewer.get();
}

void MainController::forceScenegraphTreeDataChanged()
{
    mScenegraphTreeModel->forceDataChanged();
    mMainWindowHandle->getScenegraphTreeView()->expandAll();
}

void MainController::forceAssetDefinitionTreeDataChanged()
{
    mAssetDefinitionTreeModel->forceDataChanged();
    mMainWindowHandle->getAssetDefinitionTreeView()->expandAll();
}

void MainController::onScenegraphTreeDataChanged()
{
    forceScenegraphTreeDataChanged();
}

void MainController::onAssetDefinitionTreeDataChanged()
{
    forceAssetDefinitionTreeDataChanged();
}

void MainController::onPropertiesTreeDataChanged()
{
    mPropertiesModel->forceDataChanged();
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
                mDreamProjectModel->getProject()->getProjectDefinition()
                );

    mProjectDirectoryModel.inflateFromDirectory(mProjectDirectory);

    log->info( "Load Project Result {}", loadResult);
    if (!loadResult)
    {
        emit notifyInvalidProjectDirectory(mProjectDirectory);
        emit notifyProjectWidgetsEnabledChanged(false);
        return;
    }


    updateWindowTitle(mProjectDirectory);

    shared_ptr<ProjectDefinition> currentProject = mDreamProjectModel->getProject()->getProjectDefinition();

    emit notifyProjectNameChanged(QString::fromStdString(currentProject->getName()));
    emit notifyProjectAuthorChanged(QString::fromStdString(currentProject->getAuthor()));
    emit notifyProjectDescriptionChanged(QString::fromStdString(currentProject->getDescription()));
    emit notifyProjectWindowWidthChanged(currentProject->getWindowWidth());
    emit notifyProjectWindowHeightChanged(currentProject->getWindowHeight());
    emit notifyProjectSceneListChanged(getSceneNamesListModel(currentProject->getSceneDefinitionsList()));
    emit notifyProjectWidgetsEnabledChanged(true);
    emit notifyProjectDefinitionChanged(currentProject);

    mScenegraphTreeModel.reset(new ScenegraphTreeModel(currentProject,mMainWindowHandle->getScenegraphTreeView()));
    mMainWindowHandle->getScenegraphTreeView()->setModel(mScenegraphTreeModel.get());

    mAssetDefinitionTreeModel.reset(new AssetDefinitionTreeModel(currentProject,mMainWindowHandle->getAssetDefinitionTreeView()));
    mMainWindowHandle->getAssetDefinitionTreeView()->setModel(mAssetDefinitionTreeModel.get());
    mSelectedSceneDefinitionHandle = currentProject->getStartupSceneDefinition();
    mDreamProjectModel->setSelectedSceneDefinition(mSelectedSceneDefinitionHandle);
    if (mSelectedSceneDefinitionHandle != nullptr)
    {
        log->info("Set selected scene to startup scene {}",mSelectedSceneDefinitionHandle->getNameAndUuidString());
    }
    else
    {
        log->error("Could not find startup scene");
    }

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
            ->getProjectDefinition()
            ->getJson().dump(1) << endl;
    ;
}

void
MainController::onAssetDefinitionProperty_AudioFile
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(mLastDirectory);

    QStringList filters;
    filters << "*.wav" << "*.ogg";
    openDialog.setNameFilters(filters);

    if(openDialog.exec())
    {
        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);
        mLastDirectory.setPath(sourceFilePath);

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

    mPropertiesModel->forceDataChanged();
}

void
MainController::showImportResultDialog(bool success, shared_ptr<IAssetDefinition>  adHandle, QString source)
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
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    QFileDialog openDialog;
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setDirectory(mLastDirectory);

    QStringList filters;
    filters << "*.ttf";
    openDialog.setNameFilters(filters);


    if(openDialog.exec())
    {

        QString sourceFilePath = openDialog.selectedFiles().first();
        QFile sourceFile(sourceFilePath);
        mLastDirectory.setPath(sourceFilePath);

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
(shared_ptr<IAssetDefinition>  adHandle)
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

    mPropertiesModel->forceDataChanged();
}

void MainController::onAssetDefinitionProperty_ModelMaterialShaderMap(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    log->info("Opening Material/Shader Map Widget");
    mMaterialShaderTableController.setProjectPath(mProjectDirectoryModel.getProjectDirectoryAbsolutePath());
    mMaterialShaderTableController.setModelDefinition(dynamic_pointer_cast<ModelDefinition>(adHandle));
    if (mSelectedProjectDefinitionHandle == nullptr)
    {
        log->info("SelectedProjectDefinition Handle is nullptr, setting from directory model");
        mSelectedProjectDefinitionHandle = mProjectDirectoryModel.getProjectDefinition();
    }
    mMaterialShaderTableController.setShaderHandlesVector(mSelectedProjectDefinitionHandle->getShaderAssetDefinitionVector());
    mMainWindowHandle->addRightDockWidget(&mMaterialShaderTableController);
}

void
MainController::onAssetDefinitionProperty_EditScript
(shared_ptr<IAssetDefinition>  adHandle)
{
    mScriptEditor.openScript(dynamic_pointer_cast<ScriptDefinition>(adHandle));
}

void
MainController::onAssetDefinitionProperty_EditShader
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "EditShader");
    mScriptEditor.openShader(dynamic_pointer_cast<ShaderDefinition>(adHandle));
}

void
MainController::onAssetDefinitionProperty_ScriptTemplateChanged
(shared_ptr<IAssetDefinition>  adHandle, const QString& templateName)
{
    // TODO - Move this to the script editor window
    auto log = spdlog::get("MainController");
    log->info( "Script Template Changed");
    auto result = QMessageBox::Yes;

    if (mProjectDirectoryModel.assetMainFileExists(adHandle))
    {
        result = QMessageBox::question
        (
            mMainWindowHandle,
            "Load Script Template",
            "Are you sure you want to load this template? Any existing data will be lost."
        );
    }

    if (result == QMessageBox::Yes)
    {
        QString scriptContent = mTemplatesModel.getScriptTemplate(templateName,QString::fromStdString(adHandle->getFormat()));
        log->info(
            "Proceeding to write template {} to {} \n {}",
            templateName.toStdString(),
            adHandle->getName(),
            scriptContent.toStdString()
        );
        bool writeOK = mProjectDirectoryModel.writeAssetData(scriptContent,adHandle,"",true);
        log->info("Write template success? {}",writeOK);
        return;
    }
}

void
MainController::onAssetDefinitionProperty_ShaderTemplateChanged
(shared_ptr<IAssetDefinition>  adHandle, const QString& templateName)
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

void MainController::onAssetDefinitionProperty_LightChooseAmbient(shared_ptr<IAssetDefinition>  def)
{
    auto log = spdlog::get("MainController");
    log->info( "Choose Diffuse Colour");

    QColor currentColour;
    auto lightDef = dynamic_pointer_cast<LightDefinition>(def);

    auto colour = lightDef->getAmbient();
    currentColour.setRgbF(
        static_cast<double>(colour.r),
        static_cast<double>(colour.g),
        static_cast<double>(colour.b)
    );

    auto chosenColour = QColorDialog::getColor(currentColour,mMainWindowHandle,"Ambient Colour");

    if (chosenColour.isValid())
    {
        lightDef->setAmbient(
            vec3(
                static_cast<float>(chosenColour.redF()),
                static_cast<float>(chosenColour.greenF()),
                static_cast<float>(chosenColour.blueF())
            )
        );
    }

    mPropertiesModel->forceDataChanged();
}

void
MainController::onAssetDefinitionProperty_LightChooseDiffuse
(shared_ptr<IAssetDefinition>  def)
{
    auto log = spdlog::get("MainController");
    log->info( "Choose Diffuse Colour");

    QColor currentColour;
    auto lightDef = dynamic_pointer_cast<LightDefinition>(def);

    auto colour = lightDef->getDiffuse();
    currentColour.setRgbF(
        static_cast<double>(colour.r),
        static_cast<double>(colour.g),
        static_cast<double>(colour.b)
    );

    auto chosenColour = QColorDialog::getColor(currentColour,mMainWindowHandle,"Diffuse Colour");

    if (chosenColour.isValid())
    {
        lightDef->setDiffuse(
            vec3(
                static_cast<float>(chosenColour.redF()),
                static_cast<float>(chosenColour.greenF()),
                static_cast<float>(chosenColour.blueF())
            )
        );
    }

    mPropertiesModel->forceDataChanged();
}

void
MainController::onAssetDefinitionProperty_LightChooseSpecular
(shared_ptr<IAssetDefinition>  def)
{
    auto log = spdlog::get("MainController");
    log->info( "Choose Specular Colour");

    QColor currentColour;
    auto lightDef = dynamic_pointer_cast<LightDefinition>(def);

    auto colour = lightDef->getSpecular();
    currentColour.setRgbF(
        static_cast<double>(colour.r),
        static_cast<double>(colour.g),
        static_cast<double>(colour.b)
    );

    auto chosenColour = QColorDialog::getColor(currentColour,mMainWindowHandle,"Specular Colour");

    if (chosenColour.isValid())
    {
        lightDef->setSpecular(
            vec3(
                static_cast<float>(chosenColour.redF()),
                static_cast<float>(chosenColour.greenF()),
                static_cast<float>(chosenColour.blueF())
            )
        );
    }

    mPropertiesModel->forceDataChanged();
}


void
MainController::onAssetDefinitionProperty_PathList
(shared_ptr<IAssetDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    log->info("Opening PathList Widget");
    mPathEditorFormController.setPathDefinition(dynamic_pointer_cast<PathDefinition>(adHandle));
    mPathPointViewer->setPathDefinition(dynamic_pointer_cast<PathDefinition>(adHandle));
    //mPathEditorFormController.getAllUpInYourFace();
    mMainWindowHandle->addRightDockWidget(&mPathEditorFormController);
}

void
MainController::onSceneObjectProperty_CaptureTranslation
(shared_ptr<SceneObjectDefinition> sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureTranslation");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();
    if (pHandle != nullptr)
    {
        shared_ptr<ProjectRuntime> prHandle = pHandle->getProjectRuntime();
        if (prHandle != nullptr)
        {
            shared_ptr<SceneRuntime> srHandle = prHandle->getActiveSceneRuntime();
            if (srHandle != nullptr)
            {
                shared_ptr<SceneObjectRuntime> sorHandle = srHandle->getSceneObjectRuntimeByUuid(sodHandle->getUuid());
                if (sorHandle != nullptr)
                {
                    sodHandle->getTransform()->setTranslation(sorHandle->getTranslation());
                }
            }
        }
    }

    mPropertiesModel->forceDataChanged();
}

void
MainController::onSceneObjectProperty_CaptureOrientation
(shared_ptr<SceneObjectDefinition> sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureOrientation");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        shared_ptr<ProjectRuntime> prHandle = pHandle->getProjectRuntime();
        if (prHandle != nullptr)
        {
            shared_ptr<SceneRuntime> srHandle = prHandle->getActiveSceneRuntime();
            if (srHandle != nullptr)
            {
                shared_ptr<SceneObjectRuntime> sorHandle = srHandle->getSceneObjectRuntimeByUuid(sodHandle->getUuid());
                if (sorHandle != nullptr)
                {
                    sodHandle->getTransform()->setOrientation(sorHandle->getOrientation());
                }
            }
        }
    }

    mPropertiesModel->forceDataChanged();
}

void
MainController::onSceneObjectProperty_CaptureScale
(shared_ptr<SceneObjectDefinition> sodHandle)
{
    auto log = spdlog::get("MainController");
    log->info( "CaptureScale");
    shared_ptr<Project> pHandle = mDreamProjectModel->getProject();
    if (pHandle)
    {
        shared_ptr<ProjectRuntime> prHandle = pHandle->getProjectRuntime();
        if (prHandle != nullptr)
        {
            shared_ptr<SceneRuntime> srHandle = prHandle->getActiveSceneRuntime();
            if (srHandle != nullptr)
            {
                shared_ptr<SceneObjectRuntime> sorHandle = srHandle->getSceneObjectRuntimeByUuid(sodHandle->getUuid());
                if (sorHandle != nullptr)
                {
                    sodHandle->getTransform()->setScale(sorHandle->getScale());
                }
            }
        }
    }

   mPropertiesModel->forceDataChanged();

}

void
MainController::onSceneObjectProperty_RemoveAsset
(shared_ptr<SceneObjectDefinition> sodHandle,shared_ptr<IDefinition>  adHandle)
{
    auto log = spdlog::get("MainController");
    log->info(
                "RemoveAsset {} from {}",
                adHandle->getNameAndUuidString(),
                sodHandle->getNameAndUuidString()
                );
    sodHandle->removeAssetDefinitionFromLoadQueue(dynamic_pointer_cast<IAssetDefinition>(adHandle));

    mPropertiesModel->forceDataChanged();
    mMainWindowHandle->getPropertiesTreeView()->update();
}

void
MainController::onSceneObjectProperty_RemoveChild
(shared_ptr<SceneObjectDefinition> sodHandle, shared_ptr<IDefinition> sodChildHandle)
{
    auto log = spdlog::get("MainController");
    log->info(
                "RemoveChild {} from {}",
                sodChildHandle->getNameAndUuidString(),
                sodHandle->getNameAndUuidString()
                );
    sodHandle->removeChildSceneObjectDefinition(dynamic_pointer_cast<SceneObjectDefinition>(sodChildHandle));
    onUI_ScenegraphUpdated();

    mPropertiesModel->forceDataChanged();
}
