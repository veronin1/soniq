#include "audio_processing.hpp"

#include <cmath>
#include <cstddef>

#include "read_wav.hpp"

constexpr float M_PIF = 3.1415927F;
const size_t blockSize = 1024;

// Convert bytes into 16 bit numbers
std::vector<int16_t> convertBytes(const WavFile& wav) {
  std::vector<int16_t> samples;
  if (wav.metadata.fmtChunk.bitsPerSample == bits16perSample) {
    const auto& soundData = wav.soundData;
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
std::vector<float> getBlock(std::vector<int16_t>& samples,
                            size_t currentIndex) {
  const float absoluteValue = 32768.0F;
  std::vector<float> block;
  block.reserve(blockSize);

  auto startingOffset = currentIndex * blockSize;

  if (startingOffset + blockSize > samples.size()) {
    return block;
  }

  for (size_t i = startingOffset; i < startingOffset + blockSize; ++i) {
    block.push_back(static_cast<float>(samples[i]) / absoluteValue);
  }

  return block;
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
std::vector<std::complex<float>> fastFourierTransform(
    const std::vector<float>& sample, size_t start, size_t step) {
  auto subproblem = sample.size() / step;
  if (subproblem == 1) {
    return {std::complex<float>(sample[start], 0.0F)};
  }

  auto evenVar = fastFourierTransform(sample, start, step * 2);
  auto oddVar = fastFourierTransform(sample, start + step, step * 2);

  std::vector<std::complex<float>> twiddle;

  // twiddle[i] = cos(-2πi / N) + i·sin(-2πi / N), where N = sampleSizeHalf
  // This is equivalent to: twiddle[i] = exp(-2πi * i / N)
  for (size_t i = 0; i < subproblem / 2; ++i) {
    const std::complex<float> contribution(
        std::cos(-2.0F * M_PIF * (float)i / (float)subproblem),
        std::sin(-2.0F * M_PIF * (float)i / (float)subproblem));

    twiddle.push_back(contribution);
  }

  std::vector<std::complex<float>> result(subproblem);
  for (size_t i = 0; i < subproblem / 2; ++i) {
    result[i] = evenVar[i] + twiddle[i] * oddVar[i];
    result[i + (subproblem / 2)] = evenVar[i] - twiddle[i] * oddVar[i];
  }
  return result;
}

// Compute magnitude spectrum from Fourier Transform (DFT or FFT) result
std::vector<float> computeMagnitude(
    const std::vector<std::complex<float>>& fourierResult) {
  if (fourierResult.empty()) {
    return {};
  }
  // magnitude = sqrt(real^2 + imag^2)
  std::vector<float> magnitude;
  magnitude.reserve(fourierResult.size());

  for (const auto& val : fourierResult) {
    magnitude.push_back(static_cast<float>(std::abs(val)));
  }
  return magnitude;
}