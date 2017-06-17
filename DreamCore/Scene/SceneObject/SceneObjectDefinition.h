/*
 * SceneObjectDefinition.h
 *
 * Created: 16 2017 by Ashley
 *
 * Copyright 2017 Octronic. All rights reserved.
 *
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

#include <iostream>
#include <vector>

#include "../../Common/IDefinition.h"

using std::string;
using std::vector;

namespace Dream
{
    class SceneObject;
    class SceneObjectRuntime;

    class SceneObjectDefinition : public IDefinition
    {
    private:
        SceneObject* mParentHandle;
    public:
        SceneObjectDefinition(json data, SceneObject* parentHandle = nullptr);

        bool hasUuid(string);
        void setUuid(string);
        string getUuid();

        bool hasName(string);
        void setName(string);
        string getName();
        void applyDataToRuntime(SceneObjectRuntime*);
        void setHasFocus(bool focus);
        bool hasFocus();
        void addAssetDefUuidToLoad(string uuid);
        vector<string> getAssetDefUuidsToLoad();
        void showStatus();
    };
}