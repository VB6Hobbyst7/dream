/*
 * MainWindow.h
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

#pragma once

#include <memory>
#include <vector>
#include <map>

#include <GL/glew.h>

#include <QDockWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringListModel>
#include <QTreeView>
#include <QSlider>
#include <QAction>
#include <QKeyEvent>

#include "../View/QOpenGLWindowComponent.h"
#include "../Model/TreeModels/AssetDefinition/AssetDefinitionTreeItem.h"
#include "../Model/TreeModels/Scenegraph/ScenegraphTreeItem.h"
#include "../Controller/EditorTabController.h"

using std::unique_ptr;
using std::map;
using Dream::AssetType;

namespace Ui
{
    class MainWindow;
}

class AddAssetToSceneObjectAction : public QAction
{
public:
    AddAssetToSceneObjectAction
    (ScenegraphTreeItem* itemHandle, IAssetDefinition* adHadle, QObject *parent = nullptr);

    AddAssetToSceneObjectAction
    (ScenegraphTreeItem* itemHandle, IAssetDefinition* adHadle, const QString &text, QObject *parent = nullptr);

    AddAssetToSceneObjectAction
    (ScenegraphTreeItem* itemHandle, IAssetDefinition* adHadle, const QIcon &icon, const QString &text, QObject *parent = nullptr);

    ScenegraphTreeItem* getItemHandle() ;
    IAssetDefinition* getAssetDefinitionHandle() ;

private:
    ScenegraphTreeItem* mItemHandle;
    IAssetDefinition* mAssetDefinitionHandle;

};

class ScenegraphMenuAction : public QAction
{
public:
    ScenegraphMenuAction(ScenegraphTreeItem* itemHandle, QObject *parent = nullptr);
    ScenegraphMenuAction(ScenegraphTreeItem* itemHandle, const QString &text, QObject *parent = nullptr);
    ScenegraphMenuAction(ScenegraphTreeItem* itemHandle, const QIcon &icon, const QString &text, QObject *parent = nullptr);

    ScenegraphTreeItem* getItemHandle() ;
private:
    ScenegraphTreeItem* mItemHandle;

};

class CreateAssetDefinitionAction : public QAction
{
public:
    CreateAssetDefinitionAction(QString type, QObject *parent = nullptr);
    CreateAssetDefinitionAction(QString type, const QString &text, QObject *parent = nullptr);
    CreateAssetDefinitionAction(QString type, const QIcon &icon, const QString &text, QObject *parent = nullptr);

    QString getType() ;

private:
    QString mType;
};

class DeleteAssetDefinitionAction : public QAction
{
public:
    DeleteAssetDefinitionAction
    (QObject *parent = nullptr);

    DeleteAssetDefinitionAction
    (const QString &text, QObject *parent = nullptr);

    DeleteAssetDefinitionAction
    (const QIcon &icon,const QString &text, QObject *parent = nullptr);

    IAssetDefinition* mItemHandle;
};

class MainWindowController : public QMainWindow, public LuaPrintListener
{
    Q_OBJECT
public:
    explicit MainWindowController(QWidget *parent = nullptr);
    ~MainWindowController( ) override;
    Ui::MainWindow *ui;

    void onPrint(std::string) override;

    QOpenGLWindowComponent* getWindowComponent();

    void setProjectDirectoryModel(ProjectDirectoryModel* dirModel);
    void addRightDockWidget(QWidget* widget);

    QTreeView* getScenegraphTreeView();
    QTreeView* getPropertiesTreeView();
    QTreeView* getAssetDefinitionTreeView();

    QAction* getAction_Preferences();

    QAction* getAction_File_New();
    QAction* getAction_File_Save();
    QAction* getAction_File_Open();
    QAction* getAction_File_CloseProject();

    void setActionEnabled_File_New(bool enabled);
    void setActionEnabled_File_Save(bool enabled);
    void setActionEnabled_File_Open(bool enabled);

    QAction* getAction_Scene_Stop();
    QAction* getAction_Scene_Play();
    QAction* getAction_Scene_Reload();
    QAction* getAction_Scene_NewScene();
    QAction* getAction_Scene_NewSceneObject();

    void setActionsEnabled_Scene_Playback(bool enabled);
    void setActionsEnabled_Scene_Modification(bool enabled);

    QAction* getAction_View_ToggleGrid();
    QAction* getAction_View_ToggleDebug();
    QAction* getAction_View_TogglePhysicsDebug();
    QAction* getAction_View_ToggleHighlightSelected();

    QAction* getAction_Asset_NewDefinition(AssetType type);
    QAction* getAction_Asset_AddToSelectedSceneObject();
    QAction* getAction_ControlScene();

    QAction* getAction_Debug_DumpProjectDefinitionJson();
    void setActionEnabled_Debug_DumpProjectDefinitionJson(bool enabled);

    void setupMenu_Asset_NewDefinition();

    void openScriptEditor(ScriptDefinition* scriptDefinitionHandle);
    void openShaderEditor(ShaderDefinition* shaderDefinitionHandle);


    void setTemplatesModelHandle(TemplatesModel* templatesModelHandle);

signals:
    void notifyActionNew(QString);
    void notifyActionOpen(QString);
    void notifyActionSave(QString);
    void notifyCreateNewAssetDefinition(QString type);
    void notifyScenegraphTreeDataChanged();
    void notifyPropertiesTreeDataChanged();
    void notifyMainVolumeChanged(int);
    void notifyAssetDefinitionTreeDataChanged();

public slots:
    void onInvalidProjectDirectory(QString directory);
    void onNoSceneSelected();
    void showStatusBarMessage(QString msg);
    void onSceneStopped(SceneDefinition* scene);
    void onProjectDefinitionChanged(ProjectDefinition*);
    void onScenegraphTreeExpandRequested();

    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    protected:
    void setupUiFeatures();

    int  isEditorTabOpen(IAssetDefinition* definition);
    void setupEditorTabCloseButtonSignal();

protected slots:
    void onMainVolumeChanged(int);
    void onEditorTabCloseRequested(int index);

private slots:
    void onScenegraphContextMenuRequested(const QPoint& point);
    void onAsseetDefinitionContextMenuRequested(const QPoint& point);
    void onCreateAssetDefinitionAction();
    void onScenegraphMenuAddNewSceneTriggered();
    void onScenegraphMenuDeleteSceneTriggered();
    void onScenegraphMenuAddSceneObjectTriggered();
    void onScenegraphMenuDuplicateSceneObjectTriggered();
    void onScenegraphMenuDeleteSceneObjectTriggered();
    void onAssetDefinitionMenuDeleteTriggered();
    void onAddAssetToSceneObjectTriggered();

    void onScenegraphTreeViewActivated(const QModelIndex &index);
    void onPropertiesTreeViewActivated(const QModelIndex &index);
    void onAssetDefinitionTreeViewActivated(const QModelIndex &index);
    void onMenu_Debug_LogLevelChanged(bool);

private:
    ProjectDefinition* mProjectDefinitionHandle;
    ProjectDirectoryModel* mProjectDirectoryModelHandle;
    TemplatesModel* mTemplatesModelHandle;
    bool shouldPassKey(int key);
    void setupGL(QWidget *parent);
    QOpenGLWindowComponent* mWindowComponentHandle;
    const static vector<int> mKeysPassedToWindow;
    map<AssetType,QAction*> mActionMap_Asset_NewDefinition;
    unique_ptr<QMenu> mMenu_Asset_NewDefinition;
    shared_ptr<QMenu> createAssetDefinitionTreeContextMenu(AssetDefinitionTreeItem*);
    shared_ptr<QMenu> createScenegraphTreeContextMenu(ScenegraphTreeItem*);
    void createAssetsMenu(QMenu* menu,ScenegraphTreeItem* item);
    void setupMenu_Debug();
    QSlider mVolumeSlider;
    QDockWidget mRightDockWidget;
    vector<shared_ptr<EditorTabController>> mEditorTabForms;
};


