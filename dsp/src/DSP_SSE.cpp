/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef __ARM_NEON

#include "CpuFeatures.hh"
#include "DSP.hh"
#include "Internal.hh"
#include "immintrin.h"
#include "xmmintrin.h"
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace TBE {
template <>
struct RegOps<__m128> {
  static size_t width() {
    return 4;
  }

  static __m128 zero() {
    return _mm_setzero_ps();
  }

  static __m128 mul(__m128& a, __m128& b) {
    return _mm_mul_ps(a, b);
  }

  static __m128 mul(__m128& v, float& scalar) {
    auto s = set(scalar);
    return mul(v, s);
  }

  static __m128 add(__m128& a, __m128& b) {
    return _mm_add_ps(a, b);
  }

  static __m128 set(float& val) {
    return _mm_set1_ps(val);
  }

  static __m128 mulAcc(__m128& acc, __m128& a, __m128& b) {
    return _mm_add_ps(acc, _mm_mul_ps(a, b));
  }

  static __m128 loadU(const float* buffer) {
    return _mm_loadu_ps(buffer);
  }

  static void storeU(float* buffer, __m128& a) {
    _mm_storeu_ps(buffer, a);
  }
};

//-----------------------------------
void dspInitSSE(FBDSP* d) {
  Internal::dspInit<__m128>(d);
}

//-----------------------------------
void FIR::processSSE(const float* input, float* output, size_t numSamples) {
  process<__m128>(input, output, numSamples);
}
} // namespace TBE

#endif // __ARM_NEON
