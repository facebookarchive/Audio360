/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef TBE_DISABLE_AVX
#if defined(__AVX__)

#include "DSP.hh"
#include "Internal.hh"
#include "immintrin.h"
#include "xmmintrin.h"
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace TBE {
template <>
struct RegOps<__m256> {
  static size_t width() {
    return 8;
  };

  static __m256 zero() {
    return _mm256_setzero_ps();
  }

  static __m256 mul(__m256& a, __m256& b) {
    return _mm256_mul_ps(a, b);
  }

  static __m256 mul(__m256& v, float& scalar) {
    auto s = set(scalar);
    return mul(v, s);
  }

  static __m256 add(__m256& a, __m256& b) {
    return _mm256_add_ps(a, b);
  }

  static __m256 set(float& val) {
    return _mm256_set1_ps(val);
  }

  static __m256 mulAcc(__m256& acc, __m256& a, __m256& b) {
    return _mm256_add_ps(acc, _mm256_mul_ps(a, b));
  }

  static __m256 loadU(const float* buffer) {
    return _mm256_loadu_ps(buffer);
  }

  static void storeU(float* buffer, __m256& a) {
    _mm256_storeu_ps(buffer, a);
  }
};

//-----------------------------------

void dspInitAVX(FBDSP* d) {
  Internal::dspInit<__m256>(d);
}

//-----------------------------------

void FIR::processAVX(const float* input, float* output, size_t numSamples) {
  process<__m256>(input, output, numSamples);
}
} // namespace TBE

#endif // __ARM_NEON
#endif // TBE_DISABLE_AVX
