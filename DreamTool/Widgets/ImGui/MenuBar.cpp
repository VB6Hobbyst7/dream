#include "MenuBar.h"
#include "LuaDebugWindow.h"
#include "ProjectBrowser.h"
#include "PropertiesWindow.h"
#include "SceneStateWindow.h"
#include "../../DTState.h"
#include "../../deps/ImGui/imgui_internal.h"
#include "../../deps/ImGui/imguifilesystem.h"
#include <sstream>

namespace DreamTool
{
    MenuBar::MenuBar(DTState* def)
        : ImGuiWidget(def),
          mFPS(0)
    {
        setLogClassName("MenuBar");
    }

    MenuBar::~MenuBar
    ()
    {

    }
    void
    MenuBar::draw
    ()
    {
        auto log = getLog();

        bool showQuitDialog = false;
        bool newButtonClicked = false;
        bool openButtonClicked = false;
        bool showSaveSuccessDialog = false;
        bool showPleaseDestroyScenesDialog = false;
        bool modelBatchImportClicked = false;

        auto pRuntime = mState->project->getProjectRuntime();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                newButtonClicked = ImGui::MenuItem("New");
                openButtonClicked = ImGui::MenuItem("Open");
                if(ImGui::MenuItem("Save"))
                {
                    ProjectDirectory pDir(mState->project);
                    if(pDir.saveProject())
                    {
                        showSaveSuccessDialog = true;
                    }
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Close"))
                {

                }
                ImGui::Separator();
                showQuitDialog = ImGui::MenuItem("Quit");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                bool showProjectBrowser = mState->projectBrowser.getVisible();
                if(ImGui::Checkbox("Project Browser",&showProjectBrowser))
                {
                    mState->projectBrowser.setVisible(showProjectBrowser);
                }

                bool showPropertiesWindow = mState->propertiesWindow.getVisible();
                if(ImGui::Checkbox("Properties Window",&showPropertiesWindow))
                {
                   mState->propertiesWindow.setVisible(showPropertiesWindow);
                }

                bool showSceneStatesWindow = mState->sceneStateWindow.getVisible();
                if (ImGui::Checkbox("Scene States",&showSceneStatesWindow))
                {
                   mState->sceneStateWindow.setVisible(showSceneStatesWindow);
                }

                bool showGridPropsWindow = mState->gridPropertiesWindow.getVisible();
                if (ImGui::Checkbox("Grid Properties",&showGridPropsWindow))
                {
                   mState->gridPropertiesWindow.setVisible(showGridPropsWindow);
                }


                ImGui::DragFloat("Text Scaling", &(ImGui::GetCurrentContext()->Font->Scale),0.1f,1.0f,10.0f);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene"))
            {
                vector<string> sceneNames;

                ProjectDefinition* projDef = nullptr;
                if (mState->project)
                {
                    projDef = mState->project->getProjectDefinition();
                    if (projDef)
                    {
                        auto scenesVector = projDef->getSceneDefinitionsVector();
                        for (auto scene : scenesVector)
                        {
                            sceneNames.push_back(scene->getName());
                        }
                    }
                }

                int sceneToLoadIndex = -1;
                if (StringCombo("Set Scene \"To Load\"", &sceneToLoadIndex, sceneNames, sceneNames.size()))
                {
                    auto selectedSceneDef = projDef->getSceneDefinitionAtIndex(sceneToLoadIndex);
                    if (!pRuntime->hasSceneRuntime(selectedSceneDef->getUuid()))
                    {
                        auto newSceneRT = new SceneRuntime(selectedSceneDef,pRuntime);
                        newSceneRT->setState(SceneState::SCENE_STATE_TO_LOAD);
                        pRuntime->addSceneRuntime(newSceneRT);
                        setMessageString("Added Scene Runtime: "+newSceneRT->getName());
                    }
                }

                vector<string> runtimeNames;
                if (pRuntime)
                {
                    for (auto s : pRuntime->getSceneRuntimeVector())
                    {
                        // Show only loaded scenes
                        runtimeNames.push_back(s->getName());
                    }
                }

                int sceneActiveIndex = -1;
                if (StringCombo("Set Scene \"Active\"", &sceneActiveIndex, runtimeNames, runtimeNames.size()))
                {
                    if (pRuntime)
                    {
                        auto rt = pRuntime->getSceneRuntimeVector().at(sceneActiveIndex);
                        if (rt)
                        {
                            pRuntime->setSceneRuntimeActive(rt->getUuid());
                            setMessageString("Activated Scene: "+rt->getName());
                        }
                    }
                }

                int sceneToDestroyIndex = -1;
                if (StringCombo("Set Scene \"To Destroy\"", &sceneToDestroyIndex, runtimeNames, runtimeNames.size()))
                {
                    mState->propertiesWindow.clearPropertyTargets();
                    if (pRuntime)
                    {
                        auto rt = pRuntime->getSceneRuntimeVector().at(sceneToDestroyIndex);
                        if (rt)
                        {
                            rt->setState(SceneState::SCENE_STATE_TO_DESTROY);
                            setMessageString("Destroyed Scene: "+rt->getName());
                        }
                        mState->selectionHighlighter.clearSelection();
                    }
                }

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Components"))
            {
                if (ImGui::BeginMenu("Input"))
                {
                    DTState::InputTarget mode = mState->inputTarget;
                    if (ImGui::RadioButton("To Editor",mode == DTState::InputTarget::EDITOR))
                    {
                        mState->inputTarget = DTState::InputTarget::EDITOR;
                    }

                    if (ImGui::RadioButton("To Scene", mode == DTState::InputTarget::SCENE))
                    {
                        mState->inputTarget = DTState::InputTarget::SCENE;
                    }
                    ImGui::EndMenu();
                }

                float volume = 1.0f;
                auto rt = mState->project->getProjectRuntime();
                auto audioComp = rt->getAudioComponent();
                if (audioComp)
                {
                    volume = audioComp->getVolume();
                }

                if(ImGui::SliderFloat("Volume",&volume,0.0f,1.0f))
                {
                    if (audioComp)
                    {
                        audioComp->setVolume(volume);
                    }
                }

                bool scripting = false;
                if (pRuntime)
                {
                    scripting = pRuntime->getScriptingEnabled();
                }
                if(ImGui::Checkbox("Scripting",&scripting))
                {
                    if(pRuntime)
                    {
                        pRuntime->setScriptingEnabled(scripting);
                    }
                }

                if (ImGui::MenuItem("Clear Caches"))
                {
                    if (pRuntime && !pRuntime->hasLoadedScenes())
                    {
                        pRuntime->clearAllCaches();
                    }
                    else
                    {
                        showPleaseDestroyScenesDialog = true;
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Model Batch Importer"))
                {
                   modelBatchImportClicked = true;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug"))
            {
                bool showRenderingPipeline = mState->renderPipelineWindow.getVisible();
                if (ImGui::Checkbox("Rendering Pipeline",&showRenderingPipeline))
                {
                    mState->renderPipelineWindow.setVisible(showRenderingPipeline);
                }

                bool showJoystickDebug = mState->gamepadStateWindow.getVisible();
                if (ImGui::Checkbox("Joystick State",&showJoystickDebug))
                {
                    mState->gamepadStateWindow.setVisible(showJoystickDebug);
                }
                auto showLuaDebug = mState->luaDebugWindow.getVisible();
                if (ImGui::Checkbox("Lua Debug Window",&showLuaDebug))
                {
                    mState->luaDebugWindow.setVisible(showLuaDebug);
                }

                if (pRuntime)
                {
                    auto active = pRuntime->getActiveSceneRuntime();
                    auto physicsDebug = active ? active->getPhysicsDebug() : false;
                    if (ImGui::Checkbox("Physics Debug",&physicsDebug))
                    {
                       if (active)
                       {
                           active->setPhysicsDebug(physicsDebug);
                           dynamic_cast<SceneDefinition*>(active->getDefinition())->setPhysicsDebug(physicsDebug);
                       }
                    }
                }

                if(ImGui::BeginMenu("Engine Logging"))
                {
                    static spdlog::level::level_enum mode = spdlog::level::off;
                    if (ImGui::RadioButton("Off", mode == spdlog::level::off))
                    {
                        mode = spdlog::level::off;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Error", mode == spdlog::level::err))
                    {
                        mode = spdlog::level::err;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Critical", mode == spdlog::level::critical))
                    {
                        mode = spdlog::level::critical;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Warning", mode == spdlog::level::warn))
                    {
                        mode = spdlog::level::warn;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Info", mode == spdlog::level::info))
                    {
                        mode = spdlog::level::info;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Debug", mode == spdlog::level::debug))
                    {
                        mode = spdlog::level::debug;
                        spdlog::set_level(mode);
                    }
                    if (ImGui::RadioButton("Trace", mode == spdlog::level::trace))
                    {
                        mode = spdlog::level::trace;
                        spdlog::set_level(mode);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            static char msgBuf[512] = {0};
            snprintf(
                msgBuf,
                512,
                "%s | Input -> %s | %.2f fps",
                mMessageString.c_str(),
                (
                    mState->inputTarget == DTState::InputTarget::EDITOR ?
                    "Editor" : "Scene"
                ),
                mFPS
            );

            auto maxX = ImGui::GetWindowContentRegionMax().x;
            ImVec2 msgSize = ImGui::CalcTextSize(msgBuf);

            ImGui::SameLine(maxX-msgSize.x-mMessagePadding);
            ImGui::Text("%s", msgBuf);

            ImGui::EndMainMenuBar();
        }

        // New Project
        static ImGuiFs::Dialog newDlg;
        newDlg.chooseFolderDialog(newButtonClicked,mState->lastDirectory.c_str());
        if (strlen(newDlg.getChosenPath())>0)
        {
            mState->lastDirectory = newDlg.getChosenPath();
            /*
                auto path = newDlg.getChosenPath();
                ProjectDirectoryModel dirModel();
                mState->project->clear();
            */
        }

        // Open Project
        bool openProjectFailed = false;
        static ImGuiFs::Dialog openDlg;
        const char* chosenPath = openDlg.chooseFolderDialog(openButtonClicked,mState->lastDirectory.c_str());
        if (strlen(chosenPath) > 0)
        {
            auto projectDir = openDlg.getChosenPath();
            mState->lastDirectory = projectDir;
            log->error("Opening project {}",projectDir);
            if(mState->project->openFromDirectory(projectDir))
            {
                mState->project->createProjectRuntime();
            }
            else
            {
                openProjectFailed = true;
            }
        }

        if (openProjectFailed)
        {
            ImGui::OpenPopup("Failed to open Project");
        }

        if (ImGui::BeginPopupModal("Failed to open Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("This directory does not contain a valid Dream Project\n\n");
            ImGui::Separator();

            if (ImGui::Button("OK",ImVec2(-1,0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        if (showPleaseDestroyScenesDialog)
        {
           ImGui::OpenPopup("Loaded Scenes");
        }

        static ImGuiFs::Dialog modelBatchImportDlg;
        const char* modelBatchPath = modelBatchImportDlg.chooseFolderDialog(modelBatchImportClicked,mState->lastDirectory.c_str());
        if (strlen(modelBatchPath) >0 )
        {
            string dir = modelBatchImportDlg.getChosenPath();
            mState->lastDirectory = dir;
            mState->modelDefinitionBatchImporter.setDirectory(dir);
            mState->modelDefinitionBatchImporter.findModels();
        }

        if (mState->modelDefinitionBatchImporter.hasModels())
        {
            ImGui::OpenPopup("Batch Import Models");
        }

        if(ImGui::BeginPopupModal("Batch Import Models"))
        {
            ImGui::Columns(2);

            ImGui::BeginChild("ModelsList");
            {
                ImGui::Columns(2);
                ImGui::Text("Model");
                ImGui::NextColumn();
                ImGui::Text("Import");
                ImGui::NextColumn();
                ImGui::Separator();

                int index = 0;
                for (string modelName : mState->modelDefinitionBatchImporter.getModelsFoundNames())
                {
                    ImGui::Text("%s",modelName.c_str());
                    ImGui::NextColumn();
                    ImGui::PushID(index);
                    ImGui::Checkbox("##",(mState->modelDefinitionBatchImporter.getModelsToImport()+index));
                    ImGui::PopID();
                    ImGui::NextColumn();
                    index++;
                }
                ImGui::Columns(1);
            }
            ImGui::EndChild();

            ImGui::NextColumn();
            ImGui::Text("Options");
            bool appendDirName = mState->modelDefinitionBatchImporter.getAppendDirectoryName();
            stringstream appendLabel;
            appendLabel << "Append Directory Name (";
            appendLabel << mState->modelDefinitionBatchImporter.getDirectoryName();
            appendLabel << ")";
            if (ImGui::Checkbox(appendLabel.str().c_str(),&appendDirName))
            {
               mState->modelDefinitionBatchImporter.setAppendDirectoryName(appendDirName);
            }

            auto replace = mState->modelDefinitionBatchImporter.getReplaceExisting();
            if(ImGui::Checkbox("Replace Existing",&replace))
            {
               mState->modelDefinitionBatchImporter.setReplaceExisting(replace);
            }
            if(ImGui::Button("Cancel"))
            {
                mState->modelDefinitionBatchImporter.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Import Models"))
            {
                mState->modelDefinitionBatchImporter.import();
                ImGui::CloseCurrentPopup();
            }

           ImGui::EndPopup();
        }

        if (mState->modelDefinitionBatchImporter.hasResults())
        {
            ImGui::OpenPopup("Model Import Results");
        }

        if (ImGui::BeginPopupModal("Model Import Results"))
        {
            ImGui::Columns(2);
            ImGui::Text("Model");
            ImGui::NextColumn();
            ImGui::Text("Result");
            ImGui::Separator();
            ImGui::NextColumn();
            for (auto result : mState->modelDefinitionBatchImporter.getImportResults())
            {
               ImGui::Text("%s",result.definition->getName().c_str());
               ImGui::NextColumn();
               ImGui::Text("%s",ModelDefinitionBatchImporter::resultString(result.result).c_str());
               ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();
            if(ImGui::Button("OK"))
            {
                mState->modelDefinitionBatchImporter.clear();
                ImGui::CloseCurrentPopup();
            }
           ImGui::EndPopup();
        }

        if(ImGui::BeginPopupModal("Loaded Scenes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text
            (
                "There are one or more Loaded Scenes.\n"
                "\n"
                "Please destroy them before clearing Caches.\n"
                "\n"
            );
            ImGui::Separator();

            if (ImGui::Button("OK##clearCachesDialog", ImVec2(-1, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        if (showQuitDialog)
        {
            ImGui::OpenPopup("Quit?");
        }

        if (ImGui::BeginPopupModal("Quit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to quit?\n\nAny unsaved changes will be lost.\n\n");
            ImGui::Separator();

            if (ImGui::Button("Cancel##cancelQuit", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Quit##confirmQuit", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                mState->MainLoopDone = true;
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        if (showSaveSuccessDialog)
        {
            ImGui::OpenPopup("Save Success");
        }

        if (ImGui::BeginPopupModal("Save Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Project Saved Successfully!");
            ImGui::Separator();

            if (ImGui::Button("OK",ImVec2(-1,0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }
    }

    float MenuBar::getMessagePadding() const
    {
        return mMessagePadding;
    }

    void MenuBar::setMessagePadding(float messagePadding)
    {
        mMessagePadding = messagePadding;
    }

    string MenuBar::getMessageString() const
    {
        return mMessageString;
    }

    void MenuBar::setMessageString(const string& messageString)
    {
        mMessageString = messageString;
    }

    void MenuBar::setFPS(double fPS)
    {
        mFPS = fPS;
    }
}
