/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "CpuFeatures.hh"
#include "DSP.hh"

namespace TBE {
FIR::FIR(size_t numTaps)
    : avxAvailable_(CPU::avxAvailable()),
      numTaps_(numTaps),
      ir_{new float[numTaps]},
      delay_{new float[2 * numTaps]} {
  assert(numTaps >= 8);
  init();
}

FIR::FIR(const float* ir, size_t numTaps)
    : avxAvailable_(CPU::avxAvailable()),
      numTaps_(numTaps),
      ir_(new float[numTaps]),
      delay_{new float[2 * numTaps]} {
  assert(numTaps >= 8);
  init(ir, numTaps);
}

void FIR::init() {
  memset(ir_.get(), 0, sizeof(float) * numTaps_);
  memset(delay_.get(), 0, sizeof(float) * numTaps_ * 2);
  ir_.get()[numTaps_ - 1] = 1; // Default to Dirac delta function (reversed IR)
}

void FIR::init(float const* ir, size_t numSamples) {
  memset(ir_.get(), 0, sizeof(float) * numTaps_);
  memset(delay_.get(), 0, sizeof(float) * numTaps_ * 2);
  setIR(ir, numSamples);
}

void FIR::setIR(float const* ir, size_t numSamples) {
  assert(numSamples <= numTaps_);

  for (size_t i = 1; i <= numSamples; ++i) {
    ir_[numTaps_ - i] = ir[i - 1]; // reverse the ir...
  };
}

//
// Straight up, non-vectorized implementation for cases where we can not
// use intrinsics
//
void FIR::processLinear(const float* input, float* output, size_t numSamples) {
  float x, y;
  for (size_t i = 0; i < numSamples; ++i) {
    x = input[i];
    y = ir_[numTaps_ - 1] * x;
    for (size_t s = 1; s < numTaps_; ++s) {
      y += ir_[numTaps_ - 1 - s] * delay_[s - 1];
    }
    memcpy(&delay_[1], &delay_[0], sizeof(float) * (numTaps_ - 1));
    delay_[0] = x;
    output[i] = y;
  }
}

//
// Helper function to process remaing samples in delay line
// when using a vectorized implementation
//
void FIR::processSerial(const float* input, float* output, size_t numSamples) {
  assert(numSamples < 8);
  size_t const numTaps = numTaps_;
  //
  // When we have less than eight samples, all will be stored in the
  // delay line
  //
  size_t outputIdx = 0;

  // Store the begining samples from the input in the tail line
  // after the delay line section, this will be used for the run
  size_t len = numSamples < numTaps ? numSamples : numTaps;
  memcpy(&delay_.get()[numTaps], input, sizeof(float) * len);

  float outputSample = 0;
  while (outputIdx < numSamples) {
    outputSample = 0;
    for (size_t i = 0; i < numTaps; ++i) {
      outputSample += delay_.get()[numTaps + outputIdx - i] * ir_[numTaps - 1 - i];
    }
    assert(outputIdx < numSamples);
    output[outputIdx++] = outputSample;
  }

  size_t tailSamples = numSamples > numTaps ? numTaps : numSamples;
  if (tailSamples < numTaps) {
    memcpy(delay_.get(), &delay_.get()[tailSamples], (numTaps - tailSamples) * sizeof(float));
  }

  //
  // Copy the delay line from the input to the second half of the IR buffer
  // so that the first delay line sample is right after the last IR sample
  //
  size_t const delayDestIdx = numTaps - tailSamples;
  size_t const delaySrcIdx = numSamples - tailSamples;
  memcpy(&delay_.get()[delayDestIdx], &input[delaySrcIdx], tailSamples * sizeof(float));
}
} // namespace TBE
