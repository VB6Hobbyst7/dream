/*
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

#include <vector>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include "BoundingBox.h"
#include "../../Components/Script/ScriptTasks.h"
#include "../../Components/Graphics/Frustum.h"
#include "../../Common/Runtime.h"
#include "../../Components/Transform.h"

using std::vector;
using std::function;
using std::shared_ptr;
using nlohmann::json;

namespace Dream
{
    class Actor;
    class Scene;
    class Event;
    class AnimationRuntime;
    class AudioRuntime;
    class PathRuntime;
    class ModelRuntime;
    class LightRuntime;
    class PhysicsObjectRuntime;
    class ParticleEmitterRuntime;
    class SceneRuntime;
    class ActorDefinition;
    class AssetDefinition;
    class PathDefinition;
    class ScrollerRuntime;
    class ScrollerDefinition;
    class AnimationDefinition;
    class AudioDefinition;
    class LightDefinition;
    class ModelDefinition;
    class PhysicsObjectDefinition;
    class ScriptDefinition;
    class ParticleEmitterDefinition;
    class ScriptRuntime;
    class AssetRuntime;
    class ObjectEmitterDefinition;
    class ObjectEmitterRuntime;

    class ActorRuntime : public Runtime
    {
        AnimationRuntime* mAnimationRuntime;
        AudioRuntime* mAudioRuntime;
        LightRuntime* mLightRuntime;
        ParticleEmitterRuntime* mParticleEmitterRuntime;
        PathRuntime* mPathRuntime;
        PhysicsObjectRuntime* mPhysicsObjectRuntime;
        ScriptRuntime* mScriptRuntime;
        ModelRuntime* mModelRuntime;
        ScrollerRuntime* mScrollerRuntime;
        ObjectEmitterRuntime* mObjectEmitterRuntime;

        Transform mInitialTransform;
        Transform mTransform;
        mutex mEventQueueMutex;
        vector<Event> mEventQueue;
        map<AssetType,uint32_t> mAssetDefinitions;
        vector<ActorRuntime*> mChildRuntimes;
        SceneRuntime* mSceneRuntime;
        ActorRuntime* mParentRuntime;
        BoundingBox mBoundingBox;
        bool mHasCameraFocus;
        bool mDeleted;
        bool mHidden;
        bool mAlwaysDraw;
        bool mRandomUuid;
        long mDeferredFor;
        long mObjectLifetime;
        long mDieAfter;
        ScriptOnInitTask mScriptOnInitTask;
        ScriptOnUpdateTask mScriptOnUpdateTask;
        ScriptOnEventTask mScriptOnEventTask;
        shared_ptr<ScriptOnDestroyTask> mScriptOnDestroyTask;
        map<string,string> mAttributes;

    public:
        ActorRuntime(ActorDefinition* sd, SceneRuntime* sceneRuntime = nullptr, bool randomUuid = false);
        ~ActorRuntime() override;

        void collectGarbage() override;

        SceneRuntime* getSceneRuntime();
        ActorDefinition* getActorDefinition();

        bool createAssetRuntimes();
        bool createAnimationRuntime(AnimationDefinition*);
        bool createPathRuntime(PathDefinition*);
        bool createAudioRuntime(AudioDefinition*);
        bool createModelRuntime(ModelDefinition*);
        bool createScriptRuntime(ScriptDefinition*);
        bool createPhysicsObjectRuntime(PhysicsObjectDefinition*);
        bool createParticleEmitterRuntime(ParticleEmitterDefinition*);
        bool createLightRuntime(LightDefinition*);
        bool createScrollerRuntime(ScrollerDefinition*);
        bool createObjectEmitterRuntime(ObjectEmitterDefinition*);

        AnimationRuntime* getAnimationRuntime();
        PathRuntime*  getPathRuntime();
        AudioRuntime* getAudioRuntime();
        ModelRuntime* getModelRuntime();
        ScriptRuntime* getScriptRuntime();
        PhysicsObjectRuntime* getPhysicsObjectRuntime();
        LightRuntime* getLightRuntime();
        ParticleEmitterRuntime* getParticleEmitterRuntime();
        ScrollerRuntime* getScrollerRuntime();
        ObjectEmitterRuntime* getObjectEmitterRuntime();
        AssetRuntime* getAssetRuntime(AssetType);

        bool hasAnimationRuntime();
        bool hasPathRuntime();
        bool hasAudioRuntime();
        bool hasModelRuntime();
        bool hasScriptRuntime();
        bool hasPhysicsObjectRuntime();
        bool hasLightRuntime();
        bool hasScrollerRuntime();
        bool hasObjectEmitterRuntime();

        Transform& getTransform();
        Transform getInitialTransform();
        void setTransform(Transform* transform);
        void translateWithChildren(const Vector3& translation);
        void preTranslateWithChildren(const Vector3& translation);
        void transformOffsetInitial(const mat4& matrix);
        void translateOffsetInitial(const Vector3& tx);

        bool getHasCameraFocus() const;
        void setHasCameraFocus(bool);

        bool isPlayerObject() const;
        bool hasEvents();
        void addEvent(const Event& event);
        vector<Event>* getEventQueue();
        bool tryLockEventQueue();
        void unlockEventQueue();
        void clearEventQueue();

        ActorRuntime* getChildRuntimeByUuid(uint32_t uuid);
        ActorRuntime* addChildFromTemplateUuid(uint32_t uuid);
        int countAllChildren();
        size_t countChildren();
        void addChildRuntime(ActorRuntime*);
        void removeChildRuntime(ActorRuntime*);
        ActorRuntime* createAndAddChildRuntime(ActorDefinition*);
        vector<ActorRuntime*> getChildRuntimes();
        bool isChildOf(ActorRuntime*);

        bool isParentOf(ActorRuntime* child);
        void setParentRuntime(ActorRuntime* parent);
        ActorRuntime* getParentRuntime();

        bool useDefinition() override;
        bool loadDeferred();

        bool getDeleted() const;
        void setDeleted(bool deleted);

        bool getHidden() const;
        void setHidden(bool hidden);

        void removeAnimationRuntime();
        void removeAudioRuntime();
        void removePathRuntime();
        void removeModelRuntime();
        void removeLightRuntime();
        void removeScriptRuntime();
        void removePhysicsObjectRuntime();
        void removeParticleEmitterRuntime();
        void removeScrollerRuntime();
        void removeObjectEmitterRuntime();

        bool replaceAssetUuid(AssetType type, uint32_t uuid);
        AssetDefinition* getAssetDefinitionByUuid(uint32_t uuid);
        void setAssetDefinitionsMap(const map<AssetType, uint32_t> &loadQueue);
        map<AssetType, uint32_t> getAssetDefinitionsMap();
        bool getAlwaysDraw() const;
        void setAlwaysDraw(bool alwaysDraw);

        BoundingBox& getBoundingBox();
        void setBoundingBox(const BoundingBox& boundingBox);

        float distanceFrom(ActorRuntime* other);
        float distanceFrom(const Vector3& other);
        bool visibleInFrustum();
        bool containedInFrustum();
        bool containedInFrustumAfterTransform(const mat4& tx);
        bool exceedsFrustumPlaneAtTranslation(Frustum::Plane plane, const Vector3& tx);

        bool applyToAll(const function<bool(ActorRuntime*)>& fn);
        ActorRuntime* applyToAll(const function<ActorRuntime*(ActorRuntime*)>& fn);
        void translateOffsetInitialWithChildren(const Vector3& translation);
        void initTransform();

        long getDeferredFor() const;
        void setDeferredFor(long deferred);

        long getObjectLifetime() const;
        void setObjectLifetime(long l);

        long getDieAfter() const;
        void setDieAfter(long);
        void updateLifetime();

        bool loadChildrenFromDefinition(ActorDefinition* definition);

        ScriptOnInitTask* getScriptOnInitTask();
        ScriptOnEventTask* getScriptOnEventTask();
        ScriptOnUpdateTask* getScriptOnUpdateTask();

        string getAttribute(const string& key) const;
        void setAttribute(const string& key, const string& value);
        const map<string,string>& getAttributesMap() const;
    };
}
