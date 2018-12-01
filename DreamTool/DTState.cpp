#include "DTState.h"
#include "../DreamCore/Scene/SceneRuntime.h"
#include "../DreamCore/Project/Project.h"
#include "../DreamCore/Project/ProjectDefinition.h"
#include "../DreamCore/Project/ProjectRuntime.h"
#include "../DreamCore/Components/Graphics/Shader/ShaderInstance.h"
#include "../DreamCore/Components/Input/InputComponent.h"

using namespace Dream;

namespace DreamTool
{
    DTState::DTState(int _argc, char** _argv)
        : DreamObject("DTState"),
          propertiesWindow(PropertiesWindow(this)),
          projectBrowser(ProjectBrowser(this)),
          luaDebugWindow(LuaDebugWindow(this)),
          sceneStateWindow(SceneStateWindow(this)),
          gridPropertiesWindow(GridPropertiesWindow(this)),
          scriptEditorWindow(ScriptEditorWindow(this)),
          shaderEditorWindow(ShaderEditorWindow(this)),
          menuBar(MenuBar(this)),
          renderPipelineWindow(RenderPipelinePreviewWindow(this)),
          gamepadStateWindow(GamepadStateWindow(this)),
          grid(Grid(this)),
          lightViewer(LightViewer(this)),
          selectionHighlighter(SelectionHighlighter(this)),
          cursor(Cursor(this)),
          modelDefinitionBatchImporter(ModelDefinitionBatchImporter(this)),
          argc(_argc),
          argv(_argv)
    {

    }

    DTState::~DTState()
    {

    }

    void DTState::init()
    {
        project = new Dream::Project(&windowComponent);
#ifdef WIN32
        windowComponent.setUiFontSize(24.0f);
        windowComponent.setMonoFontSize(24.0f);
#else
        windowComponent.setUiFontSize(16.0f);
        windowComponent.setMonoFontSize(18.0f);
#endif
        windowComponent.init();


        ScriptComponent::AddPrintListener(&luaDebugWindow);

        // ImGui Widgets
        windowComponent.addImGuiWidget(&propertiesWindow);
        windowComponent.addImGuiWidget(&projectBrowser);
        windowComponent.addImGuiWidget(&luaDebugWindow);
        windowComponent.addImGuiWidget(&sceneStateWindow);
        windowComponent.addImGuiWidget(&menuBar);
        windowComponent.addImGuiWidget(&gridPropertiesWindow);
        windowComponent.addImGuiWidget(&scriptEditorWindow);
        windowComponent.addImGuiWidget(&shaderEditorWindow);
        windowComponent.addImGuiWidget(&renderPipelineWindow);
        windowComponent.addImGuiWidget(&gamepadStateWindow);

        // GL Widgets
        grid.init();
        lightViewer.init();
        selectionHighlighter.init();
        cursor.init();
        windowComponent.addGLWidget(&grid);
        windowComponent.addGLWidget(&lightViewer);
        windowComponent.addGLWidget(&selectionHighlighter);
        windowComponent.addGLWidget(&cursor);
    }

