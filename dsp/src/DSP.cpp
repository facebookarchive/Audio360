/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#ifndef __ARM_NEON

#include "DSP.hh"
#include "CpuFeatures.hh"
#include "Internal.hh"

#ifndef TBE_DISABLE_SIMD
#include "immintrin.h"
#include "xmmintrin.h"
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace TBE {

//-----------------------------------
extern void dspInitAVX(FBDSP* d);
extern void dspInitSSE(FBDSP* d);

FBDSP::FBDSP() {
#ifdef TBE_DISABLE_SIMD
  Internal::dspInit<float>(this);
#elif defined(TBE_DISABLE_AVX)
  dspInitSSE(this);
#else
  (CPU::avxAvailable()) ? dspInitAVX(this) : dspInitSSE(this);
#endif
}

//-----------------------------------

void FIR::process(const float* input, float* output, size_t numSamples) {
#ifdef TBE_DISABLE_SIMD
  processLinear(input, output, numSamples);
#elif defined(TBE_DISABLE_AVX)
  processSSE(input, output, numSamples);
#else
  avxAvailable_ ? processAVX(input, output, numSamples) : processSSE(input, output, numSamples);
#endif
}

} // namespace TBE

#endif // __ARM_NEON
