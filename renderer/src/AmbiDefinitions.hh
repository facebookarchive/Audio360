/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cassert>
#include <cstddef>

namespace TBE
{
   /// Ambisonics order configuration
   enum class AmbisonicOrder
   {
      INVALID = -1,
      ORDER_1OA = 1,
      ORDER_2OA = 2,
      ORDER_3OA = 3,
      ORDER_4OA = 4,
      ORDER_5OA = 5,
      ORDER_6OA = 6,
      ORDER_7OA = 7
   };

   struct AmbisonicIRContainer
   {
      /// A struct to pass around impulse response data
      /// \param impulseResponse A pointer to the 2D array containing the ambisonic impulse responses
      /// \param ambiOrder The ambisonic order
      /// \param numberHarmonics The number of harmonics (channels) in the impulse response
      /// \param numberTaps The number of coefficients per impulse response harmonic
      AmbisonicIRContainer(const float **impulseResponse, AmbisonicOrder ambiOrder, size_t numberHarmonics,
                           size_t numberTaps)
          : ir(impulseResponse), ambisonicOrder(ambiOrder), numHarmonics(numberHarmonics), numTaps(numberTaps)
      {
      }

      const float **ir{nullptr};
      AmbisonicOrder ambisonicOrder{AmbisonicOrder::INVALID};
      size_t numHarmonics{0};
      size_t numTaps{0};
   };
} // namespace TBE
