/*
 * WavAudioInstance
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

#include "WavAudioRuntime.h"
#include "../AudioDefinition.h"

namespace Dream
{
    WavAudioRuntime::WavAudioRuntime
    (
        AudioComponent* comp,
        AudioDefinition* definition,
        SceneObjectRuntime* transform
    ) : AudioRuntime(comp, definition, transform)

    {
        setLogClassName("WavAudioInstance") ;
        return;
    }

    WavAudioRuntime::~WavAudioRuntime
    ()
    {
        return;
    }

    bool
    WavAudioRuntime::useDefinition
    ()
    {
        auto log = getLog();
        string absPath = getAssetFilePath();
        log->debug("Loading wav file from {}", absPath);
        int headerSize = sizeof(mWavHeader), filelength = 0;
        FILE* wavFile = fopen(absPath.c_str(), "r");

        if (wavFile == nullptr)
        {
            log->error("Unable to open wave file: {}", absPath);
            return false;
        }

        //Read the header
        size_t bytesRead = fread(&mWavHeader, 1, headerSize, wavFile);
        log->debug("Header Read {} bytes" ,bytesRead);
        if (bytesRead > 0)
        {
            //Read the data
            //uint16_t bytesPerSample = mWavHeader.BitsPerSample / 8;      // Number of bytes per sample
            //uint64_t numSamples = mWavHeader.ChunkSize / bytesPerSample; //How many samples are in the wav file?
            int8_t* buffer = new int8_t[AUDIO_BUFFER_SIZE];
            while ((bytesRead = fread(buffer, sizeof buffer[0], AUDIO_BUFFER_SIZE / (sizeof buffer[0]), wavFile)) > 0)
            {
                mAudioDataBuffer.insert(mAudioDataBuffer.end(), buffer, buffer + bytesRead);
            }
            mFrequency = mWavHeader.SamplesPerSecond;
            if (mWavHeader.NumOfChannels == 1)
            {
                mChannels = 1;
                mFormat = AL_FORMAT_MONO16;
            }
            else
            {
                mChannels = 2;
                mFormat = AL_FORMAT_STEREO16;
            }

            setLooping(dynamic_cast<AudioDefinition*>(mDefinition)->getLoop());

            log->debug("Read {} bytes", mAudioDataBuffer.size());
            delete [] buffer;
            buffer = NULL;
            filelength = getFileSize(wavFile);

            log->debug(
                "Status...\n"
                "\tFile size is: {} bytes\n"
                "\tRIFF header: {} {} {} {}\n"
                "\tWAVE header: {} {} {} {}\n"
                "\tFMT: {} {} {} {}\n"
                "\tData size: {}\n"
                "\tSampling Rate: {}\n"
                "\tBits used: {}\n"
                "\tChannels: {}\n"
                "\tBytes per second: {}\n"
                "\tData length: {}\n"
                "\tAudio Format: {}\n"
                "\tBlock align: {}\n"
                "\tData string: {} {} {} {}\n",
                  filelength,
                  mWavHeader.RIFF[0], mWavHeader.RIFF[1], mWavHeader.RIFF[2], mWavHeader.RIFF[3],
                  mWavHeader.WAVE[0], mWavHeader.WAVE[1], mWavHeader.WAVE[2], mWavHeader.WAVE[3],
                  mWavHeader.fmt[0], mWavHeader.fmt[1], mWavHeader.fmt[2], mWavHeader.fmt[3] ,
                  mWavHeader.ChunkSize,
                  mWavHeader.SamplesPerSecond,
                  mWavHeader.BitsPerSample,
                  mWavHeader.NumOfChannels,
                  mWavHeader.BytesPerSecond,
                  mWavHeader.Subchunk2Size,
                  mWavHeader.AudioFormat,
                  mWavHeader.BlockAlign,
                  mWavHeader.Subchunk2ID[0], mWavHeader.Subchunk2ID[1], mWavHeader.Subchunk2ID[2], mWavHeader.Subchunk2ID[3]
            );
        }
        fclose(wavFile);
        return true;
    }

    // find the file size
    long
    WavAudioRuntime::getFileSize
    (FILE* inFile)
    {
        size_t fileSize = 0;
        fseek(inFile, 0, SEEK_END);
        fileSize = ftell(inFile);
        fseek(inFile, 0, SEEK_SET);
        return fileSize;
    }

}