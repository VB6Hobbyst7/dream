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

#include "OggAudioRuntime.h"

#include <vorbis/vorbisfile.h>

#include "../AudioDefinition.h"
#include "../../../Scene/Actor/ActorRuntime.h"

#define AUDIO_BUFFER_SIZE 1024

namespace Dream
{
    OggAudioRuntime::OggAudioRuntime
    (AudioDefinition* def, ProjectRuntime* project)
        : AudioRuntime(def,project)
    {
        #ifdef DREAM_LOG
        setLogClassName("OggAudioRuntime");
        #endif
    }

    bool
    OggAudioRuntime::useDefinition
    ()
    {
        auto absPath = getAssetFilePath();
        #ifdef DREAM_LOG
        getLog()->debug("Loading Runtime: {}", absPath);
        #endif

        // 0 for Little-Endian, 1 for Big-Endian
        int endian = 0;
        int bitStream;
        long bytes;

        // Local fixed size array
        char buffer[AUDIO_BUFFER_SIZE];
        FILE *file = fopen(absPath.c_str(), "rb");
        if (file == nullptr)
        {
            #ifdef DREAM_LOG
            getLog()->error("Cannot open {} for reading", absPath);
            #endif
            return false;
        }

        // Try opening the given file
        OggVorbis_File oggFile;
        if (ov_open(file, &oggFile, nullptr, 0) != 0)
        {
            #ifdef DREAM_LOG
            getLog()->error("Error opening {} for decoding");
            #endif
            return false;
        }

        // Get some information about the OGG file
        vorbis_info *oggInfo;
        oggInfo = ov_info(&oggFile, -1);

        // Check the number of channels... always use 16-bit samples
        if (oggInfo->channels == 1)
        {
            mChannels = 1;
            mFormat = AL_FORMAT_MONO16;
        }
        else
        {
            mChannels = 2;
            mFormat = AL_FORMAT_STEREO16;
        }

        setLooping(static_cast<AudioDefinition*>(mDefinition)->getLoop());

        // The frequency of the sampling rate
        mFrequency = oggInfo->rate;

        // Keep reading until all is read
        do
        {
            // Read up to a buffer's worth of decoded sound data
            bytes = ov_read(&oggFile, &buffer[0], AUDIO_BUFFER_SIZE, endian, 2, 1, &bitStream);

            if (bytes < 0)
            {
                ov_clear(&oggFile);
                #ifdef DREAM_LOG
                getLog()->error("Error decoding {}", absPath);
                #endif
                return false;
            }
            // Append to end of buffer
            mAudioDataBuffer.insert(mAudioDataBuffer.end(), &buffer[0], &buffer[0] + bytes);
        }
        while (bytes > 0);

        // Clean up!
        ov_clear(&oggFile);

        return loadIntoAL();
    }

} // End of Dream


