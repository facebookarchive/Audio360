/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "../../dsp/src/DSP.hh"
#include "AmbiDefinitions.hh"

#include <memory>
#include <vector>

namespace TBE
{
   class AmbiSphericalConvolution
   {
    public:
      /// A class to binaurally spatialise an Ambisonic field. Input Ambisonics is assumed to be in ACN channel order,
      /// SN3D normalisation (as proposed by the ambiX specification)
      /// \param maxBufferSize Maximum mono number of samples
      /// \param ambisonicIR Contains impulse response and Ambisonic order information
      AmbiSphericalConvolution(size_t maxBufferSize, AmbisonicIRContainer ambisonicIR);

      /// Process the input Ambisonic audio through the provided Ambisonic to binaural impulse responses
      /// \param ambisonicIn The Ambisonic audio input to be binaurally spatialised as an un-interleaved signal.
      /// ambisonicIn[0][0] = harmonic 0, ambisonicIn[1][0] = harmonic 1, etc
      /// \param binauralOut The rendered stereo binaural output as an un-interleaved signal. binauralOut[0][0] = left,
      /// binauralOut[1][0] right
      /// \param bufferLength The number of samples in a mono buffer
      void process(const float **ambisonicIn, float **binauralOut, size_t bufferLength);

    private:
      AmbisonicIRContainer irs_;
      size_t ambisonicOrder_{0};

      FBDSP dsp_;
      std::unique_ptr<float[]> tmpBuf_;
      std::vector<TBE::FIR> ambiFir_;
   };
} // namespace TBE
