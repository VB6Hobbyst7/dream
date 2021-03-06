/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <deque>

#include "ScrollerDefinition.h"
#include "../DiscreteAssetRuntime.h"
#include "ScrollerTasks.h"

namespace Dream
{
    class SceneRuntime;

    class ScrollerRuntime : public DiscreteAssetRuntime
    {
        enum Range
        {
            None,
            PreRange,
            InRange,
            PostRange
        };

    public:
        ScrollerRuntime(ScrollerDefinition*,ActorRuntime*);
        ~ScrollerRuntime() override;
        bool useDefinition() override;
        void update();
        void collectGarbage() override;

        ScrollerUpdateTask* getUpdateTask();

    protected:
        ActorRuntime* createChlidRuntime(const ScrollerItem& item);
        bool removeChlidRuntime(const ScrollerItem& item);
        void addAssets(ActorRuntime* runt);
        Range checkRange(ActorRuntime* runt) const;

        Vector3 mVelocity;
        Vector3 mRangeBegin;
        Vector3 mRangeEnd;
        vector<ActorRuntime*> mPreRange;
        vector<ActorRuntime*> mInRange;
        vector<ActorRuntime*> mPostRange;
        map<uint32_t,uint32_t> mAssetsMap;
        ScrollerUpdateTask mUpdateTask;
    };
}
