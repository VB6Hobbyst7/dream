#pragma once

#include "../../TaskManager/Task.h"
#include "../Graphics/GraphicsComponentTask.h"

namespace Dream
{
    class PhysicsObjectRuntime;
    class PhysicsComponent;
    class PhysicsDebugDrawer;

    class PhysicsAddObjectTask : public Task
    {
        PhysicsComponent* mComponent;
        PhysicsObjectRuntime* mRuntime;
    public:
        PhysicsAddObjectTask(PhysicsComponent* cp, PhysicsObjectRuntime* rt);
        void execute();
    };

    class PhysicsUpdateWorldTask : public Task
    {
        PhysicsComponent* mComponent;
    public:
        PhysicsUpdateWorldTask(PhysicsComponent* cp);
        void execute();
    };

    class PhysicsDrawDebugTask : public GraphicsComponentTask
    {
        PhysicsComponent* mComponent;
    public:
        PhysicsDrawDebugTask(PhysicsComponent* cp);
        void execute();
    };
}
