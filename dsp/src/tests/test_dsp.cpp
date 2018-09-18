/*
 Copyright (c) 2018-present, Facebook, Inc.

 This source code is licensed under the MIT license found in the
 LICENSE file in the root directory of this source tree.
 */

#include "../DSP.hh"
#include "../Internal.hh"
#include "gtest/gtest.h"

namespace TBE {
TEST(FBDSP, Multiply) {
  FBDSP dsp;
  const size_t numSamples = 11;
  const float inA[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  const float inB[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  float out[numSamples] = {0.f};

  dsp.multiply(inA, inB, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], inA[i] * inB[i]) << " Idx " << i;
  }

  // Repeat the test with non-vectorized implementation
  Internal::dspInit<float>(&dsp);
  dsp.multiply(inA, inB, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], inA[i] * inB[i]) << " Idx " << i;
  }
}

TEST(FBDSP, MultiplyScalar) {
  FBDSP dsp;
  const size_t numSamples = 11;
  const float in[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  const float scalar = 4.5f;
  float out[numSamples] = {0.f};

  dsp.multiplyScalar(in, scalar, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], in[i] * scalar) << " Idx " << i;
  }

  Internal::dspInit<float>(&dsp);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], in[i] * scalar) << " Idx " << i;
  }
}

TEST(FBDSP, Add) {
  FBDSP dsp;
  const size_t numSamples = 11;
  const float inA[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  const float inB[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  float out[numSamples] = {0.f};

  dsp.add(inA, inB, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], inA[i] + inB[i]) << " Idx " << i;
  }

  Internal::dspInit<float>(&dsp);
  dsp.add(inA, inB, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], inA[i] + inB[i]) << " Idx " << i;
  }
}

TEST(FBDSP, AddScalar) {
  FBDSP dsp;
  const size_t numSamples = 11;
  const float in[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  const float scalar = 4.5f;
  float out[numSamples] = {0.f};

  dsp.addScalar(in, scalar, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], in[i] + scalar) << " Idx " << i;
  }

  Internal::dspInit<float>(&dsp);
  dsp.addScalar(in, scalar, out, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(out[i], in[i] + scalar) << " Idx " << i;
  }
}

TEST(FBDSP, MultiplyInputAndAdd) {
  FBDSP dsp;
  const size_t numSamples = 11;
  const float in[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  float bufferToAdd[numSamples] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
  const float scalar = 4.5f;
  float output[numSamples] = {0.f};

  dsp.multiplyInputAndAdd(in, scalar, bufferToAdd, output, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(output[i], (in[i] * scalar) + bufferToAdd[i]) << " Idx " << i;
  }

  Internal::dspInit<float>(&dsp);
  dsp.multiplyInputAndAdd(in, scalar, bufferToAdd, output, numSamples);
  for (size_t i = 0; i < numSamples; ++i) {
    ASSERT_EQ(output[i], (in[i] * scalar) + bufferToAdd[i]) << " Idx " << i;
  }
}

TEST(FBDSP, isBufferSilent) {
  FBDSP dsp;
  const size_t numSamples = 11;

  const float in[numSamples] = {
      -0.9238, 0.9907, -0.5498, -0.6346, -0.7374, 0.8311, 0.2004, 0.4803, -0.6001, 0.1372, -0.7094};
  ASSERT_FALSE(dsp.isBufferSilent(in, numSamples));

  const float in2[numSamples] = {-0.9238,
                                 -0.9907,
                                 -0.5498,
                                 -0.6346,
                                 -0.7374,
                                 -0.8311,
                                 -0.2004,
                                 -0.4803,
                                 -0.6001,
                                 -0.1372,
                                 -0.7094};
  ASSERT_FALSE(dsp.isBufferSilent(in2, numSamples));

  const float in3[numSamples] = {-0.0, 0.0, -0.0, -0.0, -0.0001, 0.0001, 0.0, 0.0, -0.0, 0.0, -0.0};
  ASSERT_FALSE(dsp.isBufferSilent(in3, numSamples));

  const float in4[numSamples] = {
      -0.0, 0.0, -0.0, -0.0, -0.00001, 0.00001, 0.0, 0.0, -0.0, 0.0, -0.0};
  ASSERT_TRUE(dsp.isBufferSilent(in4, numSamples));
}

// For testing sake this code is from the ICST library
void fir(
    float* buffer,
    int numOfSamples,
    const float* filterCoeffs,
    int order,
    float* continuationBuffer) {
  int i, j;
  float acc;

  int len = std::min(order, numOfSamples);

  float* temp = new float[order];

  for (i = 0; i < len; i++) {
    temp[i] = buffer[numOfSamples - i - 1];
  }

  for (i = len; i < order; i++) {
    temp[i] = continuationBuffer[i - len];
  }

  for (i = numOfSamples - 1; i >= order; i--) {
    acc = filterCoeffs[0] * buffer[i];

    for (j = 1; j <= order; j++) {
      acc += (filterCoeffs[j] * buffer[i - j]);
    }

    buffer[i] = acc;
  }

  for (i = len - 1; i >= 0; i--) {
    acc = filterCoeffs[0] * buffer[i];
    for (j = 1; j <= i; j++) {
      acc += (filterCoeffs[j] * buffer[i - j]);
    }
    for (j = i + 1; j <= order; j++) {
      acc += (filterCoeffs[j] * continuationBuffer[j - i - 1]);
    }
    buffer[i] = acc;
  }
  for (i = 0; i < order; i++) {
    continuationBuffer[i] = temp[i];
  }
  delete[] temp;
}
} // namespace TBE

