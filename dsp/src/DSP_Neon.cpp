/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "DSP.hh"

#ifdef __ARM_NEON

#include <arm_neon.h>
#include "Internal.hh"

namespace TBE {
template <>
struct RegOps<float32x4_t> {
  static size_t width() {
    return 4;
  }

  static float32x4_t zero() {
    return vdupq_n_f32(0);
  }

  static float32x4_t mul(float32x4_t& a, float32x4_t& b) {
    return vmulq_f32(a, b);
  }

  static float32x4_t mul(float32x4_t& v, float& scalar) {
    return vmulq_n_f32(v, scalar);
  }

  static float32x4_t add(float32x4_t& a, float32x4_t& b) {
    return vaddq_f32(a, b);
  }

  static float32x4_t set(float& val) {
    return vdupq_n_f32(val);
  }

  static float32x4_t mulAcc(float32x4_t& acc, float32x4_t& a, float32x4_t& b) {
    return vmlaq_f32(acc, a, b);
  }

  static float32x4_t loadU(const float* buffer) {
    return vld1q_f32(buffer);
  }

  static void storeU(float* buffer, float32x4_t& a) {
    vst1q_f32(buffer, a);
  }
};

//-----------------------------------

void dspInitNeon(FBDSP* d) {
  Internal::dspInit<float32x4_t>(d);
}

FBDSP::FBDSP() {
  dspInitNeon(this);
}

//-----------------------------------

void FIR::process(const float* input, float* output, size_t numSamples) {
  process<float32x4_t>(input, output, numSamples);
}

void FIR::processAVX(const float*, float*, size_t) {
  assert(false);
}

} // namespace TBE
#endif // __ARM_NEON
