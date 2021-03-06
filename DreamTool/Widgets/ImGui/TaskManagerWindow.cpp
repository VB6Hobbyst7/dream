#include "TaskManagerWindow.h"
#include "../../DTState.h"
#include "../../../DreamCore/Project/Project.h"
#include "../../../DreamCore/Project/ProjectRuntime.h"
#include "../../../DreamCore/TaskManager/Task.h"
#include "../../../DreamCore/TaskManager/TaskThread.h"
#include "../../../DreamCore/TaskManager/TaskManager.h"
#include "../../../DreamCore/Components/Graphics/GraphicsComponent.h"

namespace DreamTool
{
    TaskManagerWindow::TaskManagerWindow
    (DTState* proj) : ImGuiWidget(proj,false)
    {
        #ifdef DREAM_LOG
        setLogClassName("TaskManagerWindow");
        #endif
    }

    TaskManagerWindow::~TaskManagerWindow
    ()
    {

    }

    void
    TaskManagerWindow::draw
    ()
    {
        ImGui::Begin("Task Manager",&mVisible);

        if (mState->project)
        {
            auto projRunt = mState->project->getRuntime();
            if (projRunt)
            {
                auto* taskManager = projRunt->getTaskManager();
                const auto& threadVector = taskManager->getThreadVector();
                ImGui::Text("%lu Threads Running",threadVector.size());
                ImGui::Columns(static_cast<int>(threadVector.size()));
                for (TaskThread* t : threadVector)
                {
                    auto& debugQueue = t->getDebugTaskQueue();
                    ImGui::Text("Worker Thread %d (%3lu Tasks)", t->getThreadId(),debugQueue.size());
                    for (Task* task : debugQueue)
                    {
                        #ifdef DREAM_LOG
                        ImGui::Text("[%.3d/%.3d] %s",task->getTaskId(), task->getDeferralCount(), task->getClassName().c_str());
                        #else
                        ImGui::Text("[%.3d/%.3d]",task->getTaskId(), task->getDeferralCount());
                        #endif
                    }
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::Text("GraphicsComponent Tasks");
                auto* gfxComponent = projRunt->getGraphicsComponent();
                auto gfxTaskQueue = gfxComponent->getDebugTaskQueue();
                for (GraphicsComponentTask* task : gfxTaskQueue)
                {
                    #ifdef DREAM_LOG
                    ImGui::Text("[%.3d/%.3d] %s",task->getTaskId(), task->getDeferralCount(), task->getClassName().c_str());
                    #else
                    ImGui::Text("[%.3d/%.3d]",task->getTaskId(), task->getDeferralCount());
                    #endif
                }
            }
            else
            {
                ImGui::Text("No Project Runtime");
            }
        }
        else
        {
                ImGui::Text("No Project");
        }
        ImGui::End();
    }
}