TEST(FBDSP, FIRSignalTests) {
  {
    // Test minimum order and numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 8;
    size_t const numSamples = 8;
    size_t const numResult = numTaps + numSamples - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {1, 4, 10, 20, 35, 56, 84, 120, 147, 164, 170, 164, 145, 112, 64};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);
    filter.process(signal.get(), output.get(), 8);
    filter.process(zero.get(), output.get() + numSamples, 7);

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order > numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 128;
    size_t const numSamples = 8;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,    4,    10,   20,   35,   56,   84,   120,  156,  192,  228,  264,  300,  336,  372,
        408,  444,  480,  516,  552,  588,  624,  660,  696,  732,  768,  804,  840,  876,  912,
        948,  984,  1020, 1056, 1092, 1128, 1164, 1200, 1236, 1272, 1308, 1344, 1380, 1416, 1452,
        1488, 1524, 1560, 1596, 1632, 1668, 1704, 1740, 1776, 1812, 1848, 1884, 1920, 1956, 1992,
        2028, 2064, 2100, 2136, 2172, 2208, 2244, 2280, 2316, 2352, 2388, 2424, 2460, 2496, 2532,
        2568, 2604, 2640, 2676, 2712, 2748, 2784, 2820, 2856, 2892, 2928, 2964, 3000, 3036, 3072,
        3108, 3144, 3180, 3216, 3252, 3288, 3324, 3360, 3396, 3432, 3468, 3504, 3540, 3576, 3612,
        3648, 3684, 3720, 3756, 3792, 3828, 3864, 3900, 3936, 3972, 4008, 4044, 4080, 4116, 4152,
        4188, 4224, 4260, 4296, 4332, 4368, 4404, 4440, 4347, 4124, 3770, 3284, 2665, 1912, 1024};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.process(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order < numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 16;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,     4,     10,    20,    35,    56,    84,    120,   165,   220,   286,   364,   455,
        560,   680,   816,   952,   1088,  1224,  1360,  1496,  1632,  1768,  1904,  2040,  2176,
        2312,  2448,  2584,  2720,  2856,  2992,  3128,  3264,  3400,  3536,  3672,  3808,  3944,
        4080,  4216,  4352,  4488,  4624,  4760,  4896,  5032,  5168,  5304,  5440,  5576,  5712,
        5848,  5984,  6120,  6256,  6392,  6528,  6664,  6800,  6936,  7072,  7208,  7344,  7480,
        7616,  7752,  7888,  8024,  8160,  8296,  8432,  8568,  8704,  8840,  8976,  9112,  9248,
        9384,  9520,  9656,  9792,  9928,  10064, 10200, 10336, 10472, 10608, 10744, 10880, 11016,
        11152, 11288, 11424, 11560, 11696, 11832, 11968, 12104, 12240, 12376, 12512, 12648, 12784,
        12920, 13056, 13192, 13328, 13464, 13600, 13736, 13872, 14008, 14144, 14280, 14416, 14552,
        14688, 14824, 14960, 15096, 15232, 15368, 15504, 15640, 15776, 15912, 16048, 16055, 15932,
        15678, 15292, 14773, 14120, 13332, 12408, 11347, 10148, 8810,  7332,  5713,  3952,  2048};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.process(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order == numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 128;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45760,  47905,  50116,
        52394,  54740,  57155,  59640,  62196,  64824,  67525,  70300,  73150,  76076,  79079,
        82160,  85320,  88560,  91881,  95284,  98770,  102340, 105995, 109736, 113564, 117480,
        121485, 125580, 129766, 134044, 138415, 142880, 147440, 152096, 156849, 161700, 166650,
        171700, 176851, 182104, 187460, 192920, 198485, 204156, 209934, 215820, 221815, 227920,
        234136, 240464, 246905, 253460, 260130, 266916, 273819, 280840, 287980, 295240, 302621,
        310124, 317750, 325500, 333375, 341376, 349504, 357760, 365887, 373884, 381750, 389484,
        397085, 404552, 411884, 419080, 426139, 433060, 439842, 446484, 452985, 459344, 465560,
        471632, 477559, 483340, 488974, 494460, 499797, 504984, 510020, 514904, 519635, 524212,
        528634, 532900, 537009, 540960, 544752, 548384, 551855, 555164, 558310, 561292, 564109,
        566760, 569244, 571560, 573707, 575684, 577490, 579124, 580585, 581872, 582984, 583920,
        584679, 585260, 585662, 585884, 585925, 585784, 585460, 584952, 584259, 583380, 582314,
        581060, 579617, 577984, 576160, 574144, 571935, 569532, 566934, 564140, 561149, 557960,
        554572, 550984, 547195, 543204, 539010, 534612, 530009, 525200, 520184, 514960, 509527,
        503884, 498030, 491964, 485685, 479192, 472484, 465560, 458419, 451060, 443482, 435684,
        427665, 419424, 410960, 402272, 393359, 384220, 374854, 365260, 355437, 345384, 335100,
        324584, 313835, 302852, 291634, 280180, 268489, 256560, 244392, 231984, 219335, 206444,
        193310, 179932, 166309, 152440, 138324, 123960, 109347, 94484,  79370,  64004,  48385,
        32512,  16384};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.process(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 13;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,     4,     10,    20,    35,    56,    84,    120,   165,   220,   286,   364,   455,
        546,   637,   728,   819,   910,   1001,  1092,  1183,  1274,  1365,  1456,  1547,  1638,
        1729,  1820,  1911,  2002,  2093,  2184,  2275,  2366,  2457,  2548,  2639,  2730,  2821,
        2912,  3003,  3094,  3185,  3276,  3367,  3458,  3549,  3640,  3731,  3822,  3913,  4004,
        4095,  4186,  4277,  4368,  4459,  4550,  4641,  4732,  4823,  4914,  5005,  5096,  5187,
        5278,  5369,  5460,  5551,  5642,  5733,  5824,  5915,  6006,  6097,  6188,  6279,  6370,
        6461,  6552,  6643,  6734,  6825,  6916,  7007,  7098,  7189,  7280,  7371,  7462,  7553,
        7644,  7735,  7826,  7917,  8008,  8099,  8190,  8281,  8372,  8463,  8554,  8645,  8736,
        8827,  8918,  9009,  9100,  9191,  9282,  9373,  9464,  9555,  9646,  9737,  9828,  9919,
        10010, 10101, 10192, 10283, 10374, 10465, 10556, 10647, 10738, 10829, 10920, 10882, 10714,
        10415, 9984,  9420,  8722,  7889,  6920,  5814,  4570,  3187,  1664};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.process(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order and weird numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 123;
    size_t const numSamples = 63;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45696,  47712,  49728,
        51744,  53760,  55776,  57792,  59808,  61824,  63840,  65856,  67872,  69888,  71904,
        73920,  75936,  77952,  79968,  81984,  84000,  86016,  88032,  90048,  92064,  94080,
        96096,  98112,  100128, 102144, 104160, 106176, 108192, 110208, 112224, 114240, 116256,
        118272, 120288, 122304, 124320, 126336, 128352, 130368, 132384, 134400, 136416, 138432,
        140448, 142464, 144480, 146496, 148512, 150528, 152544, 154560, 156576, 158592, 160608,
        162624, 164640, 166532, 168299, 169940, 171454, 172840, 174097, 175224, 176220, 177084,
        177815, 178412, 178874, 179200, 179389, 179440, 179352, 179124, 178755, 178244, 177590,
        176792, 175849, 174760, 173524, 172140, 170607, 168924, 167090, 165104, 162965, 160672,
        158224, 155620, 152859, 149940, 146862, 143624, 140225, 136664, 132940, 129052, 124999,
        120780, 116394, 111840, 107117, 102224, 97160,  91924,  86515,  80932,  75174,  69240,
        63129,  56840,  50372,  43724,  36895,  29884,  22690,  15312,  7749};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.process(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order and weird numSamples and weird chunking
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 123;
    size_t const numSamples = 63;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numTaps]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45696,  47712,  49728,
        51744,  53760,  55776,  57792,  59808,  61824,  63840,  65856,  67872,  69888,  71904,
        73920,  75936,  77952,  79968,  81984,  84000,  86016,  88032,  90048,  92064,  94080,
        96096,  98112,  100128, 102144, 104160, 106176, 108192, 110208, 112224, 114240, 116256,
        118272, 120288, 122304, 124320, 126336, 128352, 130368, 132384, 134400, 136416, 138432,
        140448, 142464, 144480, 146496, 148512, 150528, 152544, 154560, 156576, 158592, 160608,
        162624, 164640, 166532, 168299, 169940, 171454, 172840, 174097, 175224, 176220, 177084,
        177815, 178412, 178874, 179200, 179389, 179440, 179352, 179124, 178755, 178244, 177590,
        176792, 175849, 174760, 173524, 172140, 170607, 168924, 167090, 165104, 162965, 160672,
        158224, 155620, 152859, 149940, 146862, 143624, 140225, 136664, 132940, 129052, 124999,
        120780, 116394, 111840, 107117, 102224, 97160,  91924,  86515,  80932,  75174,  69240,
        63129,  56840,  50372,  43724,  36895,  29884,  22690,  15312,  7749};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numTaps * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    //
    // This is intentionally weird to confirm the filter can handle weird chunks.
    // Breaking down the 185 result samples into chunks of:
    // 60 + 3 + 100 + 20 + 2 = 185
    //
    //
    filter.process(signal.get(), output.get(), 60);
    filter.process(signal.get() + 60, output.get() + 60, 3);
    filter.process(zero.get(), output.get() + 63, 100);
    filter.process(zero.get(), output.get() + 163, 20);
    filter.process(zero.get(), output.get() + 183, 2);

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }
} // FIRSignalTests

