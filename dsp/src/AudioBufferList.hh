/*
Copyright (c) 2018-present, Facebook, Inc.

This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once

#include "../src/DSP.hh"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>

namespace TBE
{
   class AudioBufferList
   {
    public:
      using UPtr = std::unique_ptr<AudioBufferList>;

      AudioBufferList(int32_t numSamplesPerChannel, int32_t numChannels)
          : numSamplesPerChannel_(numSamplesPerChannel), numChannels_(numChannels), ownsBuffer_(true), buffer_(nullptr)
      {
         // Can't have 0 channels!
         assert(numChannels_ > 0);
         // Can't have 0 samples!
         assert(numSamplesPerChannel_ > 0);

         buffer_ = new float *[numChannels_];

         for (int i = 0; i < numChannels_; ++i)
         {
            buffer_[i] = new float[numSamplesPerChannel_];
            memset(buffer_[i], 0, numSamplesPerChannel_ * sizeof(float));
         }
      }

      explicit AudioBufferList(float **otherBuffers, int32_t numSamplesPerChannel, int32_t numChannels,
                               bool mustOwnOtherBuffer)
          : numSamplesPerChannel_(numSamplesPerChannel),
            numChannels_(numChannels),
            ownsBuffer_(mustOwnOtherBuffer),
            buffer_(otherBuffers)
      {
      }

      ~AudioBufferList()
      {
         if (ownsBuffer_)
         {
            for (int i = 0; i < numChannels_; ++i)
            {
               delete[] buffer_[i];
            }
            delete[] buffer_;
            buffer_ = nullptr;
         }
      }

      inline float *getChannelDataToWrite(int32_t channelNum)
      {
         // Channel out of bounds!
         assert(channelNum < numChannels_);

         return buffer_[channelNum];
      }

      inline const float *getChannelDataToRead(int32_t channelNum) const
      {
         // Channel out of bounds!
         assert(channelNum < numChannels_);

         return const_cast<const float *>(buffer_[channelNum]);
      }

      inline void zero()
      {
         for (int i = 0; i < numChannels_; ++i)
         {
            memset(buffer_[i], 0, numSamplesPerChannel_ * sizeof(float));
         }
      }

      float **getData()
      {
         return buffer_;
      }

      const float **getDataReadOnly() const
      {
         return const_cast<const float **>(buffer_);
      }

      inline int32_t getNumOfChannels() const
      {
         return numChannels_;
      }

      inline int32_t getSamplesPerChannel() const
      {
         return numSamplesPerChannel_;
      }

      void sum(const AudioBufferList &other)
      {
         assert(&other != this);
         assert(getNumOfChannels() == other.getNumOfChannels());
         assert(getSamplesPerChannel() == other.getSamplesPerChannel());

         for (int i = 0; i < getNumOfChannels(); i++)
         {
            dsp_.add(getChannelDataToRead(i), other.getChannelDataToRead(i), getChannelDataToWrite(i),
                     getSamplesPerChannel());
         }
      }

      void sum(const AudioBufferList &other, int32_t numOfSamplesToSum)
      {
         assert(&other != this);
         assert(numOfSamplesToSum <= numSamplesPerChannel_);

         for (int i = 0; i < getNumOfChannels(); i++)
         {
            dsp_.add(getChannelDataToRead(i), other.getChannelDataToRead(i), getChannelDataToWrite(i),
                     numOfSamplesToSum);
         }
      }

      void sum(const AudioBufferList &other, int32_t otherChannelOffset, int32_t numChannels, int32_t numOfSamplesToSum)
      {
         assert(&other != this);
         assert(numOfSamplesToSum <= numSamplesPerChannel_);
         assert((otherChannelOffset + numChannels) <= other.getNumOfChannels());
         assert(numChannels <= getNumOfChannels());

         for (int i = 0; i < numChannels; i++)
         {
            dsp_.add(getChannelDataToRead(i), other.getChannelDataToRead(i + otherChannelOffset),
                     getChannelDataToWrite(i), numOfSamplesToSum);
         }
      }

      void sum(const AudioBufferList &other, int32_t thisChannelOffset, int32_t otherChannelOffset, int32_t numChannels,
               int32_t numOfSamplesToSum)
      {
         assert(&other != this);
         assert(numOfSamplesToSum <= numSamplesPerChannel_);
         assert((otherChannelOffset + numChannels) <= other.getNumOfChannels());
         assert(numChannels <= getNumOfChannels());

         for (int i = 0; i < numChannels; i++)
         {
            dsp_.add(getChannelDataToRead(i + thisChannelOffset), other.getChannelDataToRead(i + otherChannelOffset),
                     getChannelDataToWrite(i + thisChannelOffset), numOfSamplesToSum);
         }
      }

      void scale(float scalar)
      {
         for (int32_t c = 0; c < getNumOfChannels(); ++c)
         {
            dsp_.multiplyScalar(getChannelDataToWrite(c), scalar, getChannelDataToWrite(c), getSamplesPerChannel());
         }
      }

      float getPeak(int32_t channel) const
      {
         return getPeak(channel, numSamplesPerChannel_);
      }

      float getPeak(int32_t channel, int32_t numSamples) const
      {
         assert(channel < numChannels_);
         assert(numSamples <= numSamplesPerChannel_);

         float peak = 0.f;
         for (int i = 0; i < numSamples; ++i)
         {
            peak = std::max(peak, std::abs(getChannelDataToRead(channel)[i]));
         }
         return peak;
      }

      float getRMS(int32_t channel) const
      {
         return getRMS(channel, numSamplesPerChannel_);
      }

      float getRMS(int32_t channel, int32_t numSamples) const
      {
         assert(channel < numChannels_);
         assert(numSamples <= numSamplesPerChannel_);
         float meanSquares{0};
         auto data = getChannelDataToRead(channel);
         for (int i = 0; i < numSamples; i++)
         {
            meanSquares += (data[i] * data[i]) / numSamples;
         }
         return sqrtf(meanSquares);
      }

      bool channelIsSilent(int32_t channelIndex, int32_t numSamplesToSearch) const
      {
         assert(numSamplesToSearch <= numSamplesPerChannel_);
         assert(channelIndex < numChannels_ && channelIndex >= 0);
         bool result = true;
         for (int i = 0; i < numSamplesToSearch; ++i)
         {
            if (getChannelDataToRead(channelIndex)[i] != 0.f)
            {
               result = false;
               break;
            }
         }
         return result;
      }

      bool channelsAreSilent(int32_t numSamplesToSearch) const
      {
         assert(numSamplesToSearch <= numSamplesPerChannel_);
         bool result = true;
         for (int ch = 0; ch < getNumOfChannels(); ++ch)
         {
            result = channelIsSilent(ch, numSamplesToSearch);
            if (!result)
            {
               break;
            }
         }
         return result;
      }

      bool channelIsSilent(int32_t channelIndex) const
      {
         return channelIsSilent(channelIndex, numSamplesPerChannel_);
      }

      bool channelsAreSilent() const
      {
         return channelsAreSilent(numSamplesPerChannel_);
      }

      AudioBufferList(const AudioBufferList &) = delete;
      void operator=(const AudioBufferList &) = delete;

    private:
      int numSamplesPerChannel_;
      int numChannels_;

      bool ownsBuffer_;

      float **buffer_;

      FBDSP dsp_;
   };
} // namespace TBE