    void DTState::run()
    {
        auto log = getLog();
        if (argc > 1)
        {
            if(project->openFromDirectory(argv[1]))
            {
                project->createProjectRuntime();
                stringstream ss;
                ss  << "Opened Project: "
                    << project->getProjectDefinition()->getName();
                menuBar.setMessageString(ss.str());
            }
        }

        // Run the project
        while (!MainLoopDone)
        {
            if (windowComponent.shouldClose())
            {
                MainLoopDone = true;
            }

            auto projectRuntime = project->getProjectRuntime();

            if (projectRuntime == nullptr)
            {
                windowComponent.updateComponent(nullptr);
                glClearColor(0.0f,0.0f,0.0f,0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else if (projectRuntime)
            {
                if(!projectRuntime->hasActiveScene())
                {
                    windowComponent.updateComponent(nullptr);
                    glClearColor(0.0f,0.0f,0.0f,0.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }
                projectRuntime->updateAll();
            }

            ShaderInstance::InvalidateState();
            windowComponent.drawGLWidgets();
            windowComponent.drawImGui();
            if (ImGui::IsKeyPressed(GLFW_KEY_TAB,false))
            {
                switch (inputTarget)
                {
                    case EDITOR:
                        inputTarget = SCENE;
                        break;
                    case SCENE:
                        inputTarget = EDITOR;
                        break;
                }
            }

            if (projectRuntime)
            {

                auto sr = projectRuntime->getActiveSceneRuntime();
                if (sr)
                {
                    switch (inputTarget)
                    {
                        case InputTarget::EDITOR:
                            handleEditorInput(sr);
                            break;
                        case InputTarget::SCENE:
                            handleSceneInput(sr);
                            break;
                    }
                }
            }
            windowComponent.swapBuffers();
            FPS();
        }
    }

    void DTState::FPS()
    {
        LastTime = CurrentTime;
        CurrentTime = glfwGetTime();
        Frames = 1.0/(CurrentTime-LastTime);
        menuBar.setFPS(Frames);
    }

    void
    DTState::handleEditorInput
    (SceneRuntime* sr)
    {
        static ImGuiIO& io = ImGui::GetIO();
        static float mouseScalar = 0.001f;
        auto camera = sr->getCamera();
        bool sendKeysToCamera = false;
        if (!io.WantCaptureMouse)
        {
#ifdef __APPLE__
            if (io.MouseDown[0])
            {
                camera->deltaYaw(io.MouseDelta.x*mouseScalar);
                camera->deltaPitch(-io.MouseDelta.y*mouseScalar);
                sendKeysToCamera = true;
            }
#else
            if (io.MouseDown[2])
            {
                camera->deltaYaw(io.MouseDelta.x*mouseScalar);
                camera->deltaPitch(-io.MouseDelta.y*mouseScalar);
                sendKeysToCamera = true;
            }
#endif
        }

        if (!io.WantCaptureKeyboard)
        {
            if (sendKeysToCamera)
            {
                if (ImGui::IsKeyDown(GLFW_KEY_W))
                {
                    camera->flyForward();
                }
                if (ImGui::IsKeyDown(GLFW_KEY_S))
                {
                    camera->flyBackward();
                }
                if (ImGui::IsKeyDown(GLFW_KEY_A))
                {
                    camera->flyLeft();
                }
                if (ImGui::IsKeyDown(GLFW_KEY_D))
                {
                    camera->flyRight();
                }
                if (ImGui::IsKeyDown(GLFW_KEY_Q))
                {
                    camera->flyDown();
                }
                if (ImGui::IsKeyDown(GLFW_KEY_E))
                {
                    camera->flyUp();
                }
            }
            // Send to cursor
            else
            {
                // Z
                if (ImGui::IsKeyPressed(GLFW_KEY_W))
                {
                    cursor.onAction(Cursor::ZPlus);
                }
                if (ImGui::IsKeyPressed(GLFW_KEY_S))
                {
                    cursor.onAction(Cursor::ZMinus);
                }

                // X
                if (ImGui::IsKeyPressed(GLFW_KEY_A))
                {
                    cursor.onAction(Cursor::XMinus);
                }
                if (ImGui::IsKeyPressed(GLFW_KEY_D))
                {
                    cursor.onAction(Cursor::XPlus);
                }

                // Y
                if (ImGui::IsKeyPressed(GLFW_KEY_Q))
                {
                    cursor.onAction(Cursor::YMinus);
                }
                if (ImGui::IsKeyPressed(GLFW_KEY_E))
                {
                    cursor.onAction(Cursor::YPlus);
                }
            }
        }
    }

    void
    DTState::handleSceneInput
    (SceneRuntime* sRunt)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto pRunt = project->getProjectRuntime();
        if (pRunt)
        {
            auto inputComp = pRunt->getInputComponent();
            if (inputComp)
            {
                MouseState& ms = inputComp->getMouseState();
                KeyboardState& ks = inputComp->getKeyboardState();
                JoystickState& js = inputComp->getJoystickState();

                // Mouse
                memcpy(ms.ButtonsDown, io.MouseDown, sizeof(bool)*5);
                ms.PosX = io.MousePos.x;
                ms.PosY = io.MousePos.y;
                ms.ScrollX = io.MouseWheelH;
                ms.ScrollY = io.MouseWheel;

                // Keys
                memcpy(ks.KeysDown, io.KeysDown, sizeof(bool)*512);

                // Joystick
                for (int id=GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST; id++)
                {

                    if (glfwJoystickPresent(id))
                    {
                        js.Name = glfwGetJoystickName(id);
                        int numAxis, numButtons;
                        const float* axisData = glfwGetJoystickAxes(id,&numAxis);
                        const unsigned char* buttonData = glfwGetJoystickButtons(id, &numButtons);
                        if (axisData != nullptr)
                        {
                            js.AxisCount = numAxis;
                            memcpy(&js.AxisData[0],axisData,sizeof(float)*numAxis);
                        }
                        else
                        {
                            js.AxisCount = 0;
                        }
                        if (buttonData != nullptr)
                        {
                            js.ButtonCount = numButtons;
                            memcpy(&js.ButtonData[0],buttonData,sizeof(unsigned char)*numButtons);
                        }
                        else
                        {
                            js.ButtonCount = 0;
                        }
                    }
                }
            }
        }
    }
}
