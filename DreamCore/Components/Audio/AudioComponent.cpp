/*
* AudioComponent
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

#ifdef __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif

#include <iostream>

#include "AudioComponent.h"
#include "Wav/WavAudioInstance.h"
#include "Ogg/OggAudioInstance.h"

#include "AudioDefinition.h"
#include "../../Scene/SceneObject/SceneObjectRuntime.h"

namespace Dream
{
    AudioComponent::AudioComponent
    ()
        : IComponent()
    {
        setLogClassName("AudioComponent");
        auto log = getLog();
        log->trace("Constructing");
    }

    AudioComponent::~AudioComponent
    ()
    {
        auto log = getLog();
        log->trace("Destructing");
        for (ALuint source : mSources)
        {
            log->info("Stopping source {}" , source);
            alSourceStop(source);
        }
        deleteAllSources();
        deleteAllBuffers();
        if (mDevice)
        {
            alcCloseDevice(mDevice);
            mDevice = nullptr;
        }
    }

    bool
    AudioComponent::init
    ()
    {
        auto log = getLog();
        log->info("Initialising...");
        mDevice = alcOpenDevice(nullptr);
        mContext  = alcCreateContext(mDevice, nullptr);
        alcMakeContextCurrent(mContext);
        vec3 position = {0.0f,0.0f,0.0f};
        setListenerPosition(position);
        return true;
    }

    ALuint
    AudioComponent::generateBuffers
    (size_t size)
    {
        alGetError();
        ALuint buffer;
        alGenBuffers( static_cast<int>(size), &buffer );
        ALenum error = alGetError();
        if ( error != AL_NO_ERROR )
        {
            return static_cast<ALuint>(-1);
        }
        mBuffers.push_back(buffer);
        return buffer;
    }

    void
    AudioComponent::deleteAllBuffers
    ()
    {
        auto log = getLog();
        for (ALuint buffer : mBuffers)
        {
            log->info("Deleting buffer {}" , buffer);
            deleteBuffers(1, buffer);
        }
    }

    ALuint
    AudioComponent::generateSources
    (size_t size)
    {
        alGetError();
        ALuint buffer;
        alGenSources( static_cast<int>(size), &buffer );
        ALenum error = alGetError();
        if ( error != AL_NO_ERROR )
        {
            return static_cast<ALuint>(-1);
        }
        mSources.push_back(buffer);
        return buffer;
    }

    void
    AudioComponent::deleteAllSources
    ()
    {
        auto log = getLog();
        for (ALuint source : mSources)
        {
            log->info("Deleting buffer {}" , source);
            deleteSources(1, source);
        }
    }

    void
    AudioComponent::setSourcePosision
    (ALuint sourceId, vec3 pos)
    {
        alSource3f(sourceId, AL_POSITION, pos.x, pos.y, pos.z);
    }

    void
    AudioComponent::setListenerPosition
    (vec3 pos)
    {
        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    }

    void
    AudioComponent::deleteBuffers
    (int count, ALuint buffer)
    {
        alDeleteBuffers(count, &buffer);
    }

    void
    AudioComponent::deleteSources
    (int count, ALuint source)
    {
        alDeleteSources(count, &source);
    }

    void
    AudioComponent::playSource
    (ALuint source)
    {
        auto log = getLog();
        log->info(  "Playing source {}" , source);
        alSourcePlay(source);
    }

    void
    AudioComponent::stopSource
    (ALuint source)
    {
        auto log = getLog();
        log->info(  "Stopping source {}" , source);
        alSourceStop(source);
    }

    void
    AudioComponent::pauseSource
    (ALuint source)
    {
        auto log = getLog();
        log->info(  "Pausing source {}" , source);
        alSourcePause(source);
    }


    void
    AudioComponent::pushToPlayQueue
    (weak_ptr<AudioInstance> audioAssetWeak)
    {
        auto log = getLog();
        if (find(mPlayQueue.begin(),mPlayQueue.end(), audioAssetWeak) == mPlayQueue.end())
        {
            auto audioAsset = audioAssetWeak.lock();
            if (audioAsset == nullptr)
            {
                log->error("pushToPlayQueue: Invalid audio asset");
                return;
            }

            if (audioAsset->getSource() == 0 && audioAsset->getBuffer() == 0)
            {
                audioAsset->setBuffer(generateBuffers(1));
                audioAsset->setSource(generateSources(1));

                auto sort = audioAsset->getSceneObjectRuntime().lock();

                if (sort == nullptr) return;

                auto transform = sort->getTransform();

                vec3 tx = transform->getTranslation();
                vector<char>  bufferData = audioAsset->getAudioDataBuffer();
                alBufferData(audioAsset->getBuffer(), audioAsset->getFormat(), &bufferData[0],
                        static_cast<ALsizei> (bufferData.size()), audioAsset->getFrequency());
                alSourcei(
                    audioAsset->getSource(),
                    AL_BUFFER,
                    static_cast<ALint>(audioAsset->getBuffer())
                );
                setSourcePosision(audioAsset->getSource(), tx);
            }
            mPlayQueue.push_back(audioAssetWeak);
            log->info("Pushed audio asset to play queue");
        }
    }

    void
    AudioComponent::pushToPauseQueue
    (weak_ptr<AudioInstance> audioAsset)
    {
        auto log = getLog();
        if (find(mPauseQueue.begin(),mPauseQueue.end(), audioAsset) == mPauseQueue.end())
        {
            mPauseQueue.push_back(audioAsset);
            log->info("Pushed audio asset to play queue");
        }
    }

    void
    AudioComponent::pushToStopQueue
    (weak_ptr<AudioInstance> asset)
    {
        auto log = getLog();
        if (find(mStopQueue.begin(),mStopQueue.end(), asset) == mStopQueue.end())
        {
            mStopQueue.push_back(asset);
            log->info("Pushed audio asset to stop queue.");
        }
    }

    void
    AudioComponent::updateComponent
    ()
    {
        beginUpdate();
        updatePlayQueue();
        updatePauseQueue();
        updateStopQueue();
        endUpdate();
    }

    void AudioComponent::updatePlayQueue()
    {
        auto log = getLog();
        log->info("Updating Play Queue");
        for (weak_ptr<AudioInstance> aWeak : mPlayQueue)
        {
            auto audioAsset = aWeak.lock();
            if (audioAsset == nullptr)
            {
                continue;
            }
            if (getAudioStatus(audioAsset) != PLAYING)
            {
                playSource(audioAsset->getSource());
                audioAsset->setStatus(PLAYING);
            }
            else
            {
                log->info("" , audioAsset->getNameAndUuidString() , " is already playing");
            }
        }
        mPlayQueue.clear();
    }

    void
    AudioComponent::updatePauseQueue
    ()
    {
        auto log = getLog();
        log->info("Updating Pause Queue");
        for (weak_ptr<AudioInstance> aWeak : mPauseQueue)
        {
            auto audioAsset = aWeak.lock();
            if (audioAsset == nullptr)
            {
                continue;
            }
            if (getAudioStatus(audioAsset) != PAUSED)
            {
                pauseSource(audioAsset->getSource());
                audioAsset->setStatus(PAUSED);
            }
            else
            {
                log->info("{} is already paused", audioAsset->getNameAndUuidString());
            }
        }
        mPauseQueue.clear();
    }

    void
    AudioComponent::updateStopQueue
    ()
    {
        auto log = getLog();
        log->info("Updating Stop Queue");

        for (weak_ptr<AudioInstance> aWeak : mStopQueue)
        {
            auto audioAsset = aWeak.lock();
            if (audioAsset == nullptr)
            {
                continue;
            }
            if (getAudioStatus(audioAsset) != STOPPED)
            {
                stopSource(audioAsset->getSource());
                audioAsset->setStatus(STOPPED);
            }
            else
            {
                log->info("" , audioAsset->getNameAndUuidString() , " is already stopped");
            }
        }
        mStopQueue.clear();
    }

    void
    AudioComponent::playAudioAsset
    (weak_ptr<AudioInstance> asset)
    {
        pushToPlayQueue(asset);
    }

    void
    AudioComponent::pauseAudioAsset
    (weak_ptr<AudioInstance> asset)
    {
        pushToPauseQueue(asset);
    }

    void
    AudioComponent::stopAudioAsset
    (weak_ptr<AudioInstance> asset)
    {
        pushToStopQueue(asset);
    }

    float
    AudioComponent::getSampleOffset
    (ALuint source)
    {
        float sampleOffset;
        alGetSourcef(source, AL_SAMPLE_OFFSET, &sampleOffset );
        return sampleOffset;
    }

    vector<char>
    AudioComponent::getAudioBuffer
    (weak_ptr<AudioInstance> aWeak, int offset, int length)
    {
        auto audioAsset = aWeak.lock();
        vector<char> retval = vector<char>(length);
        if (audioAsset == nullptr)
        {
            return retval;
        }

        vector<char> audioData = audioAsset->getAudioDataBuffer();
        char* dataBegin = &audioData[0];
        retval.insert(retval.begin(), dataBegin, dataBegin+length);

        return retval;
    }

    float
    AudioComponent::getSampleOffset
    (weak_ptr<AudioInstance> aWeak )
    {
        auto audioAsset = aWeak.lock();
        if (audioAsset == nullptr)
        {
            return 0.0f;
        }
        return getSampleOffset(audioAsset->getSource());
    }

    AudioStatus
    AudioComponent::getAudioStatus
    (weak_ptr<AudioInstance> aWeak)
    {
        auto log = getLog();
        auto audioAsset = aWeak.lock();
        if (audioAsset == nullptr)
        {
            return UNKNOWN;
        }

        int state;
        alGetSourcei(audioAsset->getSource(), AL_SOURCE_STATE, &state);
        switch (state)
        {
            case AL_STOPPED:
                return STOPPED;
            case AL_PLAYING:
                return PLAYING;
            case AL_PAUSED:
                return PAUSED;
            default:
                log->error("Unknown Audio State for {} " , audioAsset->getNameAndUuidString());
                return UNKNOWN;
        }
    }

    shared_ptr<AudioInstance>
    AudioComponent::newAudioInstance
    (weak_ptr<AudioDefinition> defWeak, weak_ptr<SceneObjectRuntime> rtWeak)
    {
        auto log = getLog();
        auto definition = defWeak.lock();
        auto rt = rtWeak.lock();

        if (definition == nullptr || rt == nullptr)
        {
           return nullptr;
        }

        if (definition->getFormat().compare(Constants::ASSET_FORMAT_AUDIO_WAV) == 0)
        {
            return make_shared<WavAudioInstance>(
                dynamic_pointer_cast<AudioComponent>(shared_from_this()),definition,rt
            );
        }
        else if (definition->getFormat().compare(Constants::ASSET_FORMAT_AUDIO_OGG) == 0)
        {
            return make_shared<OggAudioInstance>(
                dynamic_pointer_cast<AudioComponent>(shared_from_this()), definition,rt
            );
        }

        log->error("Error, unrecognised audio format {}", definition->getFormat());

        return nullptr;

    }

    void
    AudioComponent::setVolume
    (float volume)
    {
       alListenerf(AL_GAIN,volume);
    }

} // End of Dream
