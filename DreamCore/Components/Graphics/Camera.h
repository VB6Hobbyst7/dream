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

#define GLM_FORCE_RADIANS

#ifdef WIN32
    #define _USE_MATH_DEFINES // for C++
#endif

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include "../../Scene/Actor/BoundingBox.h"
#include "../../Common/DreamObject.h"
#include "../../Common/Math.h"
#include "../Transform.h"
#include "Frustum.h"

using namespace glm;

namespace Dream
{
    class SceneRuntime;
    class ActorRuntime;

    /**
     * @brief Implements a Camera in 3D Space.
     */
    class Camera
        : public DreamObject
    {
    private:
        // Camera Attributes
        Vector3 mTranslation;
        Vector3 mFront;
        Vector3 mUp;
        Vector3 mRight;
        Vector3 mWorldUp;
        // Eular Angles
        float mYaw;
        float mPitch;
        // Camera options
        float mMovementSpeed;
        mat4 mProjectionMatrix;
        Frustum mFrustum;
        // Focus on SO
        ActorRuntime* mFocusedActor;
        Vector3  mFocusTranslation;
        float mMinimumDraw;
        float mMaximumDraw;
        float mMeshCullDistance;
        SceneRuntime* mSceneRuntime;
    public:
        Camera(SceneRuntime* parent);
        ~Camera();

        mat4 getViewMatrix() const;
        void update();
        void updateCameraVectors();
        void setTranslation(const Vector3&);
        void setTranslation(float,float,float);
        Vector3 getTranslation() const;
        void  setMovementSpeed(float);
        float getMovementSpeed() const;

        void flyForward(float scalar=1.0f);
        void flyBackward(float scalar=1.0f);
        void flyLeft(float scalar=1.0f);
        void flyRight(float scalar=1.0f);
        void flyUp(float scalar=1.0f);
        void flyDown(float scalar=1.0f);

        void deltaPitch(float pitch);
        void deltaYaw(float yaw);

        float getFocusedObjectTheta() const;

        Vector3 getUp() const;
        Vector3 getFront() const;

        float getYaw() const;
        void setYaw(float yaw);

        float getPitch() const;
        void setPitch(float pitch);

        bool containedInFrustum(ActorRuntime*) const;
        bool containedInFrustum(const BoundingBox&) const;
        bool containedInFrustumAfterTransform(ActorRuntime*,const mat4& tx) const;
        bool exceedsFrustumPlaneAtTranslation(Frustum::Plane plane, ActorRuntime*sor, const Vector3& tx) const;

        bool visibleInFrustum(ActorRuntime*)const;
        bool visibleInFrustum(const BoundingBox&) const;

        mat4 getProjectionMatrix() const;
        void setProjectionMatrix(const mat4& projectionMatrix);

        void setFocusedSceneObejct(ActorRuntime*);
        ActorRuntime* getFocusedActor() const;

        float getFocusPitch() const;
        void setFocusPitch(float focusPitch);

        float getFocusYaw() const;
        void setFocusYaw(float focusYaw);

        float getFocusRadius() const;
        void setFocusRadius(float focusRadius);

        float getFocusElevation() const;
        void setFocusElevation(float focusElevation);

        float getMeshCullDistance() const;
        void setMeshCullDistance(float meshCullDistance);

        float getMinimumDraw() const;
        void setMinimumDraw(float minimumDraw);

        float getMaximumDraw() const;
        void setMaximumDraw(float maximumDraw);

        void updateProjectionMatrix(float w, float h);
        bool visibleInFrustum(const BoundingBox& bb,const mat4& tx) const;

    private:
        void setFocusTranslationFromTarget(const Vector3& target);
    };
}
