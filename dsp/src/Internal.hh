/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "DSP.hh"
#include <cassert>

namespace TBE
{
   namespace Internal
   {
      /// Multiply a buffer with a scalar value (output[i] = input[i] * scalar)
      /// \param input Input buffer
      /// \param scalar Scalar value
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      template <typename TReg> void multiplyScalar(const float *input, float scalar, float *output, size_t numOfSamples)
      {
         const auto regWidth = RegOps<TReg>::width();
         size_t samplesLeft = numOfSamples;
         TReg in, out;
         while (samplesLeft >= regWidth)
         {
            in = RegOps<TReg>::loadU(input);
            out = RegOps<TReg>::mul(in, scalar);
            RegOps<TReg>::storeU(output, out);
            input += regWidth;
            output += regWidth;
            samplesLeft -= regWidth;
         }

         while (samplesLeft)
         {
            *output = *input * scalar;
            output++;
            input++;
            samplesLeft--;
         }
      }

      /// Multiply two buffers (output[i] = a[i] * b[i])
      /// \param inputA Input buffer A
      /// \param inputB Input buffer B
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      template <typename TReg>
      void multiply(const float *inputA, const float *inputB, float *output, size_t numOfSamples)
      {
         const auto regWidth = RegOps<TReg>::width();
         size_t samplesLeft = numOfSamples;
         TReg inA, inB, out;
         while (samplesLeft >= regWidth)
         {
            inA = RegOps<TReg>::loadU(inputA);
            inB = RegOps<TReg>::loadU(inputB);
            out = RegOps<TReg>::mul(inA, inB);
            RegOps<TReg>::storeU(output, out);
            inputA += regWidth;
            inputB += regWidth;
            output += regWidth;
            samplesLeft -= regWidth;
         }

         while (samplesLeft)
         {
            *output = *inputA * *inputB;
            output++;
            inputA++;
            inputB++;
            samplesLeft--;
         }
      }

      /// Add a scalar value to every element in a buffer (output[i] = input[i] + scalar)
      /// \param input Input buffer
      /// \param scalar Scalar value
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      template <typename TReg> void addScalar(const float *input, float scalar, float *output, size_t numOfSamples)
      {
         const auto regWidth = RegOps<TReg>::width();
         size_t samplesLeft = numOfSamples;
         TReg in, s, out;
         while (samplesLeft >= regWidth)
         {
            in = RegOps<TReg>::loadU(input);
            s = RegOps<TReg>::set(scalar);
            out = RegOps<TReg>::add(in, s);
            RegOps<TReg>::storeU(output, out);
            input += regWidth;
            output += regWidth;
            samplesLeft -= regWidth;
         }

         while (samplesLeft)
         {
            *output = *input + scalar;
            output++;
            input++;
            samplesLeft--;
         }
      }

      /// Add two buffers (output[i] = a[i] + b[i])
      /// \param inputA Input buffer A
      /// \param inputB Input buffer B
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      template <typename TReg> void add(const float *inputA, const float *inputB, float *output, size_t numOfSamples)
      {
         const auto regWidth = RegOps<TReg>::width();
         size_t samplesLeft = numOfSamples;
         TReg inA, inB, out;
         while (samplesLeft >= regWidth)
         {
            inA = RegOps<TReg>::loadU(inputA);
            inB = RegOps<TReg>::loadU(inputB);
            out = RegOps<TReg>::add(inA, inB);
            RegOps<TReg>::storeU(output, out);
            inputA += regWidth;
            inputB += regWidth;
            output += regWidth;
            samplesLeft -= regWidth;
         }

         while (samplesLeft)
         {
            *output = *inputA + *inputB;
            output++;
            inputA++;
            inputB++;
            samplesLeft--;
         }
      }

      /// Multiply the input buffer with a scalar value and then sum result with another buffer (output[i] = (a[i] *
      /// scalar) + b[i]) \param inputToScale Buffer to scale \param scalar Scalar value applied to inputToScale \param
      /// bufferToAdd Buffer to add to inputToScale after it is scaled \param output Output buffer where the result is
      /// written to \param numOfSamples Number of samples in the buffers
      template <typename TReg>
      void multiplyInputAndAdd(const float *inputToScale, float scalar, const float *bufferToAdd, float *output,
                               size_t numOfSamples)
      {
         const auto regWidth = RegOps<TReg>::width();
         size_t samplesLeft = numOfSamples;
         TReg inToScale, bufferAdd, scalar_vec, scaledInput, summed;
         while (samplesLeft >= regWidth)
         {
            inToScale = RegOps<TReg>::loadU(inputToScale);
            bufferAdd = RegOps<TReg>::loadU(bufferToAdd);

            scalar_vec = RegOps<TReg>::set(scalar);
            scaledInput = RegOps<TReg>::mul(inToScale, scalar_vec);

            summed = RegOps<TReg>::add(scaledInput, bufferAdd);

            RegOps<TReg>::storeU(output, summed);
            inputToScale += regWidth;
            bufferToAdd += regWidth;
            output += regWidth;
            samplesLeft -= regWidth;
         }

         while (samplesLeft)
         {
            *output = (*inputToScale * scalar) + *bufferToAdd;
            output++;
            inputToScale++;
            bufferToAdd++;
            samplesLeft--;
         }
      }

      template <typename T> void dspInit(FBDSP *d)
      {
         assert(d);
         d->multiply = multiply<T>;
         d->multiplyScalar = multiplyScalar<T>;
         d->add = add<T>;
         d->addScalar = addScalar<T>;
         d->multiplyInputAndAdd = multiplyInputAndAdd<T>;
      }

   } // namespace Internal
} // namespace TBE
