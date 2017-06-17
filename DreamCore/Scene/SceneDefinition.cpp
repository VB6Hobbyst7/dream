/*
 * SceneDefinition.cpp
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
#include "SceneDefinition.h"

#include "../Common/Constants.h"
#include "../Components/Transform3D.h"

#include "SceneObject/SceneObjectDefinition.h"

using std::cout;
using std::endl;

namespace Dream
{
    SceneDefinition::SceneDefinition(json data)
        : IDefinition(data)
    {
        if (Constants::DEBUG)
        {
            cout << "SceneDefinition: Constructing Object" << endl;
        }

        nlohmann::json sceneObjects = mJson[Constants::SCENE_SCENE_OBJECTS];

        if (!sceneObjects.is_null() && sceneObjects.is_array())
        {
            loadSceneObjectDefinition(sceneObjects);
        }
    }

    SceneDefinition::~SceneDefinition()
    {
        if (Constants::DEBUG)
        {
            cout << "SceneDefinition: Destructing Object" << endl;
        }
    }

    void
    SceneDefinition::showStatus
    ()
    {
        if (Constants::DEBUG)
        {
            cout << "SceneDefinition: " << mJson.dump(1) << endl;
        }
    }

    void
    SceneDefinition::loadSceneObjectDefinition
    (nlohmann::json jsonArray)
    {
        if (!jsonArray.is_null())
        {
            for (nlohmann::json it : jsonArray)
            {
                mSceneObjectDefinitions.push_back
                (
                    unique_ptr<SceneObjectDefinition>(new SceneObjectDefinition(it))
                );

                if (!it[Constants::SCENE_OBJECT_CHILDREN].is_null())
                {
                    loadSceneObjectDefinition(it[Constants::SCENE_OBJECT_CHILDREN]);
                }
            }
        }
    }

    void
    SceneDefinition::setCameraMovementSpeed
    (float speed)
    {
        mJson[Constants::SCENE_CAMERA][Constants::SCENE_MOVEMENT_SPEED] = speed;
    }

    float
    SceneDefinition::getCameraMovementSpeed
    ()
    {
        return mJson[Constants::SCENE_CAMERA][Constants::SCENE_MOVEMENT_SPEED];
    }

    bool
    SceneDefinition::getPhysicsDebug
    ()
    {
        return mJson[Constants::SCENE_PHYSICS_DEBUG];
    }

    string
    SceneDefinition::getNotes
    ()
    {
        return mJson[Constants::SCENE_NOTES];
    }

    void
    SceneDefinition::setNotes
    (string notes)
    {
        mJson[Constants::SCENE_NOTES] = notes;
    }

    Transform3D
    SceneDefinition::getDefaultCameraTransform
    ()
    {
        Transform3D defaultCameraTransform;
        nlohmann::json camera = mJson[Constants::SCENE_CAMERA];

        if (!camera.is_null())
        {
            nlohmann::json translation = camera[Constants::SCENE_TRANSLATION];
            defaultCameraTransform.setTranslation(
                translation[Constants::X],
                translation[Constants::Y],
                translation[Constants::Z]
            );
            nlohmann::json rotation = camera[Constants::SCENE_ROTATION];
            defaultCameraTransform.setRotation(
                rotation[Constants::X],
                rotation[Constants::Y],
                rotation[Constants::Z]
            );
        }

        return defaultCameraTransform;
    }

    vector<float>
    SceneDefinition::getGravity
    ()
    const
    {
        vector<float> gravity;
        gravity.reserve(Constants::XYZ_VECTOR_SIZE);

        if (mJson[Constants::SCENE_GRAVITY].is_null())
        {
            return gravity;
        }

        gravity[Constants::X_INDEX] = mJson[Constants::SCENE_GRAVITY][Constants::X];
        gravity[Constants::Y_INDEX] = mJson[Constants::SCENE_GRAVITY][Constants::Y];
        gravity[Constants::Z_INDEX] = mJson[Constants::SCENE_GRAVITY][Constants::Z];

        return gravity;
    }

    void
    SceneDefinition::setGravity
    (const vector<float> &gravity)
    {
        if (mJson[Constants::SCENE_GRAVITY].is_null())

        {
            mJson[Constants::SCENE_GRAVITY] = {};
        }

        mJson[Constants::SCENE_GRAVITY][Constants::X] = gravity[Constants::X_INDEX];
        mJson[Constants::SCENE_GRAVITY][Constants::Y] = gravity[Constants::Y_INDEX];
        mJson[Constants::SCENE_GRAVITY][Constants::Z] = gravity[Constants::Z_INDEX] ;
    }

    vector<float>
    SceneDefinition::getClearColour
    ()
    const
    {
        vector<float> colour;
        colour.reserve(Constants::RGB_VECTOR_SIZE);

        if (mJson[Constants::SCENE_CLEAR_COLOUR].is_null())
        {
            return colour;
        }

        colour[Constants::RED_INDEX]   = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::RED];
        colour[Constants::GREEN_INDEX] = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::GREEN];
        colour[Constants::BLUE_INDEX]  = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::BLUE];

        return colour;
    }

    void
    SceneDefinition::setClearColour
    (const vector<float> &colour)
    {
        if (mJson[Constants::SCENE_CLEAR_COLOUR].is_null())
        {
            mJson[Constants::SCENE_CLEAR_COLOUR] = {};
        }

        mJson[Constants::SCENE_CLEAR_COLOUR][Constants::RED]   = colour[Constants::RED_INDEX];
        mJson[Constants::SCENE_CLEAR_COLOUR][Constants::GREEN] = colour[Constants::GREEN_INDEX];
        mJson[Constants::SCENE_CLEAR_COLOUR][Constants::BLUE]  = colour[Constants::BLUE_INDEX] ;
    }

    vector<float>
    SceneDefinition::getAmbientColour
    ()
    const
    {
        vector<float> colour;
        colour.reserve(Constants::RGBA_VECTOR_SIZE);

        if (mJson[Constants::SCENE_CLEAR_COLOUR].is_null())
        {
            return colour;
        }

        colour[Constants::RED_INDEX]   = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::RED];
        colour[Constants::GREEN_INDEX] = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::GREEN];
        colour[Constants::BLUE_INDEX]  = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::BLUE];
        colour[Constants::ALPHA_INDEX] = mJson[Constants::SCENE_CLEAR_COLOUR][Constants::ALPHA];

        return colour;
    }

    void
    SceneDefinition::setAmbientColour
    (const vector<float> &colour)
    {
        if (mJson[Constants::SCENE_CLEAR_COLOUR].is_null())
        {
            mJson[Constants::SCENE_CLEAR_COLOUR] = {};
        }

        mJson[Constants::SCENE_AMBIENT_LIGHT_COLOUR][Constants::RED]   = colour[Constants::RED_INDEX];
        mJson[Constants::SCENE_AMBIENT_LIGHT_COLOUR][Constants::GREEN] = colour[Constants::GREEN_INDEX];
        mJson[Constants::SCENE_AMBIENT_LIGHT_COLOUR][Constants::BLUE]  = colour[Constants::BLUE_INDEX] ;
        mJson[Constants::SCENE_AMBIENT_LIGHT_COLOUR][Constants::ALPHA] = colour[Constants::ALPHA_INDEX];
    }

    size_t
    SceneDefinition::countSceneObjectDefinitions
    ()
    {
        return mSceneObjectDefinitions.size();
    }

}