TEST(FBDSP, FIRSignalTestsNoSIMD) {
  {
    // Test minimum order and numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 8;
    size_t const numSamples = 8;
    size_t const numResult = numTaps + numSamples - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {1, 4, 10, 20, 35, 56, 84, 120, 147, 164, 170, 164, 145, 112, 64};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);
    filter.processLinear(signal.get(), output.get(), 8);
    filter.processLinear(zero.get(), output.get() + numSamples, 7);

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order > numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 128;
    size_t const numSamples = 8;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,    4,    10,   20,   35,   56,   84,   120,  156,  192,  228,  264,  300,  336,  372,
        408,  444,  480,  516,  552,  588,  624,  660,  696,  732,  768,  804,  840,  876,  912,
        948,  984,  1020, 1056, 1092, 1128, 1164, 1200, 1236, 1272, 1308, 1344, 1380, 1416, 1452,
        1488, 1524, 1560, 1596, 1632, 1668, 1704, 1740, 1776, 1812, 1848, 1884, 1920, 1956, 1992,
        2028, 2064, 2100, 2136, 2172, 2208, 2244, 2280, 2316, 2352, 2388, 2424, 2460, 2496, 2532,
        2568, 2604, 2640, 2676, 2712, 2748, 2784, 2820, 2856, 2892, 2928, 2964, 3000, 3036, 3072,
        3108, 3144, 3180, 3216, 3252, 3288, 3324, 3360, 3396, 3432, 3468, 3504, 3540, 3576, 3612,
        3648, 3684, 3720, 3756, 3792, 3828, 3864, 3900, 3936, 3972, 4008, 4044, 4080, 4116, 4152,
        4188, 4224, 4260, 4296, 4332, 4368, 4404, 4440, 4347, 4124, 3770, 3284, 2665, 1912, 1024};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.processLinear(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order < numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 16;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,     4,     10,    20,    35,    56,    84,    120,   165,   220,   286,   364,   455,
        560,   680,   816,   952,   1088,  1224,  1360,  1496,  1632,  1768,  1904,  2040,  2176,
        2312,  2448,  2584,  2720,  2856,  2992,  3128,  3264,  3400,  3536,  3672,  3808,  3944,
        4080,  4216,  4352,  4488,  4624,  4760,  4896,  5032,  5168,  5304,  5440,  5576,  5712,
        5848,  5984,  6120,  6256,  6392,  6528,  6664,  6800,  6936,  7072,  7208,  7344,  7480,
        7616,  7752,  7888,  8024,  8160,  8296,  8432,  8568,  8704,  8840,  8976,  9112,  9248,
        9384,  9520,  9656,  9792,  9928,  10064, 10200, 10336, 10472, 10608, 10744, 10880, 11016,
        11152, 11288, 11424, 11560, 11696, 11832, 11968, 12104, 12240, 12376, 12512, 12648, 12784,
        12920, 13056, 13192, 13328, 13464, 13600, 13736, 13872, 14008, 14144, 14280, 14416, 14552,
        14688, 14824, 14960, 15096, 15232, 15368, 15504, 15640, 15776, 15912, 16048, 16055, 15932,
        15678, 15292, 14773, 14120, 13332, 12408, 11347, 10148, 8810,  7332,  5713,  3952,  2048};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.processLinear(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test order == numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 128;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45760,  47905,  50116,
        52394,  54740,  57155,  59640,  62196,  64824,  67525,  70300,  73150,  76076,  79079,
        82160,  85320,  88560,  91881,  95284,  98770,  102340, 105995, 109736, 113564, 117480,
        121485, 125580, 129766, 134044, 138415, 142880, 147440, 152096, 156849, 161700, 166650,
        171700, 176851, 182104, 187460, 192920, 198485, 204156, 209934, 215820, 221815, 227920,
        234136, 240464, 246905, 253460, 260130, 266916, 273819, 280840, 287980, 295240, 302621,
        310124, 317750, 325500, 333375, 341376, 349504, 357760, 365887, 373884, 381750, 389484,
        397085, 404552, 411884, 419080, 426139, 433060, 439842, 446484, 452985, 459344, 465560,
        471632, 477559, 483340, 488974, 494460, 499797, 504984, 510020, 514904, 519635, 524212,
        528634, 532900, 537009, 540960, 544752, 548384, 551855, 555164, 558310, 561292, 564109,
        566760, 569244, 571560, 573707, 575684, 577490, 579124, 580585, 581872, 582984, 583920,
        584679, 585260, 585662, 585884, 585925, 585784, 585460, 584952, 584259, 583380, 582314,
        581060, 579617, 577984, 576160, 574144, 571935, 569532, 566934, 564140, 561149, 557960,
        554572, 550984, 547195, 543204, 539010, 534612, 530009, 525200, 520184, 514960, 509527,
        503884, 498030, 491964, 485685, 479192, 472484, 465560, 458419, 451060, 443482, 435684,
        427665, 419424, 410960, 402272, 393359, 384220, 374854, 365260, 355437, 345384, 335100,
        324584, 313835, 302852, 291634, 280180, 268489, 256560, 244392, 231984, 219335, 206444,
        193310, 179932, 166309, 152440, 138324, 123960, 109347, 94484,  79370,  64004,  48385,
        32512,  16384};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.processLinear(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 13;
    size_t const numSamples = 128;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,     4,     10,    20,    35,    56,    84,    120,   165,   220,   286,   364,   455,
        546,   637,   728,   819,   910,   1001,  1092,  1183,  1274,  1365,  1456,  1547,  1638,
        1729,  1820,  1911,  2002,  2093,  2184,  2275,  2366,  2457,  2548,  2639,  2730,  2821,
        2912,  3003,  3094,  3185,  3276,  3367,  3458,  3549,  3640,  3731,  3822,  3913,  4004,
        4095,  4186,  4277,  4368,  4459,  4550,  4641,  4732,  4823,  4914,  5005,  5096,  5187,
        5278,  5369,  5460,  5551,  5642,  5733,  5824,  5915,  6006,  6097,  6188,  6279,  6370,
        6461,  6552,  6643,  6734,  6825,  6916,  7007,  7098,  7189,  7280,  7371,  7462,  7553,
        7644,  7735,  7826,  7917,  8008,  8099,  8190,  8281,  8372,  8463,  8554,  8645,  8736,
        8827,  8918,  9009,  9100,  9191,  9282,  9373,  9464,  9555,  9646,  9737,  9828,  9919,
        10010, 10101, 10192, 10283, 10374, 10465, 10556, 10647, 10738, 10829, 10920, 10882, 10714,
        10415, 9984,  9420,  8722,  7889,  6920,  5814,  4570,  3187,  1664};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.processLinear(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order and weird numSamples
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 123;
    size_t const numSamples = 63;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numSamples]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45696,  47712,  49728,
        51744,  53760,  55776,  57792,  59808,  61824,  63840,  65856,  67872,  69888,  71904,
        73920,  75936,  77952,  79968,  81984,  84000,  86016,  88032,  90048,  92064,  94080,
        96096,  98112,  100128, 102144, 104160, 106176, 108192, 110208, 112224, 114240, 116256,
        118272, 120288, 122304, 124320, 126336, 128352, 130368, 132384, 134400, 136416, 138432,
        140448, 142464, 144480, 146496, 148512, 150528, 152544, 154560, 156576, 158592, 160608,
        162624, 164640, 166532, 168299, 169940, 171454, 172840, 174097, 175224, 176220, 177084,
        177815, 178412, 178874, 179200, 179389, 179440, 179352, 179124, 178755, 178244, 177590,
        176792, 175849, 174760, 173524, 172140, 170607, 168924, 167090, 165104, 162965, 160672,
        158224, 155620, 152859, 149940, 146862, 143624, 140225, 136664, 132940, 129052, 124999,
        120780, 116394, 111840, 107117, 102224, 97160,  91924,  86515,  80932,  75174,  69240,
        63129,  56840,  50372,  43724,  36895,  29884,  22690,  15312,  7749};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numSamples * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    for (size_t i = 0; i < numResult; i += numSamples) {
      float* input = (i == 0) ? signal.get() : zero.get();
      size_t samples = (i + numSamples) < numResult ? numSamples : numResult - i;
      filter.processLinear(input, output.get() + i, samples);
    }

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }

  {
    // Test weird order and weird numSamples and weird chunking
    using BufferPtr = std::unique_ptr<float[]>;

    size_t const numTaps = 123;
    size_t const numSamples = 63;
    size_t const numResult = numSamples + numTaps - 1;
    size_t const bufferOverflowSize = 100;
    float const bufferOverflowCheckValue = 22.2222f;

    BufferPtr signal = BufferPtr(new float[numSamples]);
    BufferPtr zero = BufferPtr(new float[numTaps]);
    BufferPtr output = BufferPtr(new float[numResult + bufferOverflowSize]);
    BufferPtr ir = BufferPtr(new float[numTaps]);

    // Result generated with numpy.convolve()
    float result[] = {
        1,      4,      10,     20,     35,     56,     84,     120,    165,    220,    286,
        364,    455,    560,    680,    816,    969,    1140,   1330,   1540,   1771,   2024,
        2300,   2600,   2925,   3276,   3654,   4060,   4495,   4960,   5456,   5984,   6545,
        7140,   7770,   8436,   9139,   9880,   10660,  11480,  12341,  13244,  14190,  15180,
        16215,  17296,  18424,  19600,  20825,  22100,  23426,  24804,  26235,  27720,  29260,
        30856,  32509,  34220,  35990,  37820,  39711,  41664,  43680,  45696,  47712,  49728,
        51744,  53760,  55776,  57792,  59808,  61824,  63840,  65856,  67872,  69888,  71904,
        73920,  75936,  77952,  79968,  81984,  84000,  86016,  88032,  90048,  92064,  94080,
        96096,  98112,  100128, 102144, 104160, 106176, 108192, 110208, 112224, 114240, 116256,
        118272, 120288, 122304, 124320, 126336, 128352, 130368, 132384, 134400, 136416, 138432,
        140448, 142464, 144480, 146496, 148512, 150528, 152544, 154560, 156576, 158592, 160608,
        162624, 164640, 166532, 168299, 169940, 171454, 172840, 174097, 175224, 176220, 177084,
        177815, 178412, 178874, 179200, 179389, 179440, 179352, 179124, 178755, 178244, 177590,
        176792, 175849, 174760, 173524, 172140, 170607, 168924, 167090, 165104, 162965, 160672,
        158224, 155620, 152859, 149940, 146862, 143624, 140225, 136664, 132940, 129052, 124999,
        120780, 116394, 111840, 107117, 102224, 97160,  91924,  86515,  80932,  75174,  69240,
        63129,  56840,  50372,  43724,  36895,  29884,  22690,  15312,  7749};

    memset(output.get(), 0, numResult * sizeof(float));
    memset(zero.get(), 0, numTaps * sizeof(float));

    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      output[i] = bufferOverflowCheckValue;
    }

    for (size_t i = 0; i < numSamples; ++i) {
      signal[i] = i + 1;
    }

    for (size_t i = 0; i < numTaps; ++i) {
      ir[i] = i + 1;
    }

    TBE::FIR filter(const_cast<const float*>(ir.get()), numTaps);

    //
    // This is intentionally weird to confirm the filter can handle weird chunks.
    // Breaking down the 185 result samples into chunks of:
    // 60 + 3 + 100 + 20 + 2 = 185
    //
    //
    filter.processLinear(signal.get(), output.get(), 60);
    filter.processLinear(signal.get() + 60, output.get() + 60, 3);
    filter.processLinear(zero.get(), output.get() + 63, 100);
    filter.processLinear(zero.get(), output.get() + 163, 20);
    filter.processLinear(zero.get(), output.get() + 183, 2);

    for (size_t i = 0; i < numResult; ++i) {
      ASSERT_EQ(output[i], result[i]);
    }

    // Ensure we haven't written out of bounds of our output buffer
    for (size_t i = numResult; i < numResult + bufferOverflowSize; ++i) {
      ASSERT_EQ(output[i], bufferOverflowCheckValue);
    }
  }
} // FIRSignalTestsNoSIMD
