/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "AmbiSphericalConvolution.hh"
namespace TBE
{
   AmbiSphericalConvolution::AmbiSphericalConvolution(size_t maxBufferSize, AmbisonicIRContainer ambisonicIR)
       : irs_(ambisonicIR), ambisonicOrder_(static_cast<int>(irs_.ambisonicOrder))
   {
      // check for standard Ambisonic harmonic input count. More exotic mixed orders may be included at a later time.
      assert((ambisonicOrder_ + 1) * (ambisonicOrder_ + 1) == irs_.numHarmonics);
      assert(maxBufferSize > 0);
      assert(irs_.ir);
      assert(irs_.ir[0]);

      tmpBuf_ = std::unique_ptr<float[]>(new float[maxBufferSize]);

      // initialise FIR filters:
      for (int hm = 0; hm < irs_.numHarmonics; hm++)
      {
         ambiFir_.emplace_back(ambisonicIR.ir[hm], ambisonicIR.numTaps);
      }
   }

   void AmbiSphericalConvolution::process(const float **ambisonicIn, float **binauralOut, size_t bufferLength)
   {
      assert(binauralOut);
      assert(ambisonicIn);

      memset(binauralOut[0], 0, bufferLength * sizeof(float));
      memset(binauralOut[1], 0, bufferLength * sizeof(float));

      for (int l = 0; l <= ambisonicOrder_; l++)
      {
         for (int m = -l; m <= l; m++)
         {
            const int hm = l * l + l + m;
            memset(tmpBuf_.get(), 0, bufferLength * sizeof(float));

            ambiFir_[hm].process(ambisonicIn[hm], tmpBuf_.get(), bufferLength);

            dsp_.add(binauralOut[0], tmpBuf_.get(), binauralOut[0], bufferLength);

            // flip harmonics with m < 0 for right ear output
            if (m < 0)
            {
               dsp_.multiplyInputAndAdd(tmpBuf_.get(), -1.f, binauralOut[1], binauralOut[1], bufferLength);
            }
            else
            {
               dsp_.add(binauralOut[1], tmpBuf_.get(), binauralOut[1], bufferLength);
            }
         }
      }
   }
} // namespace TBE
