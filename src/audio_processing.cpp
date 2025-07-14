#include "audio_processing.hpp"

#include <cmath>
#include <cstddef>

#include "read_wav.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Convert bytes into 16 bit numbers
std::vector<int16_t> convertBytes(WavHeader header,
                                  std::vector<std::byte>& soundData) {
  std::vector<int16_t> samples;
  if (header.bitsPerSample == bits16perSample) {
    samples.reserve(soundData.size() / 2);

    for (size_t i = 0; i < soundData.size(); i += 2) {
      const std::byte byte1 = soundData[i];
      const std::byte byte2 = soundData[i + 1];

      const auto sample = static_cast<int16_t>(
          (static_cast<uint8_t>(byte2) << 8) | static_cast<uint8_t>(byte1));

      samples.push_back(sample);
    }
  }

  return samples;
}

// chunk the audio data
std::vector<std::vector<double>> sampleToBlock(std::vector<int16_t>& samples) {
  const double absoluteValue = 32768.0;
  const size_t blockSize = 1024;
  const size_t numBlocks = samples.size() / blockSize;
  std::vector<std::vector<double>> buffer;

  for (size_t i = 0; i < numBlocks; ++i) {
    std::vector<double> block;
    block.reserve(blockSize);
    for (size_t j = 0; j < blockSize; ++j) {
      const int16_t sample = samples[(i * blockSize) + j];
      block.push_back(static_cast<double>(sample) / absoluteValue);
    }
    buffer.push_back(std::move(block));
  }
  return buffer;
}

// calculate discrete fourier transform on a single sample
std::vector<std::complex<double>> discreteFourierTransform(
    const std::vector<double>& sample) {
  std::vector<std::complex<double>> dftResult;
  dftResult.resize(sample.size());

  for (size_t i = 0; i < sample.size(); i++) {
    dftResult[i] = 0;
    for (size_t j = 0; j < sample.size(); j++) {
      // for my own wellbeing:
      // the complex is sample[n] * cos(angle), sample[n] * sin(angle)
      const double angle =
          (2 * M_PI) * (double)i * (double)j / (double)sample.size();
      const std::complex<double> contribution(sample[j] * cos(angle),
                                              -sample[j] * sin(angle));
      dftResult[i] += contribution;
    }
  }
  return dftResult;
}

// radix-2 cooley tukey fast fourier transform using 1024 sample
std::vector<std::complex<double>> fastFourierTransform(
    const std::vector<double>& sample) {
  size_t sampleSize = sample.size();
  size_t sampleSizeHalf = sampleSize / 2;
  if (sampleSize <= 1) {
    return {std::complex<double>(sample[0], 0.0)};
  }

  std::vector<double> odd;
  std::vector<double> even;

  for (size_t i = 0; i < sampleSize; ++i) {
    if ((i % 2) == 0) {
      even.push_back(sample[i]);
    } else {
      odd.push_back((sample[i]));
    }
  }

  auto evenVar = fastFourierTransform(even);
  auto oddVar = fastFourierTransform(odd);

  std::vector<std::complex<double>> twiddle;

  // twiddle[i] = cos(-2πi / N) + i·sin(-2πi / N), where N = sampleSizeHalf
  // This is equivalent to: twiddle[i] = exp(-2πi * i / N)
  for (size_t i = 0; i < sampleSizeHalf; ++i) {
    const std::complex<double> contribution(
        std::cos(-2 * M_PI * (double)i / (double)sampleSizeHalf),
        std::sin(-2 * M_PI * (double)i / (double)sampleSizeHalf));

    twiddle.push_back(contribution);
  }

  std::vector<std::complex<double>> result;
  for (size_t i = 0; i < sampleSizeHalf; ++i) {
    result[i] = evenVar[i] + twiddle[i] * oddVar[i];
    result[i + sampleSizeHalf] = evenVar[i] - twiddle[i] * oddVar[i];
  }
  return result;
}

// calculate magnititude
std::vector<double> dftToMagnitude(
    const std::vector<std::complex<double>>& dftResult) {
  if (dftResult.empty()) {
    return {};
  }
  // magntitude = sqrt(real, pow2 + imaginary number, pow 2)
  std::vector<double> magnitude;
  magnitude.reserve(dftResult.size());

  for (const auto& val : dftResult) {
    magnitude.push_back(std::abs(val));
  }
  return magnitude;
}