/*
 * ProjectRuntime.h
 *
 * Created: 05 2017 by Ashley
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

#include <string>
#include <vector>

#include "../Common/Runtime.h"

using std::string;
using std::vector;

namespace Dream
{
    // Forward Declarations

    class Project;
    class PathComponent;
    class AnimationComponent;
    class AudioComponent;
    class InputComponent;
    class GraphicsComponent;
    class IWindowComponent;
    class PhysicsComponent;
    class ScriptComponent;
    class NanoVGComponent;
    class Transform;
    class SceneRuntime;
    class SceneDefinition;
    class MaterialCache;
    class TextureCache;
    class ScriptCache;
    class ModelCache;
    class ShaderCache;
    class ComponentThread;
    class AssetDefinition;
    class SceneObjectRuntime;

    class Time;

    // Class Declaration
    class ProjectRuntime : public Runtime
    {

    private: // Member Variables
        bool mDone;

        Time* mTime;
        Project* mProject;

        // Components
        AnimationComponent* mAnimationComponent;
        AudioComponent* mAudioComponent;
        InputComponent* mInputComponent;
        GraphicsComponent* mGraphicsComponent;
        NanoVGComponent* mNanoVGComponent;
        PhysicsComponent* mPhysicsComponent;
        PathComponent* mPathComponent;
        ScriptComponent* mScriptComponent;
        IWindowComponent* mWindowComponent;

        // Caches
        TextureCache* mTextureCache;
        MaterialCache* mMaterialCache;
        ModelCache* mModelCache;
        ShaderCache* mShaderCache;
        ScriptCache* mScriptCache;
        bool mScriptingEnabled;

        vector<SceneRuntime*> mSceneRuntimeVector;

    public: // Public Functions
        ProjectRuntime(
            Project* parentProject,
            IWindowComponent* wc = nullptr);
        ~ProjectRuntime() override;

        void setDone(bool);
        bool isDone();

        Time* getTime();

        AnimationComponent* getAnimationComponent();
        PathComponent* getPathComponent();
        AudioComponent* getAudioComponent();
        PhysicsComponent* getPhysicsComponent();
        GraphicsComponent* getGraphicsComponent();
        NanoVGComponent* getNanoVGComponent();
        IWindowComponent* getWindowComponent();
        ScriptComponent* getScriptComponent();
        Project* getProject();
        InputComponent* getInputComponent();

        bool initComponents();

        void collectGarbage(SceneRuntime* rt);
        void collectGarbage() override;

        void updateAll();
        bool updateLogic(SceneRuntime* rt);
        void updateGraphics(SceneRuntime* rt);

        int getWindowWidth();
        void setWindowWidth(int);

        int getWindowHeight();
        void setWindowHeight(int);

        bool constructSceneRuntime(SceneRuntime* rt);
        void destructSceneRuntime(SceneRuntime* rt, bool clearCaches = false);
        void clearAllCaches();

        bool useDefinition() override;

        ShaderCache* getShaderCache();
        MaterialCache* getMaterialCache();
        ModelCache* getModelCache();
        TextureCache* getTextureCache();
        ScriptCache* getScriptCache();

        bool getScriptingEnabled() const;
        void setScriptingEnabled(bool);
        bool hasActiveScene();

        AssetDefinition* getAssetDefinitionByUuid(string uuid);

        SceneObjectRuntime* getSceneObjectRuntimeByUuid(SceneRuntime* rt, string uuid);
        SceneRuntime* getActiveSceneRuntime();
        SceneRuntime* getSceneRuntimeByUuid(string uuid);

        void addSceneRuntime(SceneRuntime*);
        void removeSceneRuntime(SceneRuntime*);
        void setSceneRuntimeActive(string uuid);
        vector<SceneRuntime*> getSceneRuntimeVector();
        bool hasSceneRuntime(string uuid);
        bool hasLoadedScenes();

    private: // Member Functions
        bool initPathComponent();
        bool initAnimationComponent();
        bool initAudioComponent();
        bool initInputComponent();
        bool initPhysicsComponent();
        bool initGraphicsComponent();
        bool initNanoVGComponent();
        bool initWindowComponent();
        bool initScriptComponent();
        bool initCaches();

        void deleteCaches();
        void deleteComponents();
    };

} // End Dream
