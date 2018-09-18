/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "AmbiSphericalConvolution.hh"
namespace TBE {
AmbiSphericalConvolution::AmbiSphericalConvolution(
    size_t maxBufferSize,
    AmbisonicIRContainer ambisonicIR)
    : maxBufferSize_(maxBufferSize), irs_(ambisonicIR), ambisonicOrder_(static_cast<int>(irs_.ambisonicOrder)) {
  // check for standard Ambisonic harmonic input count. More exotic mixed orders may be included at
  // a later time.
  assert((ambisonicOrder_ + 1) * (ambisonicOrder_ + 1) == irs_.numHarmonics);
  assert(maxBufferSize > 0);
  assert(irs_.ir);
  assert(irs_.ir[0]);

  tmpBuf_ = std::unique_ptr<float[]>(new float[maxBufferSize]);
  oddHmBuf_ = std::unique_ptr<float[]>(new float[maxBufferSize]);
  silenceCounts_ = std::unique_ptr<int[]>(new int[irs_.numHarmonics]);

  int max_num_taps = 0;

  if (irs_.numTapsVec) {
    for (int hm = 0; hm < irs_.numHarmonics; hm++) {
      const int num_taps = irs_.numTapsVec[hm];
      max_num_taps = (num_taps > max_num_taps) ? num_taps : max_num_taps;
    }
  }

  // initialise FIR filters:
  for (int hm = 0; hm < irs_.numHarmonics; hm++) {
    ambiFir_.emplace_back(irs_.ir[hm], ambisonicIR.numTapsVec[hm]);
    silenceCounts_.get()[hm] = 0;
  }
}

void AmbiSphericalConvolution::process(
    const float** ambisonicIn,
    float** binauralOut,
    int bufferLength) {
  assert(binauralOut);
  assert(ambisonicIn);
  assert(bufferLength <= maxBufferSize_);

  memset(binauralOut[0], 0, bufferLength * sizeof(float));
  memset(oddHmBuf_.get(), 0, bufferLength * sizeof(float));

  for (int l = 0; l <= ambisonicOrder_; l++) {
    for (int m = -l; m <= l; m++) {
      const int hm = l * l + l + m;
      memset(tmpBuf_.get(), 0, bufferLength * sizeof(float));

      if (dsp_.isBufferSilent(ambisonicIn[hm], bufferLength)) {
        silenceCounts_.get()[hm]++;
      } else {
        silenceCounts_.get()[hm] = 0;
      }

      if (silenceCounts_.get()[hm] > 1) {
        continue;
      }

      ambiFir_[hm].process(ambisonicIn[hm], tmpBuf_.get(), bufferLength);

      // flip harmonics with m < 0 for right ear output
      if (m < 0) {
        dsp_.add(tmpBuf_.get(), oddHmBuf_.get(), oddHmBuf_.get(), bufferLength);
      } else {
        dsp_.add(binauralOut[0], tmpBuf_.get(), binauralOut[0], bufferLength);
      }
    }
  }

  dsp_.multiplyInputAndAdd(oddHmBuf_.get(), -1.f, binauralOut[0], binauralOut[1], bufferLength);
  dsp_.add(oddHmBuf_.get(), binauralOut[0], binauralOut[0], bufferLength);
}
} // namespace TBE
