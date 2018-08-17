/*
 * MainWindow.cpp
 *
 * Created: 13 2017 by Ashley
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
#include "MainWindowController.h"
#include "ui_MainWindow.h"
#include <QWindow>
#include <QSpacerItem>
#include <QLabel>
#include <vector>
#include <algorithm>
#include <memory>

#include "View/QOpenGLWindowComponent.h"
#include <spdlog/spdlog.h>
#include <QMenu>


using std::pair;
using std::begin;
using std::find;
using std::end;
using std::make_shared;

const vector<int> MainWindowController::mKeysPassedToWindow =
{
    Qt::Key_W,
    Qt::Key_A,
    Qt::Key_S,
    Qt::Key_D,
    Qt::Key_Up,
    Qt::Key_Down,
    Qt::Key_Left,
    Qt::Key_Right,
    Qt::Key_Shift,
    Qt::Key_Alt
};

MainWindowController::MainWindowController
(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mProjectDefinitionHandle(nullptr)
{
    auto log = spdlog::get("MainWindowController");
    if (log == nullptr)
    {
        log = spdlog::stdout_color_mt("MainWindowController");
    }

    log->info("Constructing");

    ui->setupUi(this);
    setupUiFeatures();

    setupGL(parent);
    setupMenu_Asset_NewDefinition();
    setupMenu_Debug();
    setActionsEnabled_Scene_Playback(false);
    setActionsEnabled_Scene_Modification(false);
    setActionEnabled_File_Save(false);
}

void MainWindowController::setupMenu_Debug()
{
    connect(ui->actionLogLevelTrace,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelDebug,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelInfo,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelWarn,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelError,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelCritical,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
    connect(ui->actionLogLevelOff,SIGNAL(triggered(bool)),this,SLOT(onMenu_Debug_LogLevelChanged(bool))) ;
}

void MainWindowController::setupUiFeatures()
{
    ui->dataGLSplitter->setStretchFactor(1,10);

    ui->scenegraphPropertiesSplitter->setStretchFactor(3,1);

    mVolumeSlider.setOrientation(Qt::Horizontal);
    mVolumeSlider.setMinimum(0);
    mVolumeSlider.setMaximum(100);
    mVolumeSlider.setSingleStep(1);
    mVolumeSlider.setMinimumWidth(150);
    mVolumeSlider.setMaximumWidth(150);
    mVolumeSlider.setValue(100);
    connect(&mVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(onMainVolumeChanged(int)));
    QIcon* volIcon = new QIcon(":svg/volume-up-solid.svg");
    QLabel* volLabel = new QLabel();
    volLabel->setPixmap(volIcon->pixmap(QSize(16,16)));
    volLabel->setFixedSize(QSize(16,16));
    ui->statusBar->addPermanentWidget(volLabel,1);
    ui->statusBar->addPermanentWidget(&mVolumeSlider,1);

    ui->scenegraphTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        ui->scenegraphTreeView,
        SIGNAL(customContextMenuRequested(const QPoint &)),
        this,
        SLOT(onScenegraphContextMenuRequested(const QPoint &))
    );
    ui->assetDefinitionTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        ui->assetDefinitionTreeView,
        SIGNAL(customContextMenuRequested(const QPoint &)),
        this,
        SLOT(onAsseetDefinitionContextMenuRequested(const QPoint &))
    );
    connect(
        ui->assetDefinitionTreeView,
        SIGNAL(activated(const QModelIndex&)),
        this,
        SLOT(onAssetDefinitionTreeViewActivated(const QModelIndex&))
    );
    connect(
        ui->scenegraphTreeView,
        SIGNAL(activated(const QModelIndex&)),
        this,
        SLOT(onScenegraphTreeViewActivated(const QModelIndex&))
    );
    connect(
        ui->propertiesTreeView,
        SIGNAL(activated(const QModelIndex&)),
        this,
        SLOT(onPropertiesTreeViewActivated(const QModelIndex&))
    );
}

void
MainWindowController::onScenegraphContextMenuRequested
(const QPoint &point)
{
    auto log = spdlog::get("MainWindowController");
    log->info("SceneGraph Context Menu Requested {},{}",point.x(),point.y());
    QModelIndex index = ui->scenegraphTreeView->indexAt(point);
    if (index.isValid())
    {
        auto item = static_cast<ScenegraphTreeItem*>(index.internalPointer());
        auto contextMenu = createScenegraphTreeContextMenu(item);
        contextMenu->exec(ui->scenegraphTreeView->mapToGlobal(point));
    }
}

void
MainWindowController::onAsseetDefinitionContextMenuRequested
(const QPoint &point)
{

    auto log = spdlog::get("MainWindowController");
    log->info("Asset Definition Context Menu Requested {},{}",point.x(),point.y());
    QModelIndex index = ui->assetDefinitionTreeView->indexAt(point);
    if (index.isValid())
    {
        auto item = static_cast<AssetDefinitionTreeItem*>(index.internalPointer());
        auto contextMenu = createAssetDefinitionTreeContextMenu(item);
        contextMenu->exec(ui->assetDefinitionTreeView->mapToGlobal(point));
    }
}

void MainWindowController::onCreateAssetDefinitionAction()
{
    CreateAssetAction* caa = dynamic_cast<CreateAssetAction*>(sender());
    auto log = spdlog::get("MainWindowController");
    log->info("Create Asset Definition of type {}", caa->getType().toStdString());
    emit notifyCreateNewAssetDefinition(caa->getType());
}

shared_ptr<QMenu>
MainWindowController::createAssetDefinitionTreeContextMenu
(AssetDefinitionTreeItem* item)
{
    IAssetDefinition* assetDef;
    auto menu = make_shared<QMenu>();
    QAction* nameAction = nullptr;
    CreateAssetAction* createAction = nullptr;
    QAction* deleteAction = nullptr;
    switch (item->getType())
    {
        case ASSET_DEFINITION:
            assetDef = static_cast<IAssetDefinition*>(item->getAssetDefinition());
            nameAction = new QAction(QString::fromStdString(assetDef->getName()),menu.get());
            nameAction->setEnabled(false);
            menu->addAction(nameAction);
            menu->addSeparator();
            deleteAction = new QAction("Delete",menu.get());
            menu->addAction(deleteAction);
            break;
        case ASSET_TREE_NODE:
            createAction = new CreateAssetAction
            (
                item->getTitle(),
                QString("New %1 Asset").arg(item->getTitle()),
                menu.get()
            );
            menu->addAction(createAction);
            break;
    }

    if (createAction != nullptr)
    {
        connect(
            createAction,SIGNAL(triggered()),
            this,SLOT(onCreateAssetDefinitionAction())
        );
    }
    return menu;
}

shared_ptr<QMenu>
MainWindowController::createScenegraphTreeContextMenu
(ScenegraphTreeItem* item)
{
    ProjectDefinition* projectDef = nullptr;
    SceneDefinition* sceneDef = nullptr;
    SceneObjectDefinition* sceneObjectDef = nullptr;

    ScenegraphMenuAction* titleAction = nullptr;
    ScenegraphMenuAction* deleteAction = nullptr;
    ScenegraphMenuAction* addChildAction = nullptr;
    ScenegraphMenuAction* duplicateAction = nullptr;
    QMenu*   addAssetMenu = nullptr;

    auto menu = make_shared<QMenu>();
    switch (item->getType())
    {
        case SCENEGRAPH_PROJECT:
            projectDef = static_cast<ProjectDefinition*>(item->getItem());

            titleAction = new ScenegraphMenuAction(item, QString::fromStdString(projectDef->getName()),menu.get());
            titleAction->setEnabled(false);
            menu->addAction(titleAction);
            addChildAction = new ScenegraphMenuAction(item,"Add Scene",menu.get());
            menu->addSeparator();
            menu->addAction(addChildAction);

            connect(addChildAction,SIGNAL(triggered()), this, SLOT(onScenegraphMenuAddNewSceneTriggered()));

            break;
        case SCENEGRAPH_SCENE:
            sceneDef = static_cast<SceneDefinition*>(item->getItem());

            titleAction = new ScenegraphMenuAction(item,QString::fromStdString(sceneDef->getName()),menu.get());
            titleAction->setEnabled(false);
            deleteAction = new ScenegraphMenuAction(item, "Delete Scene",menu.get());
            menu->addAction(titleAction);
            menu->addSeparator();
            menu->addAction(deleteAction);

            connect(deleteAction,SIGNAL(triggered()), this, SLOT(onScenegraphMenuDeleteSceneTriggered()));

            break;
        case SCENEGRAPH_SCENE_OBJECT:
            sceneObjectDef = static_cast<SceneObjectDefinition*>(item->getItem());

            titleAction = new ScenegraphMenuAction(item, QString::fromStdString(sceneObjectDef->getName()),menu.get());
            titleAction->setEnabled(false);
            deleteAction = new ScenegraphMenuAction(item, "Delete Scene Object",menu.get());
            addChildAction = new ScenegraphMenuAction(item, "Add Child",menu.get());
            duplicateAction = new ScenegraphMenuAction(item, "Duplicate",menu.get());

            menu->addAction(titleAction);
            menu->addSeparator();
            menu->addAction(addChildAction);
            menu->addAction(duplicateAction);
            addAssetMenu = menu->addMenu("Add Asset");
            createAssetsMenu(addAssetMenu,item);
            menu->addAction(deleteAction);

            connect(duplicateAction,SIGNAL(triggered()), this, SLOT(onScenegraphMenuDuplicateSceneObjectTriggered()));
            connect(addChildAction,SIGNAL(triggered()), this, SLOT(onScenegraphMenuAddSceneObjectTriggered()));
            connect(deleteAction,SIGNAL(triggered()), this, SLOT(onScenegraphMenuDeleteSceneObjectTriggered()));

            break;
        case SCENEGRAPH_TREE_NODE :
            break;
    }


    return menu;
}


void
MainWindowController::onScenegraphMenuAddNewSceneTriggered
()
{
    auto log = spdlog::get("MainWindowController");
    log->info("Scenegraph Menu: Add New Scene");
    auto action = dynamic_cast<ScenegraphMenuAction*>(sender());

    // Only triggered by project item
    auto projectDefHandle = static_cast<ProjectDefinition*>(action->getItemHandle()->getItem());
    auto newScene = projectDefHandle->createNewSceneDefinition();
    log->info("Created new scene {} for project {}", newScene->getNameAndUuidString(), projectDefHandle->getNameAndUuidString());
    emit notifyScenegraphTreeDataChanged();
}

void
MainWindowController::onScenegraphMenuDeleteSceneTriggered
()
{
    auto log = spdlog::get("MainWindowController");
    log->info("Scenegraph Menu: Delete Scene");
    auto action = dynamic_cast<ScenegraphMenuAction*>(sender());

    // Only triggered by project item
    auto sceneDefHandle = static_cast<SceneDefinition*>(action->getItemHandle()->getItem());
    auto parentProjectHandle = sceneDefHandle->getProjectDefinitionHandle();
    parentProjectHandle->removeSceneDefinition(sceneDefHandle);
    log->info("Deleted scene from project {}",parentProjectHandle->getNameAndUuidString());
    emit notifyScenegraphTreeDataChanged();
}

void
MainWindowController::onScenegraphMenuAddSceneObjectTriggered
()
{
    auto log = spdlog::get("MainWindowController");
    log->info("Scenegraph Menu: Add Scene Object");
    auto action = dynamic_cast<ScenegraphMenuAction*>(sender());

    switch (action->getItemHandle()->getType())
    {
        case SCENEGRAPH_TREE_NODE:
        case SCENEGRAPH_PROJECT:
        case SCENEGRAPH_SCENE:
            break;
        case SCENEGRAPH_SCENE_OBJECT:
            auto sceneObjectDefinitionHandle = static_cast<SceneObjectDefinition*>(action->getItemHandle()->getItem());
            auto newSceneObject = sceneObjectDefinitionHandle->createNewChildSceneObjectDefinition();
            log->info(
                "Created scene object {} for {}",
                newSceneObject->getNameAndUuidString(),
                sceneObjectDefinitionHandle->getNameAndUuidString()
            );
            break;
    }
    emit notifyScenegraphTreeDataChanged();
}

void MainWindowController::onScenegraphMenuDuplicateSceneObjectTriggered()
{
    auto log = spdlog::get("MainWindowController");
    log->info("Scenegraph Menu: Duplicate Scene Object");

    auto action = dynamic_cast<ScenegraphMenuAction*>(sender());

    switch (action->getItemHandle()->getType())
    {
        case SCENEGRAPH_TREE_NODE:
        case SCENEGRAPH_PROJECT:
        case SCENEGRAPH_SCENE:
            break;
        case SCENEGRAPH_SCENE_OBJECT:
            auto sceneObjectDefinitionHandle = static_cast<SceneObjectDefinition*>(action->getItemHandle()->getItem());
            auto duplicateSO = sceneObjectDefinitionHandle->duplicate();
            log->info(
                "Duplicated scene object {} to {}",
                sceneObjectDefinitionHandle->getNameAndUuidString(),
                duplicateSO->getNameAndUuidString()
            );
            break;
    }
    emit notifyScenegraphTreeDataChanged();
}

void
MainWindowController::onScenegraphMenuDeleteSceneObjectTriggered
()
{
    auto log = spdlog::get("MainWindowController");
    log->info("Scenegraph Menu: Delete Scene Object");

    auto action = dynamic_cast<ScenegraphMenuAction*>(sender());
    auto sceneObjectDefinitionHandle = static_cast<SceneObjectDefinition*>(action->getItemHandle()->getItem());
    auto parentSceneObject = sceneObjectDefinitionHandle->getParentSceneObjectHandle();

    if (parentSceneObject != nullptr)
    {
        parentSceneObject->removeChildSceneObjectDefinition(sceneObjectDefinitionHandle);
        log->info(
            "Deleted scene object from {}",
            parentSceneObject->getNameAndUuidString()
        );
        emit notifyScenegraphTreeDataChanged();
    }
    else
    {
       QMessageBox::warning(this,"Error","Cannot delete Root SceneObject");
    }
}

void MainWindowController::onAddAssetToSceneObjectTriggered()
{
    auto log = spdlog::get("MainWindowController");
    auto action = dynamic_cast<AddAssetToSceneObjectAction*>(sender());
    IAssetDefinition* adHandle = action->getAssetDefinitionHandle();
    ScenegraphTreeItem* treeItemHandle = action->getItemHandle();
    if (treeItemHandle->getType() == SCENEGRAPH_SCENE_OBJECT)
    {
        auto sceneObjHandle = static_cast<SceneObjectDefinition*>(treeItemHandle->getItem());
        log->info("Adding asset {} to object {}",adHandle->getNameAndUuidString(),sceneObjHandle->getNameAndUuidString());
        sceneObjHandle->addAssetDefinitionToLoadQueue(adHandle);
    }
    emit notifyScenegraphTreeDataChanged();
    emit notifyPropertiesTreeDataChanged();
}

void
MainWindowController::onScenegraphTreeViewActivated
(const QModelIndex& index)
{
    auto log = spdlog::get("MainWindowController");
    if (index.isValid())
    {
        auto item = static_cast<ScenegraphTreeItem*>(index.internalPointer());
        log->info("Valid index in current tree {}",item->getTitle().toStdString());
    }
    else
    {
        log->info("No valid index in current tree");
    }
}

void
MainWindowController::onPropertiesTreeViewActivated
(const QModelIndex& index)
{
    auto log = spdlog::get("MainWindowController");
    if (index.isValid())
    {
        auto item = static_cast<ScenegraphTreeItem*>(index.internalPointer());
        log->info("Valid index in current tree {}",item->getTitle().toStdString());
    }
    else
    {
        log->info("No valid index in current tree");
    }
}

void
MainWindowController::onAssetDefinitionTreeViewActivated
(const QModelIndex& index)
{
    auto log = spdlog::get("MainWindowController");
    if (index.isValid())
    {
        auto item = static_cast<ScenegraphTreeItem*>(index.internalPointer());
        log->info("Valid index in current tree {}",item->getTitle().toStdString());
    }
    else
    {
        log->info("No valid index in current tree");
    }
}

void MainWindowController::onMenu_Debug_LogLevelChanged(bool)
{
    auto log = spdlog::get("MainWindowController");
    auto action = dynamic_cast<QAction*>(sender());
    QString level = action->text();
    cout << "Setting log level to " << level.toStdString() << endl;
    showStatusBarMessage(QString("Log Level: %1").arg(level));
    spdlog::set_level(spdlog::level::from_str(level.toLower().toStdString()));
}


void
MainWindowController::createAssetsMenu
(QMenu* menu,ScenegraphTreeItem* item)
{
    if (mProjectDefinitionHandle != nullptr)
    {
        auto assetsMap = mProjectDefinitionHandle->getAssetDefinitionHandlesMap();
        auto beginAssetsMap = begin(assetsMap);
        auto endAssetsMap = end(assetsMap);
        auto currentAssetMapPair = beginAssetsMap;

        while (currentAssetMapPair != endAssetsMap)
        {
            AssetType currentType = currentAssetMapPair->first;
            string currentTypeString = Constants::getAssetTypeStringFromTypeEnum(currentType);
            QString currentTypeReadable = QString::fromStdString(Constants::getAssetTypeReadableNameFromString(currentTypeString));

            QMenu* currentMenu = menu->addMenu(currentTypeReadable);

            vector<IAssetDefinition*> currentVector = currentAssetMapPair->second;

            for (IAssetDefinition* currentDefinition : currentVector )
            {
                QAction* nextAssetAction = new AddAssetToSceneObjectAction(
                    item,
                    currentDefinition,
                    QString::fromStdString(currentDefinition->getName()),
                    currentMenu
                );
                connect(nextAssetAction,SIGNAL(triggered()),this,SLOT(onAddAssetToSceneObjectTriggered()));
                currentMenu->addAction(nextAssetAction);
            }
            currentAssetMapPair++;
        }
    }
}

void
MainWindowController::setActionsEnabled_Scene_Modification
(bool enabled)
{
    ui->actionSceneMenuNewScene->setEnabled(enabled);
    ui->actionSceneNewSceneObject->setEnabled(enabled);
    ui->actionSceneRemoveScene->setEnabled(enabled);

    ui->actionAssetMenuAddAssetToSelectedSceneObject->setEnabled(enabled);

    mMenu_Asset_NewDefinition->setEnabled(enabled);
    setActionEnabled_Debug_DumpProjectDefinitionJson(enabled);
}

void
MainWindowController::setActionsEnabled_Scene_Playback
(bool enabled)
{
    ui->actionPlay->setEnabled(enabled);
    ui->actionStop->setEnabled(enabled);
    ui->actionReload->setEnabled(enabled);
}

void
MainWindowController::setActionEnabled_File_Open
(bool enabled)
{
    ui->actionOpen->setEnabled(enabled);
}

QAction*
MainWindowController::getAction_Asset_NewDefinition
(AssetType type)
{
    return mActionMap_Asset_NewDefinition.at(type);
}

QAction*
MainWindowController::getAction_Scene_NewScene
()
{
    return ui->actionSceneMenuNewScene;
}

QAction*
MainWindowController::getAction_Scene_NewSceneObject
()
{
    return ui->actionSceneNewSceneObject;
}

QAction*
MainWindowController::getAction_ControlScene
()
{
    return ui->actionControl;
}

QAction*
MainWindowController::getAction_Asset_AddToSelectedSceneObject
()
{
    return ui->actionAssetMenuAddAssetToSelectedSceneObject;
}

QAction*
MainWindowController::getAction_Debug_DumpProjectDefinitionJson
()
{
    return ui->actionDebug_DumpProjectDefinitionJson;
}

void
MainWindowController::setActionEnabled_Debug_DumpProjectDefinitionJson
(bool enabled)
{

    ui->actionDebug_DumpProjectDefinitionJson->setEnabled(enabled);
}

void
MainWindowController::setActionEnabled_File_Save
(bool enabled)
{
    ui->actionSave->setEnabled(enabled);
}

void
MainWindowController::setActionEnabled_File_New
(bool enabled)
{
    ui->actionNew->setEnabled(enabled);
}

void
MainWindowController::setupMenu_Asset_NewDefinition
()
{
    mMenu_Asset_NewDefinition.reset(new QMenu());
    mMenu_Asset_NewDefinition->setTitle("New Definition");

    for (pair<AssetType,string> typePair : Constants::DREAM_ASSET_TYPES_MAP)
    {
        QAction *typeAction = new QAction
                (
                    QString::fromStdString
                    (
                        Constants::getAssetTypeReadableNameFromString(typePair.second)
                        )
                    );
        mMenu_Asset_NewDefinition->addAction(typeAction);
        mActionMap_Asset_NewDefinition.insert(pair<AssetType,QAction*>(typePair.first, typeAction));
    }

    ui->menuAsset->addMenu(mMenu_Asset_NewDefinition.get());
}

void
MainWindowController::setupGL
(QWidget *parent)
{
    QSurfaceFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QSurfaceFormat::CoreProfile );
    glFormat.setSamples(4);
    mWindowComponentHandle = ui->openGLWidget;
    mWindowComponentHandle->setFormat(glFormat);
}

MainWindowController::~MainWindowController
()
{
    delete ui;
}

QAction*
MainWindowController::getAction_File_New
()
{
    return ui->actionNew;
}

QAction*
MainWindowController::getAction_File_Open
()
{
    return ui->actionOpen;
}

QAction*
MainWindowController::getAction_File_CloseProject
()
{
    return ui->actionCloseProject;
}

QAction*
MainWindowController::getAction_View_TogglePhysicsDebug
()
{
    return ui->actionTogglePhysicsDebug;
}


QAction*
MainWindowController::getAction_File_Save
()
{
    return ui->actionSave;
}

QAction*
MainWindowController::getAction_Scene_Play
()
{
    return ui->actionPlay;
}

QAction*
MainWindowController::getAction_Scene_Stop
()
{
    return ui->actionStop;
}

void
MainWindowController::onInvalidProjectDirectory
(QString directory)
{
    QMessageBox::critical(
                this,
                tr("Invalid Project Directory"),
                tr("\"%1\"\n\nDoes not contain a vaild Dream project.").arg(directory)
                );
}

void
MainWindowController::onNoSceneSelected
()
{
    QMessageBox::critical(
                this,
                tr("No Scene Scelected"),
                tr("Please select a scene from the Scenegraph tree before running")
                );
    showStatusBarMessage(QString("No Scene Selected to Run!"));
}

QTreeView*
MainWindowController::getScenegraphTreeView
()
{
    return ui->scenegraphTreeView;
}

QTreeView*
MainWindowController::getPropertiesTreeView
()
{
    return ui->propertiesTreeView;
}

QTreeView*
MainWindowController::getAssetDefinitionTreeView
()
{
    return ui->assetDefinitionTreeView;
}

QAction*
MainWindowController::getAction_Preferences
()
{
    return ui->actionPreferences;
}

void
MainWindowController::showStatusBarMessage
(QString msg)
{
    ui->statusBar->showMessage(msg);
}

QAction*
MainWindowController::getAction_Scene_Reload
()
{
    return ui->actionReload;
}

QAction*
MainWindowController::getAction_View_ToggleGrid
()
{
    return ui->actionToggleGrid;
}

QAction*
MainWindowController::getAction_View_ToggleDebug
()
{
    return ui->actionToggleDebug;
}

QOpenGLWindowComponent*
MainWindowController::getWindowComponent
()
{
    return mWindowComponentHandle;
}

void
MainWindowController::clearOpenGLComponentRuntime
()
{
    mWindowComponentHandle->clearProjectRuntimeHandle();
}

void
MainWindowController::onSceneStopped
(SceneDefinition* scene)
{
    if (scene)
    {
        showStatusBarMessage(
                    QString("Stopped Scene: %1")
                    .arg(QString::fromStdString(scene->getNameAndUuidString())
                         )
                    );
    }
    else
    {
        showStatusBarMessage(QString("No Scene Running"));
    }
}

void MainWindowController::onProjectDefinitionChanged(ProjectDefinition* def)
{
   mProjectDefinitionHandle = def;
}


void
MainWindowController::keyPressEvent
(QKeyEvent* ke)
{
    //qDebug() << "MainWindow:" << ke->key() << "pressed";

    if (shouldPassKey(ke->key()))
    {
        mWindowComponentHandle->keyPressEvent(ke);
    }
    else
    {
        QMainWindow::keyPressEvent(ke);
    }
}

bool
MainWindowController::shouldPassKey
(int key)
{
    return std::find
            (
                begin(mKeysPassedToWindow),
                end(mKeysPassedToWindow),
                key
                ) != end(mKeysPassedToWindow);
}

void
MainWindowController::keyReleaseEvent
(QKeyEvent* ke)
{
    //qDebug() << "MainWindow:" << ke->key() << " released";

    if (shouldPassKey(ke->key()))
    {
        mWindowComponentHandle->keyReleaseEvent(ke);
    }
    else
    {
        QMainWindow::keyReleaseEvent(ke);
    }
}

CreateAssetAction::CreateAssetAction
(QString type, QObject* parent)
    : QAction(parent),
      mType(type) {}

CreateAssetAction::CreateAssetAction
(QString type, const QString& text, QObject* parent)
    : QAction(text,parent),
      mType(type)
{}

CreateAssetAction::CreateAssetAction
(QString type, const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon,text,parent),
      mType(type) {}

QString
CreateAssetAction::
getType() const
{
    return mType;
}

ScenegraphMenuAction::ScenegraphMenuAction
(ScenegraphTreeItem *itemHandle, QObject* parent)
    : QAction(parent), mItemHandle(itemHandle) {}

ScenegraphMenuAction::ScenegraphMenuAction
(ScenegraphTreeItem *itemHandle, const QString& text, QObject* parent)
    : QAction(text,parent), mItemHandle(itemHandle) {}

ScenegraphMenuAction::ScenegraphMenuAction
(ScenegraphTreeItem *itemHandle, const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon,text,parent), mItemHandle(itemHandle) {}

ScenegraphTreeItem*
ScenegraphMenuAction::getItemHandle
() const
{
   return mItemHandle;
}

AddAssetToSceneObjectAction::AddAssetToSceneObjectAction
(ScenegraphTreeItem* itemHandle, IAssetDefinition* adHandle, QObject* parent)
    : QAction(parent),
      mItemHandle(itemHandle),
      mAssetDefinitionHandle(adHandle)
{

}

AddAssetToSceneObjectAction::AddAssetToSceneObjectAction
(ScenegraphTreeItem* itemHandle, IAssetDefinition* adHandle, const QString& text, QObject* parent)
    : QAction(text,parent),
      mItemHandle(itemHandle),
      mAssetDefinitionHandle(adHandle)

{

}

AddAssetToSceneObjectAction::AddAssetToSceneObjectAction
(ScenegraphTreeItem* itemHandle, IAssetDefinition* adHandle, const QIcon& icon, const QString& text, QObject* parent)
: QAction(icon,text,parent),
      mItemHandle(itemHandle),
      mAssetDefinitionHandle(adHandle)

{

}

ScenegraphTreeItem*
AddAssetToSceneObjectAction::getItemHandle
() const
{
    return mItemHandle;
}

IAssetDefinition*
AddAssetToSceneObjectAction::getAssetDefinitionHandle
() const
{
    return mAssetDefinitionHandle;
}


void MainWindowController::onMainVolumeChanged(int vol)
{
   auto log = spdlog::get("MainWindowController");
   log->info("Volume changed to {}",vol);
   emit notifyMainVolumeChanged(vol);
}
