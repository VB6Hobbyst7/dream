#include "AudioInstance.h"

#include "../../Scene/SceneObject/SceneObjectRuntime.h"
#include "AudioDefinition.h"
#include "AudioComponent.h"

namespace Dream
{
    AudioInstance::AudioInstance
    (shared_ptr<AudioComponent> component,
     shared_ptr<AudioDefinition> definition,
     shared_ptr<SceneObjectRuntime> transform)
        : IAssetInstance(definition, transform),
          mAudioComponent(component)
    {
        setStatus(UNKNOWN);
        setLooping(false);
        setBuffer(0);
        setSource(0);
    }

    void
    AudioInstance::setLooping
    (bool looping)
    {
        mLooping = looping;
    }

    bool
    AudioInstance::isLooping
    ()
    {
        return mLooping;
    }

    std::vector<ALchar>
    AudioInstance::getAudioDataBuffer
    ()
    {
        return mAudioDataBuffer;
    }

    ALsizei
    AudioInstance::getFrequency
    ()
    {
        return mFrequency;
    }

    ALenum
    AudioInstance::getFormat
    ()
    {
        return mFormat;
    }

    void
    AudioInstance::setBuffer
    (ALuint buffer)
    {
        mBuffer = buffer;
    }

    ALuint
    AudioInstance::getBuffer
    ()
    {
        return mBuffer;
    }

    void
    AudioInstance::setSource
    (ALuint source)
    {
        mSource = source;
    }

    ALuint
    AudioInstance::getSource
    ()
    {
        return mSource;
    }

    void AudioInstance::play()
    {
       mAudioComponent->playAudioAsset(static_pointer_cast<AudioInstance>(shared_from_this()));
    }

    void AudioInstance::pause()
    {
       mAudioComponent->pauseAudioAsset(static_pointer_cast<AudioInstance>(shared_from_this()));
    }

    void AudioInstance::stop()
    {
       mAudioComponent->stopAudioAsset(static_pointer_cast<AudioInstance>(shared_from_this()));
    }

    void
    AudioInstance::setStatus
    (AudioStatus status)
    {
        mStatus = status;
    }

    AudioStatus
    AudioInstance::getStatus
    ()
    {
        return mStatus;
    }
} // End of Dream
