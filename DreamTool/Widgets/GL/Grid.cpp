/*
 * Grid.cpp
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
#include "Grid.h"
#include <DreamCore.h>

using namespace Dream;
namespace DreamTool
{

    Grid::Grid
    (
        Project* p,
        AxisPair xp,
        glm::vec3 position,
        float majorSpacing,
        float minorSpacing,
        float size,
        vec3 majorColour,
        vec3 minorColour
    )
        : GLWidget(p),
          mTranslation(position),
          mAxisPair(xp),
          mSize(size),
          mMajorSpacing(majorSpacing),
          mMinorSpacing(minorSpacing),
          mMajorColour(majorColour),
          mMinorColour(minorColour)

    {
        setLogClassName("Grid");
        auto log = getLog();
        log->debug("Constructing with majorSpacing: {}, minorSpacing {}", mMajorSpacing, minorSpacing);
    }

    Grid::~Grid
    ()
    {
        auto log = getLog();
        log->debug("Destructing");
    }

    void
    Grid::init
    ()
    {
        GLWidget::init();
        recalculateGridLines();
    }

    void
    Grid::initMajorGridData
    ()
    {
        auto log = getLog();
        log->debug("Init Major Data");
        float halfSize = (mSize/2.0f);

        // Major Grid
        for (float z = -halfSize; z <= halfSize; z += mMajorSpacing)
        {
            LineVertex majorStart, majorEnd;

            majorStart.Position = vec3(-halfSize, 0.0f, z);
            majorStart.Color = mMajorColour;

            majorEnd.Position = vec3(halfSize, 0.0f, z);
            majorEnd.Color = mMajorColour;

            mVertexBuffer.push_back(majorStart);
            mVertexBuffer.push_back(majorEnd);
        }

        for (float x = -halfSize; x <= halfSize; x += mMajorSpacing)
        {
            LineVertex majorStart, majorEnd;

            majorStart.Position = vec3(x, 0.0f, -halfSize);
            majorStart.Color = mMajorColour;

            majorEnd.Position = vec3(x, 0.0f, halfSize);
            majorEnd.Color = mMajorColour;

            mVertexBuffer.push_back(majorStart);
            mVertexBuffer.push_back(majorEnd);
        }
    }

    void
    Grid::initMinorGridData
    ()
    {
        auto log = getLog();
        log->debug("Init Minor Data");
        float halfSize = (mSize/2.0f);

        // Minor Grid
        for (float z = -halfSize; z <= halfSize; z += mMinorSpacing)
        {

            if (static_cast<int>(z) % static_cast<int>(mMajorSpacing) == 0) continue;

            LineVertex minorStart, minorEnd;

            minorStart.Position = vec3(-halfSize, 0.0f, z);
            minorStart.Color = mMinorColour;

            minorEnd.Position = vec3(halfSize, 0.0f, z);
            minorEnd.Color = mMinorColour;

            mVertexBuffer.push_back(minorStart);
            mVertexBuffer.push_back(minorEnd);
        }

        for (float x = -halfSize; x <= halfSize; x += mMinorSpacing)
        {

            if (static_cast<int>(x) % static_cast<int>(mMajorSpacing) == 0) continue;

            LineVertex minorStart, minorEnd;

            minorStart.Position = vec3(x, 0.0f, -halfSize);
            minorStart.Color = mMinorColour;

            minorEnd.Position = vec3(x, 0.0f, halfSize);
            minorEnd.Color = mMinorColour;

            mVertexBuffer.push_back(minorStart);
            mVertexBuffer.push_back(minorEnd);
        }
    }

    void Grid::initAxisLines()
    {
        vec3 red(1,0,0);
        vec3 green(0,1,0);
        vec3 blue(0,0,1);

        LineVertex xStart, xEnd;
        xStart.Position = vec3(0);
        xStart.Color = red;
        xEnd.Position = vec3(mSize,0,0);
        xEnd.Color = red;
        mVertexBuffer.push_back(xStart);
        mVertexBuffer.push_back(xEnd);

        LineVertex yStart, yEnd;
        yStart.Position = vec3(0);
        yStart.Color = green;
        yEnd.Position = vec3(0,mSize,0);
        yEnd.Color = green;
        mVertexBuffer.push_back(yStart);
        mVertexBuffer.push_back(yEnd);

        LineVertex zStart, zEnd;
        zStart.Position = vec3(0);
        zStart.Color = blue;
        zEnd.Position = vec3(0,0,mSize);
        zEnd.Color = blue;
        mVertexBuffer.push_back(zStart);
        mVertexBuffer.push_back(zEnd);
    }

    vec3 Grid::getMinorColour() const
    {
        return mMinorColour;
    }

    void Grid::setMinorColour(vec3 minorColour)
    {
        mMinorColour = minorColour;
    }

    vec3 Grid::getMajorColour() const
    {
        return mMajorColour;
    }

    void Grid::setMajorColour(vec3 majorColour)
    {
        mMajorColour = majorColour;
    }

    glm::vec3 Grid::getTranslation() const
    {
        return mTranslation;
    }

    void Grid::setTranslation(vec3 translation)
    {
        mTranslation = translation;
        mModelMatrix = glm::translate(mat4(1.0f),mTranslation);
    }

    float
    Grid::getMajorSpacing
    ()
    {
        return mMajorSpacing;
    }

    void
    Grid::setMajorSpacing
    (float ms)
    {
        mMajorSpacing = ms;
    }

    float
    Grid::getMinorSpacing
    ()
    {
        return mMinorSpacing;
    }

    void
    Grid::setMinorSpacing
    (float ms)
    {
        mMinorSpacing = ms;
    }

    float Grid::getSize()
    {
        return mSize;
    }

    void Grid::setSize(float sz)
    {
        mSize = sz;
    }

    void Grid::recalculateGridLines()
    {
        mVertexBuffer.clear();
        initMinorGridData();
        initMajorGridData();
        initAxisLines();
    }
}