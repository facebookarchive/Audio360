/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cassert>
#include "AmbiDefinitions.hh"

namespace TBE {

/// Get a container referencing the impulse response for the specified sample rate
/// \param sampleRate Get the impulse responses for this sample rate
/// \return An AmbisonicIRContainer struct referencing the impulse response for the specified sample
/// rate
AmbisonicIRContainer get3OAAmbisonicImpulseResponse(float sampleRate);

}; // namespace TBE
