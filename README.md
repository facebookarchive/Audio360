# Facebook Audio 360 Renderer

This project presents the Ambisonic to binaural rendering process used for spatial audio playback in 360 videos across the Facebook apps.

This repository contains a set of Head Related Impulse Responses (HRIRs) which have been converted to be applied directly on an Ambisonic signal and an implementation for using these impulse responses to render Ambisonic audio to binaural audio. The implementation uses an optimised FIR filtering class which is also included.

For general discussion and support please use the [Spatial Workstation Facebook group](https://www.facebook.com/groups/audio360support/) and the [Knowledge Base](https://facebookincubator.github.io/facebook-360-spatial-workstation/KB.html)

To hear this code in action you can download builds of the [Facebook Spatial Workstation and Rendering SDK](https://facebook360.fb.com/spatial-workstation/)

## Repository Structure

The repository consists of two parts:

1. dsp - an optimised FIR filtering class, DSP operations and some simple memory management for audio buffers
2. renderer - Ambisonic to binaural rendering processing and impulse response data

## Requirements

This repository will work on Windows, macOS, iOS, Linux and Android. Building the code requires installation of [CMake](https://cmake.org/). In addition, if you want to run the tests then that requires installation of [Google Test](https://github.com/google/googletest).

## Building

**1. Clone the repo**

```
git clone git@github.com:facebookincubator/Audio360.git
```

**2. Build the renderer**

```
cd audio360/renderer
mkdir build && cd build
cmake ..
cmake --build .
```

A static library of the renderer will be built in `audio360/renderer/build`.

If you are building for Android, you can use the toolchain provided in the [Android NDK](https://developer.android.com/ndk/guides/cmake.html).

**3. Build and run tests**

Clone gtest:

```
git clone https://github.com/google/googletest.git /your/path/to/gtest
```

Follow the steps above, but define the path to gtest:

```
cd audio360/renderer
mkdir build && cd build
cmake -DGTEST_SRC_DIR=/your/path/to/gtest ..
cmake --build .
./FBAudioRenderer-tests
```

## Usage Example

1. Build the renderer as described above.
1. In your IDE/build system, add the renderer src directory to the header include path: `audio360/renderer/src`
1. In your IDE/build system, add the renderer and dsp build directory to the linker path: `audio360/renderer/build`; `audio360/renderer/build/dsp`
1. Link your project to the renderer and dsp libraries (on \*nix: -lFBAudioRenderer -ldsp)

In your application, include:  
```cpp
#include "AmbiBinauralCoefficients.hh"
#include "AmbiSphericalConvolution.hh"
```
An instance of the `AmbiSphericalConvolution` can be used to process your ambisonic signal
```cpp
using namespace TBE;
const size_t kBufferSize = 1024;
const float kSampleRate = 48000.f;

// Ambisonic data, un-interleaved
// ambisonicInput[0][0] = harmonic 0
// ambisonicInput[1][0] = harmonic 1
// ambisonicInput[2][0] = harmonic 2
const float **ambisonicInput;

// Binaural output, un-interleaved
// binauralOutput[0][0] = left
// binauralOutput[1][0] = right
float **binauralOutput;

AmbiSphericalConvolution renderer(kBufferSize, AmbisonicOrder::ORDER_2OA, getAmbisonicImpulseResponse(kSampleRate));
// Process loop:
renderer.process(ambisonicInput, binauralOutput, kBufferSize);
```

## A Quick Primer on Ambisonics and Binaural Rendering

Ambisonics is an audio format which describes an entire sound field around a centre point using nothing but a multichannel audio file. To achieve meaningful playback of the audio scene a decoding (or rendering) step is required.

Historically, Ambisonics has been commonly used for playback over real world speaker arrays. However, for a listener armed only with a pair of headphones it is possible to render the Ambisonic audio to binaural audio. In binaural rendering we model the propagation of sound around the human ears, head and shoulders, giving an authentic, life-like experience of belonging in the audio scene. Streaming Ambisonic audio followed by client-side binaural rendering allows the listener to rotate their head within the field in real-time.

## Impulse Responses

The renderer presented in this project uses a set of impulse responses which map an Ambisonic sound field to a stereo binaural signal. These impulse responses have been created by taking a set of Head Related Transfer Function (HRTF) data, sampled at discrete points on the sphere, and summing them together with corresponding spherical harmonic basis coefficients.

It is not possible to achieve all binaural cues effectively at lower Ambisonic orders and additional problems with colouration can arise from the use of non-personalised binaural reproduction techniques. In designing impulse responses for generic use, a balance between various aspects of rendering quality need to be met - this covers common concepts in spatial audio like spatialisation, externalisation, localisation and timbre. The impulse responses used by the Facebook renderer have been generated to provide an effective binaural render whilst preserving audio fidelity with acceptable timbre colouration. As we work on making improvements to our rendering technology we hope to provide updates in this open source repository.

## Rendering Implementation Notes

Spherical harmonics exhibit useful symmetry properties which can be utilised to provide both left and right ear binaural signals. Each harmonic function can be seen as being either mathematically even or odd across the median plane (between left and right hemispheres). When the harmonic is even there is no difference between processing for the left and right ears, when the harmonic is odd the right ear signal is composed with an extra multiply by -1 to get the correct binaural signal output. For this reason, only one set of coefficients is needed to provide rendering information for both left and right ears.

## Contributing

See the CONTRIBUTING file for how to help out.

- For support and general discussion please use the [Spatial Workstation Facebook group](https://www.facebook.com/groups/audio360support/)

- To hear this code in action you can download builds of the [Facebook Spatial Workstation and Rendering SDK](https://facebook360.fb.com/spatial-workstation/)

- Further help and information on the [Knowledge Base](https://facebookincubator.github.io/facebook-360-spatial-workstation/KB.html)

## License

The Facebook Audio 360 Renderer is MIT licensed, as found in the LICENSE file.
