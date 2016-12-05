/*
* Dream::Scene
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

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>
#include <json.hpp>

#include "SceneObject.h"
#include "Components/Graphics/Camera.h"
#include "String.h"
#include "Constants.h"

namespace Dream {

  using namespace std;

  class Scene {
  public:
    string mUUID;
    string mName;
    SceneObject *mRootSceneObject;
    vector<SceneObject*> mScenegraphVector;
    Transform3D* mDefaultCameraTransform;
    float mCameraMovementSpeed;
  public:
    Scene();
    Scene(nlohmann::json);
    ~Scene();
    bool init();
    string getUUID();
    void setUUID(string);
    string getName();
    void setName(string);
    void loadSceneObjects(nlohmann::json,SceneObject*);
    void loadDefaultCameraTransform(nlohmann::json);
    int countChildrenOfSceneObject(SceneObject*);
    void setRootSceneObject(SceneObject*);
    SceneObject* getRootSceneObject();
    size_t getNumberOfSceneObjects();
    bool hasSceneObect(SceneObject*);
    SceneObject* getSceneObjectByName(string);
    SceneObject* getSceneObjectByUUID(string);
    string generateSceneObjectPath(SceneObject*);
    void showStatus();
    void showScenegraph();
    string indent(int);
    void generateScenegraphVector();
    void generateSceneObjectPaths();
    bool isScenegraphVectorEmpty();
    vector<SceneObject*> getScenegraphVector();
    vector<float> getDefaultCameraTranslation();
    vector<float> getDefaultCameraRotation();
    void setCameraMovementSpeed(float);
    float getCameraMovementSpeed();
  }; // End of Scene

} // End of Dream

#endif // End of SCENE_H