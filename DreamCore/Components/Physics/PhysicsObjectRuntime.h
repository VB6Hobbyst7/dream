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

#include <string>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

#include "../DiscreteAssetRuntime.h"
#include "../Transform.h"

using namespace std;
using namespace glm;
using namespace nlohmann;

namespace Dream
{
    class PhysicsMotionState;
    class PhysicsObjectDefinition;
    class PhysicsComponent;
    class ModelCache;
    class ModelRuntime;
    class SceneObjectRuntime;

    class PhysicsObjectRuntime : public DiscreteAssetRuntime
    {
    private:
        btCollisionShape *mCollisionShape;
        btMotionState *mMotionState;
        btRigidBody *mRigidBody;
        btRigidBody::btRigidBodyConstructionInfo *mRigidBodyConstructionInfo;
        bool mInPhysicsWorld;
        PhysicsComponent* mPhysicsComponentHandle;
        ModelCache* mModelCache;

        PhysicsObjectDefinition* getAssetDefinitionByUuid(string);
        btCollisionShape* createTriangleMeshShape(ModelRuntime*);

    public:
        PhysicsObjectRuntime(
            PhysicsObjectDefinition*, PhysicsComponent*,
            ModelCache*, SceneObjectRuntime*
        );
        ~PhysicsObjectRuntime() override;
        bool useDefinition() override;
        btCollisionShape* createCollisionShape(PhysicsObjectDefinition*);
        btCollisionShape* getCollisionShape();
        btRigidBody* getRigidBody();
        void getWorldTransform(btTransform&);
        btCollisionObject* getCollisionObject();

        vec3 getCenterOfMassPosition();
        void applyCentralImpulse(vec3);
        void applyTorqueImpulse(vec3);
        void applyForce(vec3);
        void applyTorque(vec3);
        void clearForces();

        void setCenterOfMassTransform(const Transform& tx);
        void setWorldTransform(const Transform& tx);

        vec3 getLinearVelocity();
        void setLinearVelocity(float, float, float);

        bool isInPhysicsWorld();
        void setInPhysicsWorld(bool inPhysicsWorld);

        void setLinearFactor(float x, float y, float z);

        void setAngularFactor(float x, float y, float z);
        void setAngularVelocity(float x, float y, float z);

        float getRestitution() const;
        void setRestitution(float r);

        float getFriction() const;
        void setFriction(float friction);

        float getMass() const;
        void setMass(float mass);

        void  setCcdSweptSphereRadius(float);
        float getCcdSweptSphereRadius();
        void setCenterOfMassTransform(const mat4& tx);
    };
}