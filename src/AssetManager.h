/*
* Dream::AssetManager
*
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


#ifndef AssetManager_h
#define AssetManager_h

#include <iostream>
#include <vector>

#include "AssetInstance.h"
#include "AssetDefinition.h"
#include "Components/Scripting/ChaiScriptInstance.h"
#include "Components/Audio/AudioInstance.h"
#include "Components/Animation/AnimationInstance.h"
#include "Components/Graphics/SpriteInstance.h"
#include "Components/Graphics/ShaderInstance.h"
#include "Components/Graphics/AssimpModelInstance.h"
#include "Components/Graphics/LightInstance.h"
#include "Components/Physics/PhysicsObjectInstance.h"
#include "Scene.h"

namespace Dream {

  class AssetManager {
  private:
    std::vector<AssetInstance*> mAssetInstances;
    std::vector<SceneObject*> mSceneObjectsWithPhysicsObjects;
    std::vector<AssetDefinition*> mAssetDefinitions;
    std::string mProjectPath;
  public:
    AssetManager();
    ~AssetManager();

    bool createAllAssetInstances(Scene*);
    void destroyAllAssetInstances();
    void addAssetInstance(AssetInstance*);

    AssetInstance* getAssetInstanceByUUID(std::string);
    AssetInstance* createAssetInstanceFromDefinitionUUID(SceneObject*, std::string);
    AssetInstance* createAssetInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createAnimationInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createAudioInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createModelInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createScriptInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createShaderInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createPhysicsObjectInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createLightInstance(SceneObject*, AssetDefinition*);
    AssetInstance* createSpriteInstance(SceneObject*, AssetDefinition*);
    void showStatus();
    std::vector<SceneObject*> getSceneObjectsWithPhysicsObjects();

    void loadAssetDefinitionsFromJson(nlohmann::json);
    void addAssetDefinition(AssetDefinition*);
    void removeAssetDefinition(AssetDefinition*);
    size_t getNumberOfAssetDefinitions();
    AssetDefinition* getAssetDefinitionByUUID(std::string);

    void setProjectPath(std::string);

  }; // End of AssetManager
} // End of Dream

#endif /* AssetManager_h */
