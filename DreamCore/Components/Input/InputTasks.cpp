#include "InputTasks.h"
#include "InputComponent.h"
#include "../../Scene/SceneRuntime.h"

namespace Dream
{
    InputPollDataTask::InputPollDataTask
    (InputComponent* cp)
        : Task(), mComponent(cp)
    {
        #ifdef DREAM_LOG
        setLogClassName("InputPollDataTask");
        #endif
    }

    void
    InputPollDataTask::execute
    ()
    {
        if (mComponent->tryLock())
        {
            #ifdef DREAM_LOG
            getLog()->critical("Executing on worker {}",mThreadId);
            #endif
            mComponent->pollData();
            mComponent->unlock();
            setState(TaskState::COMPLETED);
        }
        else
        {
            setState(TaskState::WAITING);
            mDeferralCount++;
        }
    }

    InputExecuteScriptTask::InputExecuteScriptTask
    (InputComponent* cp)
        : Task(), mComponent(cp)
    {
        #ifdef DREAM_LOG
        setLogClassName("InputExecuteScriptTask");
        #endif
    }

    void InputExecuteScriptTask::execute()
    {
        #ifdef DREAM_LOG
        getLog()->critical("Executing on thread {}",mThreadId);
        #endif

        if (mComponent->tryLock())
        {
            #ifdef DREAM_LOG
            getLog()->critical("Executing...");
            #endif

            if (mComponent->executeInputScript())
            {
                setState(TaskState::COMPLETED);
            }
            else
            {
                setState(TaskState::WAITING);
                mDeferralCount++;
            }
            mComponent->unlock();
        }
        else
        {
            mDeferralCount++;
        }
    }
}
