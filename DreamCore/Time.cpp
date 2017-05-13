#include "Time.h"

namespace Dream
{
  Time::Time() : ILuaExposable()
  {
    mCurrentTime = chrono::high_resolution_clock::now();
    mLastTime = chrono::high_resolution_clock::now();
  }

  Time::~Time()
  {

  }

  void Time::update()
  {
    if (VERBOSE)
    {
      cout << "Time: Update Called" << endl;
    }
    mLastTime    = mCurrentTime;
    mCurrentTime = chrono::high_resolution_clock::now();
    mTimeDelta   = mCurrentTime-mLastTime;
    //show();
    return;
  }

  void Time::show()
  {
    if (DEBUG)
    {
      cout << "Time: " << endl
           << "\tCurrent Time: " << getCurrentTime() << endl
           << "\t   Last Time: " << getLastTime() << endl
           << "\t  Time Delta: " << getTimeDelta() << endl;
    }
  }

  double Time::getCurrentTime()
  {
    return mCurrentTime.time_since_epoch().count();
  }

  double Time::getLastTime()
  {
    return mLastTime.time_since_epoch().count();
  }

  double Time::getTimeDelta()
  {
    return mTimeDelta.count();
  }

    void Time::exposeLuaApi(lua_State* state)
    {
        luabind::module(state) [
                luabind::class_<Time>("Time")
                .def("getCurrentTime",&Time::getCurrentTime)
                .def("getLastTime",&Time::getLastTime)
                .def("getTimeDelta",&Time::getTimeDelta)
                ];
    }

} // End of Dream