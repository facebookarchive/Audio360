/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <assert.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace TBE
{
   template <typename T> struct RegOps
   {
      static size_t width();
      static T zero();
      static T mul(T &a, T &b);
      static T mul(T &a, float &scalar);
      static T add(T &a, T &b);
      static T set(float &val);
      static T mulAcc(T &acc, T &a, T &b);
      static T loadU(const float *buffer);
      static void storeU(float *buffer, T &a);
   };

   /// A helper class for SIMD optimised functions supporting AVX, SSE and NEON. The functions are loaded on runtime
   /// based on the CPU type.
   /// Typical use:
   /// FBDSP dsp;
   /// dsp.multiply(inoutA, inputB, output, numOfSamples);
   class FBDSP
   {
    public:
      /// Multiply two buffers (output[i] = a[i] * b[i])
      /// \param inputA Input buffer A
      /// \param inputB Input buffer B
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      void (*multiply)(const float *inputA, const float *inputB, float *output, size_t numOfSamples){nullptr};

      /// Multiply a buffer with a scalar value (output[i] = input[i] * scalar)
      /// \param input Input buffer
      /// \param scalar Scalar value
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      void (*multiplyScalar)(const float *input, float scalar, float *output, size_t numOfSamples){nullptr};

      /// Add two buffers (output[i] = a[i] + b[i])
      /// \param inputA Input buffer A
      /// \param inputB Input buffer B
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      void (*add)(const float *inputA, const float *inputB, float *output, size_t numOfSamples){nullptr};

      /// Add a scalar value to every element in a buffer (output[i] = input[i] + scalar)
      /// \param input Input buffer
      /// \param scalar Scalar value
      /// \param output Output buffer where the result is written to
      /// \param numOfSamples Number of samples in the buffers
      void (*addScalar)(const float *input, float scalar, float *output, size_t numOfSamples){nullptr};

      /// Multiply the input buffer with a scalar value and then sum result with another buffer (output[i] = (a[i] *
      /// scalar) + b[i]) \param inputToScale Buffer to scale \param scalar Scalar value applied to inputToScale \param
      /// bufferToAdd Buffer to add to inputToScale after it is scaled \param output Output buffer where the result is
      /// written to \param numOfSamples Number of samples in the buffers
      void (*multiplyInputAndAdd)(const float *inputToScale, float scalar, const float *bufferToAdd, float *output,
                                  size_t numOfSamples){nullptr};

      FBDSP();
   };

   class FIR
   {
    public:
      using UPtr = std::unique_ptr<FIR>;
      using IRMem = std::unique_ptr<float[]>;

      //
      // The FIR uses twice the numTaps for its delay line buffer, because it
      // processes the delayline in paralel and uses the second half as a temp
      // place to store the incoming input samples
      //
      FIR(size_t numTaps);
      FIR(const float *ir, size_t numTaps);
      void process(const float *input, float *output, size_t numSamples);

    private:
      void init();
      void init(float const *ir, size_t numSamples);
      void setIR(float const *ir, size_t numSamples); // NOT thread safe in any way!
      void processSerial(const float *input, float *output, size_t numSamples);
      void processAVX(const float *input, float *output, size_t numSamples);

      template <typename TReg> void process(const float *input, float *output, size_t numSamples)
      {
         size_t const regWidth = RegOps<TReg>::width();
         size_t const numTaps = numTaps_;

         if (numSamples < regWidth)
         {
            processSerial(input, output, numSamples);
            return;
         }

         TReg c1;
         TReg i1;
         TReg i2;
         TReg i3;

         TReg acc1;
         TReg acc2;
         TReg acc3;

         size_t inputIdx = 0;  // the idx in the delay line and the input buffer
         size_t outputIdx = 0; // The delay line is stored in the output buffer
         size_t coefIdx = 0;

         // Store the begining samples from the input in the second half of the delay line
         // after the current delay. This will be used for the initial paralel run.
         size_t len = numSamples < numTaps ? numSamples : numTaps;
         memcpy(&delay_.get()[numTaps], input, sizeof(float) * len);

         outputIdx = 0;
         while (inputIdx < numTaps && outputIdx + regWidth <= numSamples)
         {
            acc1 = RegOps<TReg>::zero();

            for (coefIdx = 0; coefIdx < numTaps; ++coefIdx)
            {
               assert(inputIdx + coefIdx < numTaps * 2);
               c1 = RegOps<TReg>::set(ir_[coefIdx]);
               i1 = RegOps<TReg>::loadU(delay_.get() + inputIdx + coefIdx + 1);
               acc1 = RegOps<TReg>::mulAcc(acc1, i1, c1);
            }
            assert(outputIdx < numSamples);
            RegOps<TReg>::storeU(output + outputIdx, acc1);
            inputIdx += regWidth;
            outputIdx += regWidth;
         }

         //
         // If all that remains are a few samples that can't fit in a line
         // process them here taking the input from the delay_ line
         //
         if (numSamples - outputIdx < 8)
         {
            float outputSample = 0;
            while (outputIdx < numSamples)
            {
               outputSample = 0;
               for (size_t i = 0; i < numTaps; ++i)
               {
                  outputSample += delay_.get()[numTaps + outputIdx - i] * ir_[numTaps - 1 - i];
               }
               assert(outputIdx < numSamples);
               output[outputIdx++] = outputSample;
            }
         }

         //
         // Move the delay line with the number of samples incoming
         //
         size_t tailSamples = numSamples > numTaps ? numTaps : numSamples;
         if (tailSamples < numTaps)
         {
            memcpy(delay_.get(), &delay_.get()[tailSamples], (numTaps - tailSamples) * sizeof(float));
         }

         //
         // Now copy the next delay line from the input samples behind
         //
         assert(tailSamples <= numTaps);
         assert(tailSamples <= numSamples);

         size_t const delayDestIdx = numTaps - tailSamples;
         size_t const delaySrcIdx = numSamples - tailSamples;

         assert(delayDestIdx + tailSamples <= numTaps);
         memcpy(&delay_.get()[delayDestIdx], &input[delaySrcIdx], tailSamples * sizeof(float));

         //
         // Now process the pipeline.
         // Note: Performance starts degrading after 3 lines of AVX registers
         //
         inputIdx = 0;
         outputIdx = inputIdx + int(numTaps - 1);

         while (outputIdx + 3 * regWidth < numSamples)
         {
            assert(outputIdx < numSamples);
            acc1 = RegOps<TReg>::zero();
            acc2 = acc1;
            acc3 = acc1;

            for (coefIdx = 0; coefIdx < numTaps; ++coefIdx)
            {
               c1 = RegOps<TReg>::set(ir_[coefIdx]);
               i1 = RegOps<TReg>::loadU(input + inputIdx + coefIdx);
               i2 = RegOps<TReg>::loadU(input + inputIdx + regWidth + coefIdx);
               i3 = RegOps<TReg>::loadU(input + inputIdx + 2 * regWidth + coefIdx);

               acc1 = RegOps<TReg>::mulAcc(acc1, i1, c1);
               acc2 = RegOps<TReg>::mulAcc(acc2, i2, c1);
               acc3 = RegOps<TReg>::mulAcc(acc3, i3, c1);
            }
            RegOps<TReg>::storeU(output + outputIdx, acc1);
            RegOps<TReg>::storeU(output + outputIdx + regWidth, acc2);
            RegOps<TReg>::storeU(output + outputIdx + 2 * regWidth, acc3);
            inputIdx += 3 * regWidth;
            outputIdx += 3 * regWidth;
         }

         while (outputIdx + 2 * regWidth < numSamples)
         {
            assert(outputIdx < numSamples);
            acc1 = RegOps<TReg>::zero();
            acc2 = acc1;
            for (coefIdx = 0; coefIdx < numTaps; ++coefIdx)
            {
               c1 = RegOps<TReg>::set(ir_[coefIdx]);
               i1 = RegOps<TReg>::loadU(input + inputIdx + coefIdx);
               i2 = RegOps<TReg>::loadU(input + inputIdx + regWidth + coefIdx);
               acc1 = RegOps<TReg>::mulAcc(acc1, i1, c1);
               acc2 = RegOps<TReg>::mulAcc(acc2, i2, c1);
            }
            RegOps<TReg>::storeU(output + outputIdx, acc1);
            RegOps<TReg>::storeU(output + outputIdx + regWidth, acc2);
            inputIdx += 2 * regWidth;
            outputIdx += 2 * regWidth;
         }

         while (outputIdx + regWidth < numSamples)
         {
            assert(outputIdx < numSamples);
            acc1 = RegOps<TReg>::zero();
            for (coefIdx = 0; coefIdx < numTaps; ++coefIdx)
            {
               c1 = RegOps<TReg>::set(ir_[coefIdx]);
               i1 = RegOps<TReg>::loadU(input + inputIdx + coefIdx);
               acc1 = RegOps<TReg>::mulAcc(acc1, i1, c1);
            }
            RegOps<TReg>::storeU(output + outputIdx, acc1);
            inputIdx += regWidth;
            outputIdx += regWidth;
         }

         //
         // Do a serial run for the last few samples
         //
         float outputSample = 0;
         while (outputIdx < numSamples)
         {
            outputSample = 0;
            for (size_t i = 0; i < numTaps; ++i)
            {
               assert(outputIdx - i >= 0);
               assert(outputIdx - i < numSamples);
               outputSample += input[outputIdx - i] * ir_[numTaps - 1 - i];
            }
            assert(outputIdx < numSamples);
            output[outputIdx++] = outputSample;
         }
      }

      const bool avxAvailable_;
      size_t numTaps_;
      IRMem ir_;
      IRMem delay_;
   };
} // namespace TBE
