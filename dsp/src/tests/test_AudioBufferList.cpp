/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <cmath>
#include "../AudioBufferList.hh"
#include "gtest/gtest.h"

/////////////////////////////////////////////////
///// Vector Mult
/////////////////////////////////////////////////
namespace TBE {
namespace SIMD {
class AudioBufferListTest : public ::testing::Test {
 protected:
  void SetUp() override {
    numChannels_ = 2;
    numSamples_ = 512;
    TestAudioBufferList_.reset(new AudioBufferList(numSamples_, numChannels_));
  }

  void TearDown() override {}

  int numChannels_;
  int numSamples_;
  AudioBufferList::UPtr TestAudioBufferList_;
};

TEST_F(AudioBufferListTest, initTest) {
  ASSERT_EQ(TestAudioBufferList_->getNumOfChannels(), numChannels_);
  ASSERT_EQ(TestAudioBufferList_->getSamplesPerChannel(), numSamples_);

  for (int i = 0; i < numChannels_; ++i) {
    for (int j = 0; j < numSamples_; ++j) {
      ASSERT_EQ(TestAudioBufferList_->getData()[i][j], 0.f);
    }
  }
}

TEST_F(AudioBufferListTest, readWriteTest) {
  const float expected_sample_1 = 0.5;
  const float expected_sample_2 = 0.2;

  for (int i = 0; i < numSamples_; ++i) {
    TestAudioBufferList_->getChannelDataToWrite(0)[i] = expected_sample_1;
    TestAudioBufferList_->getChannelDataToWrite(1)[i] = expected_sample_2;
  }

  for (int i = 0; i < numSamples_; ++i) {
    ASSERT_EQ(TestAudioBufferList_->getChannelDataToRead(0)[i], expected_sample_1);
    ASSERT_EQ(TestAudioBufferList_->getChannelDataToRead(1)[i], expected_sample_2);
  }
}

TEST_F(AudioBufferListTest, zeroTest) {
  for (int i = 0; i < numChannels_; ++i) {
    for (int j = 0; j < numSamples_; ++j) {
      TestAudioBufferList_->getData()[i][j] = j;
    }
  }

  TestAudioBufferList_->zero();

  for (int i = 0; i < numChannels_; ++i) {
    for (int j = 0; j < numSamples_; ++j) {
      ASSERT_EQ(TestAudioBufferList_->getData()[i][j], 0.f);
    }
  }
}

TEST_F(AudioBufferListTest, getNumOfChannelsTest) {
  ASSERT_EQ(TestAudioBufferList_->getNumOfChannels(), numChannels_);
}

TEST_F(AudioBufferListTest, getBufferLengthTest) {
  ASSERT_EQ(TestAudioBufferList_->getSamplesPerChannel(), numSamples_);
}

TEST_F(AudioBufferListTest, Sum) {
  const int32_t channels = 4;
  const int32_t samples = 7;

  auto fill = [&](AudioBufferList& b1, AudioBufferList& b2) {
    for (int ch = 0; ch < b1.getNumOfChannels(); ch++) {
      for (int i = 0; i < samples; i++) {
        b1.getChannelDataToWrite(ch)[i] = i;
        b2.getChannelDataToWrite(ch)[i] = i;
      }
    }
  };

  {
    AudioBufferList a(samples, channels);
    AudioBufferList b(samples, channels);
    fill(a, b);
    b.sum(a);

    for (int ch = 0; ch < a.getNumOfChannels(); ch++) {
      for (int i = 0; i < samples; i++) {
        EXPECT_EQ(b.getChannelDataToRead(ch)[i], a.getChannelDataToRead(ch)[i] * 2);
      }
    }
  }

  {
    const auto samps_to_sum = 3;
    AudioBufferList a(samples, channels);
    AudioBufferList b(samples, channels);
    fill(a, b);
    b.sum(a, samps_to_sum);

    for (int ch = 0; ch < a.getNumOfChannels(); ch++) {
      for (int i = 0; i < samps_to_sum; i++) {
        EXPECT_EQ(b.getChannelDataToRead(ch)[i], a.getChannelDataToRead(ch)[i] * 2);
      }
    }
  }

  {
    const auto samps_to_sum = 3;
    const auto start_ch = 1;
    const auto num_ch = channels / 2;
    AudioBufferList a(samples, channels);
    AudioBufferList b(samples, channels);
    fill(a, b);
    b.sum(a, start_ch, num_ch, samps_to_sum);

    for (int ch = 1; ch < num_ch; ch++) {
      for (int i = 0; i < samps_to_sum; i++) {
        EXPECT_EQ(b.getChannelDataToRead(ch)[i], a.getChannelDataToRead(ch)[i] * 2);
      }
    }
  }
}

TEST_F(AudioBufferListTest, Peak) {
  const int32_t channels = 2;
  const int32_t samples = 7;

  AudioBufferList buffer(samples, channels);
  for (int ch = 0; ch < buffer.getNumOfChannels(); ch++) {
    for (int i = 0; i < samples; i++) {
      buffer.getChannelDataToWrite(ch)[i] = -i * (ch + 1);
    }
  }

  EXPECT_EQ(buffer.getPeak(0, samples), 6);
  EXPECT_EQ(buffer.getPeak(1, samples), 12);
}

TEST_F(AudioBufferListTest, ChannelsAreSilent) {
  {
    AudioBufferList buffer(16 /*samples*/, 2 /*channels*/);
    for (int i = 0; i < buffer.getSamplesPerChannel(); i++) {
      buffer.getChannelDataToWrite(1)[i] = 0.5f;
    }
    EXPECT_FALSE(buffer.channelsAreSilent());
    EXPECT_FALSE(buffer.channelsAreSilent(buffer.getSamplesPerChannel() / 2));
  }

  {
    AudioBufferList buffer(16 /*samples*/, 2 /*channels*/);
    buffer.zero();
    EXPECT_TRUE(buffer.channelsAreSilent());
    EXPECT_TRUE(buffer.channelsAreSilent(buffer.getSamplesPerChannel() / 2));
  }
}

TEST_F(AudioBufferListTest, ChannelIsSilent) {
  AudioBufferList buffer(16 /*samples*/, 2 /*channels*/);

  buffer.zero();
  EXPECT_TRUE(buffer.channelIsSilent(0));
  EXPECT_TRUE(buffer.channelIsSilent(1));

  for (int i = 0; i < buffer.getSamplesPerChannel(); i++) {
    buffer.getChannelDataToWrite(1)[i] = 0.5f;
  }
  EXPECT_TRUE(buffer.channelIsSilent(0));
  EXPECT_FALSE(buffer.channelIsSilent(1));
  EXPECT_TRUE(buffer.channelIsSilent(0, buffer.getSamplesPerChannel() / 2));
  EXPECT_FALSE(buffer.channelIsSilent(1, buffer.getSamplesPerChannel() / 2));
}

TEST_F(AudioBufferListTest, getRMS) {
  unsigned fs = 48000;
  AudioBufferList buf(fs, 3);
  float amplitude = 0.7;

  // sine in channel 0, square wave in channel 1, triangle in channel 2
  auto sinBuf = buf.getChannelDataToWrite(0);
  auto squareBuf = buf.getChannelDataToWrite(1);
  auto triBuf = buf.getChannelDataToWrite(2);
  for (int sample = 0; sample < fs; sample++) {
    double dummy;
    auto ft = sample * 440.f / fs;
    auto frac_ft = modf(ft, &dummy);
    sinBuf[sample] = amplitude * sinf(2 * M_PI * ft);
    squareBuf[sample] = (frac_ft < 0.5) ? amplitude : -amplitude;
    triBuf[sample] = fabs(2 * amplitude * frac_ft - amplitude);
  }

  auto closeEnough = 0.00025;
  EXPECT_NEAR(buf.getRMS(0, fs), amplitude / sqrt(2), closeEnough);
  EXPECT_NEAR(buf.getRMS(1, fs), amplitude, closeEnough);
  EXPECT_NEAR(buf.getRMS(2, fs), amplitude / sqrt(3), closeEnough);
}

TEST_F(AudioBufferListTest, scale) {
  const int32_t numSamples = 512;
  const int32_t numChannels = 10;

  AudioBufferList buffer(numSamples, numChannels);

  for (int32_t c = 0; c < numChannels; ++c) {
    for (int32_t s = 0; s < numSamples; ++s) {
      buffer.getChannelDataToWrite(c)[s] = 1.0f;
    }
  }

  buffer.scale(2.0f);

  for (int32_t c = 0; c < numChannels; ++c) {
    for (int32_t s = 0; s < numSamples; ++s) {
      ASSERT_EQ(buffer.getChannelDataToRead(c)[s], 2.0f);
    }
  }
}
} // namespace SIMD
} // namespace TBE
