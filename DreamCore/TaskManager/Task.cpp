#include "Task.h"

#include <algorithm>

using std::find;

namespace Dream
{
        Task::Task()
            : DreamObject("Task"),
            mThreadId(-1),
            mState(CLEAR),
            mDeferralCount(0)
        {
            clearState();
        }

        Task::~Task() {}

        void Task::incrementDeferralCount()
        {
            mDeferralCount++;
        }

        void Task::setThreadId(int t)
        {
            mThreadId = t;
        }

        unsigned int Task::getDeferralCount()
        {
            return mDeferralCount;
        }

        void Task::clearState()
        {
            mWaitingForMutex.lock();
            mWaitingFor.clear();
            mWaitingForMutex.unlock();

            mWaitingForMeMutex.lock();
            mWaitingForMe.clear();
            mWaitingForMeMutex.unlock();

            mThreadId = -1;
            mState = TaskState::CLEAR;
            mDeferralCount = 0;
        }

        void Task::clearDependency(Task* t)
        {
            mWaitingForMutex.lock();
            auto itr = std::find(mWaitingFor.begin(), mWaitingFor.end(), t);
            if (itr != mWaitingFor.end())
            {
                mWaitingFor.erase(itr);
            }
            else
            {
                #ifdef DREAM_LOG
                getLog()->critical("*** WHAT THE F JEFF!!! *** {} was not waiting for {}",getClassName(), t->getClassName());
                #endif
            }
            mWaitingForMutex.unlock();
        }

        void Task::notifyDependents()
        {
           mWaitingForMeMutex.lock();
           for (Task* t : mWaitingForMe)
           {
               #ifdef DREAM_LOG
               getLog()->critical("is notifying dependant {} of completion",t->getClassName());
               #endif
               t->clearDependency(this);
           }
           mWaitingForMe.clear();
           mWaitingForMeMutex.unlock();
        }

        bool Task::isWaitingForDependencies()
        {
            mWaitingForMutex.lock();
            bool retval = !mWaitingFor.empty();
            mWaitingForMutex.unlock();
            #ifdef DREAM_LOG
            if (retval)
            {
                mState = TaskState::WAITING;
                getLog()->critical("Waiting for {} dependencies to finish",mWaitingFor.size());
            }
            #endif
            return retval;
        }

        void Task::dependsOn(Task* t)
        {
            mWaitingForMutex.lock();
            mWaitingFor.push_back(t);
            mWaitingForMutex.unlock();

            t->mWaitingForMeMutex.lock();
            t->mWaitingForMe.push_back(this);
            t->mWaitingForMeMutex.unlock();
        }

        void Task::setState(const TaskState& s)
        {
            mState = s;;
        }

        TaskState Task::getState() const
        {
            return mState;
        }
}