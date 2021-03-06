#pragma once

#include "ImGuiWidget.h"
#include "../../../DreamCore/Components/Script/ScriptComponent.h"

using namespace Dream;

namespace DreamTool
{
    class ScriptDebugWindow
            : public ImGuiWidget,
              public ScriptPrintListener
    {
    public:
        ScriptDebugWindow(DTState* proj);
        ~ScriptDebugWindow() override;

        void draw() override;
        void onPrint(const string&) override;
    private:
        ImGuiTextBuffer mLogBuffer;
    };
}
