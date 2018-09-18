/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "../../../dsp/src/AudioBufferList.hh"
#include "../AmbiBinauralCoefficients2OA.hh"
#include "../AmbiBinauralCoefficients3OA.hh"
#include "../AmbiSphericalConvolution.hh"
#include "gtest/gtest.h"

#include <cmath>

namespace TBE {
static const int kNumTestTaps[16] =
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

class AmbiSphericalConvolutionTest : public ::testing::Test {
 public:
 protected:
  virtual void SetUp() {
    for (int i = 0; i < kMaxBufferSize; i++) {
      noise_[i] = 2.f * std::rand() / RAND_MAX - 1.f;
    }

    input2OABuf_.zero();
    input3OABuf_.zero();
    kTestIRs_.zero();
    binauralOutBuffer_.zero();

    // create synthetic IRs for a basic two virtual speaker setup on hard left and hard right. Dirac
    // delta, transparent, impulse response with a fixed delay:
    kTestIRs_.getChannelDataToWrite(0)[kIRDelay_] = 1.f;
    kTestIRs_.getChannelDataToWrite(1)[kIRDelay_] = 1.f;
    kTestIRs_.getChannelDataToWrite(2)[kIRDelay_] = 0.f;
    kTestIRs_.getChannelDataToWrite(3)[kIRDelay_] = 0.f;
    kTestIRs_.getChannelDataToWrite(4)[kIRDelay_] = 0.f;
    kTestIRs_.getChannelDataToWrite(5)[kIRDelay_] = 0.f;
    kTestIRs_.getChannelDataToWrite(6)[kIRDelay_] = -0.5f;
    kTestIRs_.getChannelDataToWrite(7)[kIRDelay_] = 0.f;
    kTestIRs_.getChannelDataToWrite(8)[kIRDelay_] = -kSqrt3Over2_;
  }

  virtual void TearDown() {}

  AmbiSphericalConvolutionTest();

  const int kTestSampleRate_ = 48000;
  static const size_t kMaxBufferSize = 1024;
  static const int kNum2OAHarmonics = 9;
  static const int kNum3OAHarmonics = 16;
  static const int kStereoNumChannels = 2;
  float noise_[kMaxBufferSize];

  int kIRDelay_ = 50; // delay input by 50 samples
  const float kSqrt3Over2_ = std::sqrt(3.f) / 2.f;
  float kRMSdBPannedDifference_ = 6.f;
  float kRMSdBToleranceSame_ = 0.1f;

  AudioBufferList input2OABuf_;
  AudioBufferList input3OABuf_;
  AudioBufferList kTestIRs_;
  AudioBufferList binauralOutBuffer_;

  void writeNoiseTo2OAInputBuffer(const float* ambiGains);
};

AmbiSphericalConvolutionTest::AmbiSphericalConvolutionTest()
    : input2OABuf_(kMaxBufferSize, kNum2OAHarmonics),
      input3OABuf_(kMaxBufferSize, kNum3OAHarmonics),
      kTestIRs_(kNumTestTaps[0], kNum2OAHarmonics),
      binauralOutBuffer_(kMaxBufferSize, kStereoNumChannels) {}

void AmbiSphericalConvolutionTest::writeNoiseTo2OAInputBuffer(const float* ambiGains) {
  for (int hm = 0; hm < kNum2OAHarmonics; hm++) {
    for (int i = 0; i < kMaxBufferSize; i++) {
      input2OABuf_.getChannelDataToWrite(hm)[i] = noise_[i] * ambiGains[hm];
    }
  }
}

TEST_F(AmbiSphericalConvolutionTest, processLeftTest) {
  // pass in noise panned hard left and right in an ambisonic field, test the left/right balance in
  // the binaural output makes sense.

  const float ambi_pan_left[kNum2OAHarmonics] = {
      1.f, 1.f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, -kSqrt3Over2_};

  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize, get2OAAmbisonicImpulseResponse(kTestSampleRate_));

  writeNoiseTo2OAInputBuffer(ambi_pan_left);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_GT(left - right, kRMSdBPannedDifference_);
}

TEST_F(AmbiSphericalConvolutionTest, processRightTest) {
  const float ambi_pan_right[kNum2OAHarmonics] = {
      1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, -kSqrt3Over2_};
  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize, get2OAAmbisonicImpulseResponse(kTestSampleRate_));

  writeNoiseTo2OAInputBuffer(ambi_pan_right);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_GT(right - left, kRMSdBPannedDifference_);
}

