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


#include "AudioRuntime.h"

#include "../../Scene/SceneObject/SceneObjectRuntime.h"
#include "AudioDefinition.h"
#include "AudioComponent.h"

namespace Dream
{
    AudioRuntime::AudioRuntime
    (AudioComponent* component,
     AudioDefinition* definition,
     SceneObjectRuntime* rt)
        : DiscreteAssetRuntime(definition, rt),
          mAudioComponent(component),
          mStartTime(-1),
          mLastSampleOffset(0),
          mChannels(-1)
    {
        setLogClassName("AudioInstance");
        setStatus(UNKNOWN);
        setLooping(false);
        setBuffer(0);
        setSource(0);
        generateEventList();
    }

    AudioRuntime::~AudioRuntime()
    {
       if (mAudioComponent != nullptr)
       {
          mAudioComponent->deleteBuffers(1,mBuffer);
          mAudioComponent->deleteSources(1,mSource);
       }
    }

    void
    AudioRuntime::setLooping
    (bool looping)
    {
        mLooping = looping;
        alSourcei(mSource, AL_LOOPING, mLooping ? 1 : 0);
    }

    bool
    AudioRuntime::isLooping
    ()
    const
    {
        return mLooping;
    }

    std::vector<ALchar>
    AudioRuntime::getAudioDataBuffer
    ()
    const
    {
        return mAudioDataBuffer;
    }

    ALsizei
    AudioRuntime::getFrequency
    ()
    const
    {
        return mFrequency;
    }

    ALenum
    AudioRuntime::getFormat
    ()
    const
    {
        return mFormat;
    }

    void
    AudioRuntime::setBuffer
    (ALuint buffer)
    {
        mBuffer = buffer;
    }

    ALuint
    AudioRuntime::getBuffer
    ()
    const
    {
        return mBuffer;
    }

    void
    AudioRuntime::setSource
    (ALuint source)
    {
        mSource = source;
    }

    ALuint
    AudioRuntime::getSource
    ()
    const
    {
        return mSource;
    }

    void
    AudioRuntime::play
    ()
    {
        if (mAudioComponent != nullptr)
        {
            mAudioComponent->pushToPlayQueue(this);
        }
    }

    void
    AudioRuntime::pause
    ()
    {
        if (mAudioComponent != nullptr)
        {
            mAudioComponent->pushToPauseQueue(this);
        }
    }

    void
    AudioRuntime::stop
    ()
    {
        if (mAudioComponent != nullptr)
        {
            mAudioComponent->pushToStopQueue(this);
        }
    }

    void
    AudioRuntime::setStatus
    (AudioStatus status)
    {
        mStatus = status;
    }

    AudioStatus
    AudioRuntime::getStatus
    ()
    {
        return mStatus;
    }

    void
    AudioRuntime::setSourcePosision
    (glm::vec3 pos)
    {
        alSource3f(mSource, AL_POSITION, pos.x, pos.y, pos.z);
    }

    void
    AudioRuntime::setVolume
    (float volume)
    {
        alSourcef(mSource, AL_GAIN,volume);
    }

    void
    AudioRuntime::generateEventList
    ()
    {
        auto ad = dynamic_cast<AudioDefinition*>(mDefinition);
        auto nMarkers = ad->countMarkers();

        if (nMarkers == 0)
        {
           return;
        }

        mMarkerEvents.clear();

        for (int markerIndex = 0; markerIndex< nMarkers; markerIndex++)
        {
            auto log = getLog();
            log->trace("Generating events for marker {}", markerIndex);
            auto markerStart = ad->getMarkerSampleIndex(markerIndex);
            auto count = ad->getMarkerRepeat(markerIndex);
            auto step = ad->getMarkerRepeatPeriod(markerIndex);
            auto markerName = ad->getMarkerName(markerIndex);

            auto next = markerStart;
            log->trace("Marker {}'s is : ", markerIndex, next);
            Event e(mSceneObjectRuntime,"audio");
            e.setString("name",markerName);
            e.setNumber("time",markerStart);
            mMarkerEvents.push_back(e);

            for (int i=0; i<count; i++)
            {
                auto repeatIndex = i+1;
                auto next = markerStart + (repeatIndex*step);
                log->trace("Marker {}'s {}th step is : {}", markerIndex, repeatIndex, next);
                Event e(mSceneObjectRuntime,"audio");
                e.setString("name",markerName);
                e.setNumber("time",next);
                mMarkerEvents.push_back(e);
            }
        }

        std::sort(
            mMarkerEvents.begin(),
            mMarkerEvents.end(),
            [](Event& first, Event& second)
            {
                return first.getNumber("time") < second.getNumber("time");
            }
        );

        mMarkerEventsCache = mMarkerEvents;

        if (mAudioComponent != nullptr)
        {
            mAudioComponent->pushToUpdateQueue(this);
        }
    }

    void
    AudioRuntime::updateMarkers
    ()
    {
        auto log = getLog();
        auto currentSample = mAudioComponent->getSampleOffset(mSource);
        // has just looped, restore cached events
        if (mLooping && mLastSampleOffset > currentSample)
        {
            log->debug("Just Looped");
           mMarkerEvents = mMarkerEventsCache;
        }

        for (auto it = mMarkerEvents.begin(); it != mMarkerEvents.end();)
        {
            auto time = (*it).getNumber("time");
            if (currentSample > time)
            {
                mSceneObjectRuntime->addEvent((*it));
                it++;
                mMarkerEvents.pop_front();
            }
            else
            {
                break;
            }
        }
        mLastSampleOffset = currentSample;
    }

    long long AudioRuntime::getStartTime() const
    {
        return mStartTime;
    }

    void AudioRuntime::setStartTime(long long startTime)
    {
        mStartTime = startTime;
    }

    int AudioRuntime::getChannels() const
    {
        return mChannels;
    }
} // End of Dream