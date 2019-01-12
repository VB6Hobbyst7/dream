/*
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 */

#pragma once

#include "../AssetDefinition.h"

namespace Dream
{
    struct ScrollerItem
    {
       unsigned int index;
       uint32_t uuid;
       vec3 offset;
    };

    class ScrollerDefinition : public AssetDefinition
    {
    public:
        ScrollerDefinition(ProjectDefinition*, json);
        ~ScrollerDefinition() override;

        vec3 getVelocity();
        void setVelocity(vec3);

        bool getLoop();
        void setLoop(bool loop);


        vector<ScrollerItem> getItemsArray();
        void addItem(const ScrollerItem& item);
    protected:
        json wrapScrollerItem(const ScrollerItem& item);
        ScrollerItem unwrapScrollerItem(const json& j);

        void checkItemsArray();
    };
}