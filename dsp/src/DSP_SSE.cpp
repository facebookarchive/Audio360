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

namespace TBE
{
   template <> struct RegOps<__m128>
   {
      static size_t width()
      {
         return 4;
      }

      static __m128 zero()
      {
         return _mm_setzero_ps();
      }

      static __m128 mul(__m128 &a, __m128 &b)
      {
         return _mm_mul_ps(a, b);
      }

      static __m128 mul(__m128 &v, float &scalar)
      {
         auto s = set(scalar);
         return mul(v, s);
      }

      static __m128 add(__m128 &a, __m128 &b)
      {
         return _mm_add_ps(a, b);
      }

      static __m128 set(float &val)
      {
         return _mm_set1_ps(val);
      }

      static __m128 mulAcc(__m128 &acc, __m128 &a, __m128 &b)
      {
         return _mm_add_ps(acc, _mm_mul_ps(a, b));
      }

      static __m128 loadU(const float *buffer)
      {
         return _mm_loadu_ps(buffer);
      }

      static void storeU(float *buffer, __m128 &a)
      {
         _mm_storeu_ps(buffer, a);
      }
   };

   //-----------------------------------

#ifndef TBE_DISABLE_AVX
   extern void dspInitAVX(FBDSP *d);
#endif

   void dspInitSSE(FBDSP *d)
   {
      Internal::dspInit<__m128>(d);
   }

   FBDSP::FBDSP()
   {
#ifndef TBE_DISABLE_AVX
      (CPU::avxAvailable()) ? dspInitAVX(this) : dspInitSSE(this);
#else
      dspInitSSE(this);
#endif
   }

   //-----------------------------------

   void FIR::process(const float *input, float *output, size_t numSamples)
   {
#ifndef TBE_DISABLE_AVX
      if (avxAvailable_)
      {
         processAVX(input, output, numSamples);
      }
      else
      {
#endif // TBE_DISABLE_AVX
         process<__m128>(input, output, numSamples);
#ifndef TBE_DISABLE_AVX
      }
#endif // TBE_DISABLE_AVX
   }
} // namespace TBE

#endif // __ARM_NEON
