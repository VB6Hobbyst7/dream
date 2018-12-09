#include "GamepadStateWindow.h"
#include "../../DTState.h"
#include "../../../DreamCore/Project/Project.h"
#include "../../../DreamCore/Project/ProjectRuntime.h"
#include "../../../DreamCore/Components/Input/InputComponent.h"


namespace DreamTool
{
    GamepadStateWindow::GamepadStateWindow
    (DTState* state, bool visible)
        : ImGuiWidget (state,visible)
    {
        setLogClassName("GamepadStateWindow");
    }

    GamepadStateWindow::~GamepadStateWindow
    ()
    {

    }

    void
    GamepadStateWindow::draw
    ()
    {
        if (mState->project)
        {

            ImGui::Begin("Gamepad State",&mVisible);
            auto pRuntime = mState->project->getRuntime();
            if (pRuntime)
            {
                auto inputComp = pRuntime->getInputComponent();
                if (inputComp)
                {
                    JoystickState& js = inputComp->getJoystickState();

                    ImGui::Text("Gamepad: %s",js.Name.c_str());
                    ImGui::Text("Axis");
                    {
                        ImGui::Columns(2);
                        for (int i=0; i<js.AxisCount; i++)
                        {
                            stringstream label;
                            label << "Asix " << i;
                            ImGui::SliderFloat(label.str().c_str(),&js.AxisData[i],-1.0f,1.0f);
                            ImGui::NextColumn();
                        }
                        ImGui::Columns(1);
                    }

                    ImGui::Text("Buttons");
                    {
                        ImGui::Columns(4);
                        for (int i = 0; i < js.ButtonCount; i++ )
                        {
                            stringstream label;
                            label << "Button " << i;
                            ImGui::Checkbox(label.str().c_str(),(bool*)&js.ButtonData[i]);
                            ImGui::NextColumn();
                        }
                        ImGui::Columns(1);
                    }

                }
            }
            ImGui::End();
        }
    }
}
