/*
 * Grid.h
 *
 * Created: 07 2017 by Ashley
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
#include "GLWidget.h"

using namespace glm;
using namespace std;

namespace DreamTool
{
    class Grid : public GLWidget
    {
    public:
        Grid(
            Project* p,
            float majorSpacing = 10.0f,
            float minorSpacing = 1.0f,
            float size = 100.0f,
            vec3 majorColour = vec3(1.0f),
            vec3 minorColour = vec3(0.6f)
        );

        ~Grid() override;
        void init();
        float getMajorSpacing();
        float getMinorSpacing();

    protected: // Member functions
        void initMajorGridData();
        void initMinorGridData();
        void initAxisLines();

    protected: // Variables
        float mSize;
        float mMajorSpacing;
        float mMinorSpacing;
        vec3 mMajorColour;
        vec3 mMinorColour;
    };

}