TEST_F(AmbiSphericalConvolutionTest, processFrontTest) {
  const float ambi_pan_front[kNum2OAHarmonics] = {
      1.f, 0.f, 0.f, 1.f, 0.f, 0.f, -0.5f, 0.f, kSqrt3Over2_};

  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize, get2OAAmbisonicImpulseResponse(kTestSampleRate_));

  writeNoiseTo2OAInputBuffer(ambi_pan_front);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_LT(std::abs(left - right), kRMSdBToleranceSame_);
}

TEST_F(AmbiSphericalConvolutionTest, processDiracLeftTest) {
  const float ambi_pan_left[kNum2OAHarmonics] = {
      1.f, 1.f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, -kSqrt3Over2_};
  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize,
      AmbisonicIRContainer(
          kTestIRs_.getDataReadOnly(), AmbisonicOrder::ORDER_2OA, 9, (int*)kNumTestTaps));

  writeNoiseTo2OAInputBuffer(ambi_pan_left);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // calculate RMS of the first 'kIRDelay_' number of samples, test it should be zero, then test the
  // rest of the buffer for non-zero rms to ensure the sample delay has occured.
  float sq_tot = 0.f;
  for (int i = 0; i < kIRDelay_; i++) {
    sq_tot += binauralOutBuffer_.getChannelDataToRead(0)[i] *
            binauralOutBuffer_.getChannelDataToRead(0)[i] +
        binauralOutBuffer_.getChannelDataToRead(1)[i] *
            binauralOutBuffer_.getChannelDataToRead(1)[i];
  }
  float first_section_rms = std::sqrt(sq_tot / static_cast<float>(kIRDelay_));

  sq_tot = 0.f;
  for (int i = kIRDelay_; i < kMaxBufferSize; i++) {
    sq_tot += binauralOutBuffer_.getChannelDataToRead(0)[i] *
            binauralOutBuffer_.getChannelDataToRead(0)[i] +
        binauralOutBuffer_.getChannelDataToRead(1)[i] *
            binauralOutBuffer_.getChannelDataToRead(1)[i];
  }
  float second_section_rms = std::sqrt(sq_tot / static_cast<float>(kMaxBufferSize - kIRDelay_));

  EXPECT_LT(first_section_rms, 0.0001f);
  EXPECT_GT(second_section_rms, 0.9f);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_GT(left - right, kRMSdBPannedDifference_);
}

TEST_F(AmbiSphericalConvolutionTest, processDiracRightTest) {
  const float ambi_pan_right[kNum2OAHarmonics] = {
      1.f, -1.f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, -kSqrt3Over2_};
  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize,
      AmbisonicIRContainer(
          kTestIRs_.getDataReadOnly(), AmbisonicOrder::ORDER_2OA, 9, (int*)kNumTestTaps));

  writeNoiseTo2OAInputBuffer(ambi_pan_right);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_GT(right - left, kRMSdBPannedDifference_);
}

TEST_F(AmbiSphericalConvolutionTest, processDiracFrontTest) {
  const float ambi_pan_front[kNum2OAHarmonics] = {
      1.f, 0.f, 0.f, 1.f, 0.f, 0.f, -0.5f, 0.f, kSqrt3Over2_};
  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize,
      AmbisonicIRContainer(
          kTestIRs_.getDataReadOnly(), AmbisonicOrder::ORDER_2OA, 9, (int*)kNumTestTaps));

  writeNoiseTo2OAInputBuffer(ambi_pan_front);

  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
  sph_rend.process(input2OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

  // check left/right balance:
  float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
  float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
  EXPECT_LT(std::abs(left - right), kRMSdBToleranceSame_);
}

TEST_F(AmbiSphericalConvolutionTest, passThrough3OA) {
  AmbiSphericalConvolution sph_rend(
      kMaxBufferSize, get3OAAmbisonicImpulseResponse(kTestSampleRate_));

  // put noise in each harmonic seperately and check for non-silent output signal
  for (int hm = 0; hm < kNum3OAHarmonics; hm++) {
    input3OABuf_.zero();

    for (int i = 0; i < kMaxBufferSize; i++) {
      input3OABuf_.getChannelDataToWrite(hm)[i] = noise_[i];
    }

    sph_rend.process(input3OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);
    sph_rend.process(input3OABuf_.getDataReadOnly(), binauralOutBuffer_.getData(), kMaxBufferSize);

    // check left/right balance:
    float left = 20.f * std::log10(binauralOutBuffer_.getRMS(0));
    float right = 20.f * std::log10(binauralOutBuffer_.getRMS(1));
    EXPECT_GT(left + right, -55.f);
  }
}
} // namespace TBE
