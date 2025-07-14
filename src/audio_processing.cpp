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

// calculate magnititude
std::vector<double> dftToMagnitude(
    const std::vector<std::complex<double>>& dftResult) {
  if (dftResult.empty()) {
    return {};
  }
  // magntitude = sqrt(real, pow2 + imaginary number, pow 2)
  std::vector<double> magnitude;
  magnitude.resize(dftResult.size());

  for (const auto& val : dftResult) {
    magnitude.push_back(
        std::sqrt(std::pow(val.real(), 2) + std::pow(val.imag(), 2)));
  }
  return magnitude;
}