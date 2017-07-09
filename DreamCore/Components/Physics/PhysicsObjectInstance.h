#pragma once

#include "../IAssetInstance.h"

#include <string>
#include <map>
#include <vector>
#include <json.hpp>

#include <assimp/Importer.hpp>
#include <btBulletDynamicsCommon.h>

using std::string;
using std::map;
using std::vector;

using nlohmann::json;

struct aiMesh;
struct aiNode;

namespace Dream
{
    class PhysicsMotionState;

    class PhysicsObjectInstance : public IAssetInstance
    {

    private:
        static map<string,const aiScene*> AssimpModelCache;
        static ::Assimp::Importer mImporter;
        static const aiScene* getModelFromCache(string);
        btCollisionShape *mCollisionShape;
        btMotionState *mMotionState;
        btRigidBody *mRigidBody;
        btRigidBody::btRigidBodyConstructionInfo *mRigidBodyConstructionInfo;
        bool mInPhysicsWorld;

    public:
        PhysicsObjectInstance(AssetDefinition*,SceneObjectRuntime*);
        ~PhysicsObjectInstance();
        bool load(string);
        btCollisionShape* createCollisionShape(AssetDefinition*, string projectPath);
        btCollisionShape* getCollisionShape();
        btRigidBody* getRigidBody();
        void getWorldTransform(btTransform&);
        btCollisionObject* getCollisionObject();
        void setLinearVelocity(float, float, float);

        bool isInPhysicsWorld();
        void setInPhysicsWorld(bool inPhysicsWorld);

    protected:
        void loadExtraAttributes(json);
        void loadExtraAttributes(json,AssetDefinition*,bool);
        void processAssimpNode(aiNode*,const aiScene*, btTriangleMesh* triMesh);
        void processAssimpMesh(aiMesh*, btTriangleMesh*);
        AssetDefinition* getAssetDefinitionByUuid(string);

    }; // End of PhysicsObjectInstance

} // End of Dream
