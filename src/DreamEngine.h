#ifndef DREAMENGINE_H
#define DREAMENGINE_H

#include <iostream>
#include <thread>
#include <SDL2/SDL.h>

#include "Constants.h"
#include "ArgumentParser.h"
#include "Time.h"
#include "FileReader.h"
#include "Scene.h"
#include "Project.h"
#include "AssetInstance.h"
#include "AssetManager.h"
#include "Components/Graphics/Camera.h"
#include "Components/Animation/AnimationComponent.h"
#include "Components/Audio/AudioComponent.h"
#include "Components/Physics/PhysicsComponent.h"
#include "Components/Graphics/GraphicsComponent.h"

namespace Dream {
  class DreamEngine {
  private:
    static DreamEngine* _Instance;
    Time *mTime;
    Project *mProject;
    Scene *mActiveScene;
    AssetManager *mAssetManager;
    Camera *mCamera;
    AudioComponent *mAudioComponent;
    GraphicsComponent *mGraphicsComponent;
    PhysicsComponent *mPhysicsComponent;
    AnimationComponent *mAnimationComponent;
    bool mDone;
    SDL_Event mEvent;
    DreamEngine(void);
  public:
    static DreamEngine* getInstance();
    ~DreamEngine(void);
    bool loadFromArgumentParser(ArgumentParser*);
    bool initEngine();
    bool initSDL();
    bool initAssetManager();
    bool initComponents();

    bool loadProjectFromFileReader(string projectPath, FileReader*);
    bool loadScene(Scene*);
    bool loadSceneByUuid(string);
    Project* getProject();
    void setProject(Project*);
    bool isProjectLoaded();

    AssetManager* getAssetManager();

    bool update();
    void setTime(Time*);
    Time* getTime();
    void setDone(bool);
    bool isDone();

    Scene *getActiveScene();

    void setAssetManager(AssetManager*);
    Camera* getCamera();
    void setCamera(Camera*);

    void updateComponents();
    void destroyComponents();
    bool initAnimationComponent();
    bool initAudioComponent();
    bool initPhysicsComponent();
    bool initGraphicsComponent();

    void destroyScene(Scene*);

    AnimationComponent* getAnimationComponent();
    void setAnimationComponent(AnimationComponent*);

    AudioComponent* getAudioComponent();
    void setAudioComponent(AudioComponent*);

    PhysicsComponent* getPhysicsComponent();
    void setPhysicsComponent(PhysicsComponent*);

    GraphicsComponent* getGraphicsComponent();
    void setGraphicsComponent(GraphicsComponent*);

    map<SceneObject*,LuaScriptInstance*>* getLuaScriptMap();
    SDL_Event getSDL_Event();


  }; // End of Dream
} // End of Dream

#endif // DREAMENGINE_H
