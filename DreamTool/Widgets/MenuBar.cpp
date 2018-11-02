#include "MenuBar.h"
#include "ImGuiHelpers.h"
#include "LuaDebugWindow.h"
#include "ProjectBrowser.h"
#include "PropertiesWindow.h"

#include "../deps/ImGui/imgui_internal.h"
#include "../deps/ImGui/imguifilesystem.h"

using Dream::SceneDefinition;
using Dream::SceneState;
using Dream::SceneRuntime;
using Dream::ProjectRuntime;
using Dream::ProjectDirectory;

extern bool MainLoopDone;

namespace DreamTool
{
    MenuBar::MenuBar
    (Project* def, ProjectBrowser* pb, PropertiesWindow* pw, LuaDebugWindow* debugWindow)
        : DTWidget(def),
          mProjectBrowser(pb),
          mPropertiesWindow(pw),
          mLuaDebugWindow(debugWindow)
    {

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

        bool showQuit = false;
        bool newButtonClicked = false;
        bool openButtonClicked = false;
        bool saveSuccess = false;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                newButtonClicked = ImGui::MenuItem("New");
                openButtonClicked = ImGui::MenuItem("Open");
                if(ImGui::MenuItem("Save"))
                {
                    ProjectDirectory pDir(mProject);
                    if(pDir.saveProject())
                    {
                        saveSuccess = true;
                    }
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Close"))
                {

                }
                ImGui::Separator();
                showQuit = ImGui::MenuItem("Quit");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                bool showProjectBrowser = !mProjectBrowser->getHidden();
                bool showPropertiesWindow = !mPropertiesWindow->getHidden();

                if(ImGui::Checkbox("Project Browser",&showProjectBrowser))
                {
                    mProjectBrowser->setHidden(!showProjectBrowser);
                }

                if(ImGui::Checkbox("Properties Window",&showPropertiesWindow))
                {
                   mPropertiesWindow->setHidden(!showPropertiesWindow);
                }

                ImGui::DragFloat("Text Scaling", &(ImGui::GetCurrentContext()->Font->Scale),0.1f,1.0f,10.0f);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene"))
            {

                SceneRuntime* sceneRuntime = ProjectRuntime::CurrentSceneRuntime;
                if (ImGui::MenuItem("Start Scene"))
                {
                    if(sceneRuntime)
                    {
                        sceneRuntime->setState(SceneState::SCENE_STATE_NOT_LOADED);
                    }
                }
                if (ImGui::MenuItem("Stop Scene"))
                {
                    if(sceneRuntime)
                    {
                        sceneRuntime->setState(SceneState::SCENE_STATE_STOPPED);
                    }
                }

                ImGui::Separator();

                vector<string> sceneNames;
                static int sceneIndex = -1;

                if (mProject)
                {
                    auto projDef = mProject->getProjectDefinition();
                    if (projDef)
                    {
                        auto scenesVector = projDef->getSceneDefinitionsVector();
                        for (auto scene : scenesVector)
                        {
                            sceneNames.push_back(scene->getName());
                        }
                    }
                }

                if (StringCombo("Active Scene", &sceneIndex, sceneNames, sceneNames.size()))
                {
                    ImGui::OpenPopup("Change Scene?");
                }

                if (ImGui::BeginPopupModal("Change Scene?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Do you want to switch to this Scene?");
                    ImGui::Separator();

                    if (ImGui::Button("Not Now", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Switch Scene", ImVec2(120, 0)))
                    {
                        if (mProject)
                        {
                            auto projDef = mProject->getProjectDefinition();
                            if (projDef)
                            {
                                auto selectedSceneDef = projDef->getSceneDefinitionAtIndex(sceneIndex);
                                auto projRunt = mProject->getProjectRuntime();
                                if (projRunt)
                                {
                                    if (ProjectRuntime::CurrentSceneRuntime)
                                    {
                                        ProjectRuntime::CurrentSceneRuntime->setState(SceneState::SCENE_STATE_STOPPED);
                                        projRunt->destructSceneRuntime(ProjectRuntime::CurrentSceneRuntime);
                                        ProjectRuntime::CurrentSceneRuntime = nullptr;
                                    }
                                    ProjectRuntime::CurrentSceneRuntime = projRunt->constructSceneRuntime(selectedSceneDef);
                                    mPropertiesWindow->clearPropertyTargets();
                                }
                            }
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::EndPopup();
                }

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Components"))
            {
                if (ImGui::BeginMenu("Camera"))
                {
                    static int mode = 1;
                    if (ImGui::RadioButton("Free Mode",mode == 0))
                    {

                    }

                    if (ImGui::RadioButton("Look At", mode == 1))
                    {

                    }

                    if (ImGui::RadioButton("Scripted", mode == 2))
                    {

                    }

                    ImGui::EndMenu();
                }
                float volume = 1.0f;
                if(ImGui::SliderFloat("Volume",&volume,0.0f,1.0f))
                {

                }
                auto pRuntime = mProject->getProjectRuntime();
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

                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug"))
            {
                auto showLuaDebug = !mLuaDebugWindow->getHidden();
                if (ImGui::Checkbox("Lua Debug Window",&showLuaDebug))
                {
                    mLuaDebugWindow->setHidden(!showLuaDebug);
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
            ImGui::EndMainMenuBar();
        }

        static ImGuiFs::Dialog newDlg;
        newDlg.chooseFolderDialog(newButtonClicked);
        if (strlen(newDlg.getChosenPath())>0)
        {
            /*
                auto path = newDlg.getChosenPath();
                ProjectDirectoryModel dirModel();
                mProject->clear();
            */
        }

        bool openProjectFailed = false;
        static ImGuiFs::Dialog openDlg;
        const char* chosenPath = openDlg.chooseFolderDialog(openButtonClicked);
        if (strlen(chosenPath) > 0)
        {
            auto projectDir = openDlg.getChosenPath();
            log->error("Opening project {}",projectDir);
            if(mProject->openFromDirectory(projectDir))
            {
                mProject->createProjectRuntime();
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

        if (ImGui::BeginPopupModal("Failed to open Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("This directory does not contain a valid Dream Project\n\n");
            ImGui::Separator();

            ImGui::PushItemWidth(-1);
            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopItemWidth();

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        if (showQuit)
        {
            ImGui::OpenPopup("Quit?");
        }

        if (ImGui::BeginPopupModal("Quit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
                MainLoopDone = true;
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        if (saveSuccess)
        {
            ImGui::OpenPopup("Save Success");
        }

        if (ImGui::BeginPopupModal("Save Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Project Saved Successfully!");
            ImGui::Separator();

            ImGui::PushItemWidth(-1);
            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopItemWidth();

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

    }
}
