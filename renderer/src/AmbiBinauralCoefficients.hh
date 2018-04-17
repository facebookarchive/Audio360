/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "AmbiDefinitions.hh"
#include <cassert>

namespace TBE
{

   /// Get a container referencing the impulse response for the specified sample rate
   /// \param sampleRate Specify the sample rate
   /// \return An AmbisonicIRContainer struct referencing the impulse response for the specified sample rate
   AmbisonicIRContainer getAmbisonicImpulseResponse(float sampleRate);

}; // namespace TBE
