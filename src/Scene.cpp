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

#include "Scene.h"

namespace Dream {

  Scene::Scene() {
    mRootSceneObject = nullptr;
    mDefaultCameraTransform = new Transform3D();
  }

  Scene::Scene(nlohmann::json jsonScene) {
    mRootSceneObject = nullptr;
    mDefaultCameraTransform = new Transform3D();
    mUUID = jsonScene[SCENE_JSON_UUID];
    mName = jsonScene[SCENE_JSON_NAME];

    loadDefaultCameraTransform(jsonScene[SCENE_JSON_CAMERA]);

    nlohmann::json sceneObjects = jsonScene[SCENE_JSON_SCENE_OBJECTS];

    if (!sceneObjects.is_null() && sceneObjects.is_array()) {
      loadSceneObjects(sceneObjects,nullptr);
    }
  }

  void Scene::loadDefaultCameraTransform(nlohmann::json camera) {
    if (!camera.is_null()) {
      nlohmann::json translation = camera[SCENE_JSON_TRANSLATION];
      mDefaultCameraTransform->setTranslation({
        translation[SCENE_JSON_X],
        translation[SCENE_JSON_Y],
        translation[SCENE_JSON_Z]
      });

      nlohmann::json rotation = camera[SCENE_JSON_ROTATION];
      mDefaultCameraTransform->setRotation({
        rotation[SCENE_JSON_X],
        rotation[SCENE_JSON_Y],
        rotation[SCENE_JSON_Z]
      });

      if (!camera[SCENE_JSON_MOVEMENT_SPEED].is_null()) {
        setCameraMovementSpeed(camera[SCENE_JSON_MOVEMENT_SPEED]);
      }

    } else {
      mDefaultCameraTransform->setTranslation({0.0f, 0.0f, 0.0f});
      mDefaultCameraTransform->setRotation   ({0.0f, 0.0f, 0.0f});
    }
  }

  Scene::~Scene() {
    if (mDefaultCameraTransform != nullptr) {
        delete mDefaultCameraTransform;
    }
  }

  bool Scene::init() {
    cout << "Scene: Initialising Scene " << getName() << "(" << getUUID() << ")" << endl;
    return true;
  }

  bool Scene::isScenegraphVectorEmpty() {
    return mScenegraphVector.empty();
  }

  string Scene::getName() {
    return mName;
  }

  string Scene::getUUID() {
    return mUUID;
  }

  void Scene::setUUID(string uuid) {
    mUUID = uuid;
  }

  void Scene::setName(string name) {
    mName = name;
  }

  void Scene::loadSceneObjects(nlohmann::json jsonArray, SceneObject* parent) {
    cout << "Loading scene objects from array: "<< jsonArray.dump() << endl;
    if (!jsonArray.is_null()) {
      for (nlohmann::json::iterator it = jsonArray.begin(); it != jsonArray.end(); ++it) {
        cout << "Scene: Creating SceneObject " << endl;
        SceneObject *nextSceneObject = new SceneObject(*it);
        if (parent != nullptr) {
          nextSceneObject->setParent(parent);
          parent->addChild(nextSceneObject);
        } else {
          setRootSceneObject(nextSceneObject);
        }
        if (!((*it)[SCENE_OBJECT_CHILDREN]).is_null()){
          loadSceneObjects((*it)[SCENE_OBJECT_CHILDREN],nextSceneObject);
        }

        nextSceneObject->showStatus();
      }
    }
  }

  bool Scene::hasSceneObect(SceneObject *obj) {
    return mRootSceneObject == obj || mRootSceneObject->isParentOfDeep(obj);
  }

  string Scene::generateSceneObjectPath(SceneObject* so) {
    stringstream stream;
    vector<string> pathVector;

    SceneObject* next = so;
    while (next != nullptr) {
      pathVector.push_back(next->getUUID());
      next = next->getParent();
    }

    reverse(pathVector.begin(),pathVector.end());
    for (vector<string>::iterator it = pathVector.begin(); it != pathVector.end(); ++it) {
      stream << PATH_DELIMETER << *it;
    }
    string retval = stream.str();
    return retval;
  }

  SceneObject* Scene::getSceneObjectByUUID(string uuid) {
    for (vector<SceneObject*>::iterator it = mScenegraphVector.begin(); it != mScenegraphVector.end(); it++) {
      if ((*it)->hasUUID(uuid)){
        return (*it);
      }
    }
    return nullptr;
  }

  size_t Scene::getNumberOfSceneObjects() {
    return mScenegraphVector.size();
  }

  void Scene::showStatus() {
    cout << "Scene:" << endl;
    cout << "            UUID: " << mUUID << endl;
    cout << "            Name: " << mName << endl;
    cout << "Camera Transform: " << endl;
    cout << "     Translation: " << String::floatVectorToString(mDefaultCameraTransform->getTranslation()) << endl;
    cout << "        Rotation: " << String::floatVectorToString(mDefaultCameraTransform->getRotation())    << endl;
    cout << "   Scene Objects: " << getNumberOfSceneObjects() << endl;
    showScenegraph();
  }

  void Scene::showScenegraph() {
    if (mRootSceneObject == nullptr) {
      cout << "Scenegraph is empty!" << endl;
      return;
    }
    generateScenegraphVector();
    generateSceneObjectPaths();
    for(vector<SceneObject*>::iterator it = mScenegraphVector.begin(); it != mScenegraphVector.end(); it++) {
      cout << (*it)->getNameUUIDString() << " @ " << (*it)->getPath() << endl;
    }
  }

  void Scene::setRootSceneObject(SceneObject* root) {
    mRootSceneObject = root;
  }

  SceneObject* Scene::getRootSceneObject() {
    return mRootSceneObject;
  }

  void Scene::generateScenegraphVector() {
    cout << "Scene: Regenerating Scene Graph Vector" << endl;
    mScenegraphVector.clear();
    if (mRootSceneObject != nullptr) {
        mRootSceneObject->getChildrenVectorDeep(&mScenegraphVector);
    }
  }

  void Scene::generateSceneObjectPaths() {
    for(vector<SceneObject*>::iterator it = mScenegraphVector.begin(); it != mScenegraphVector.end(); it++) {
       (*it)->generatePath();
    }
  }

  vector<SceneObject*> Scene::getScenegraphVector() {
    return mScenegraphVector;
  }

  vector<float> Scene::getDefaultCameraTranslation() {
    return mDefaultCameraTransform->getTranslation();
  }

  vector<float> Scene::getDefaultCameraRotation() {
    return mDefaultCameraTransform->getRotation();
  }

  void Scene::setCameraMovementSpeed (float speed) {
    mCameraMovementSpeed = speed;
  }

  float Scene::getCameraMovementSpeed() {
    return mCameraMovementSpeed;
  }

} // End of Dream