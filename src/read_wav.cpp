#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "readwav.hpp"

WavFile read_wav(const std::string& filename) {
  std::ifstream wavFile(filename, std::ios::binary);
  if (!wavFile) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  // read entire wav header into struct
  WavHeader header{};
  std::vector<char> headerBuffer(sizeof(WavHeader));
  wavFile.read(headerBuffer.data(), sizeof(header));
  if (!wavFile) {
    throw std::runtime_error("Failed to read WAV header");
  }
  std::memcpy(&header, headerBuffer.data(), sizeof(WavHeader));

  if (std::memcmp(header.riff.data(), "RIFF", 4) != 0 ||
      std::memcmp(header.wave.data(), "WAVE", 4) != 0 ||
      std::memcmp(header.fmt.data(), "fmt ", 4) != 0 ||
      std::memcmp(header.data.data(), "data", 4) != 0) {
    throw std::runtime_error("Invalid WAV file format");
  }
  if (header.bitsPerSample != bits16perSample) {
    throw std::runtime_error("Only 16-bit samples are supported");
  }

  // read samples into soundData vector
  std::vector<std::byte> soundData(header.dataSize);
  std::vector<char> soundDataBuffer(header.dataSize);
  wavFile.read(soundDataBuffer.data(), header.dataSize);
  if (!wavFile) {
    throw std::runtime_error("Read error");
  }
  std::memcpy(soundData.data(), soundDataBuffer.data(), header.dataSize);

  printHeader(header);

  return WavFile{header, std::move(soundData)};
}

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

std::vector<std::complex<double>> discreteFourierTransform(
    const std::vector<double>& sample) {
  const size_t frequencyBin = 5;

  std::vector<std::complex<double>> dftResult;
  dftResult.resize(sample.size());

  for (int i = 0; i < sample.size(); i++) {
    dftResult[i] = 0;
    for (int j = 0; j < sample.size(); j++) {
      // for my own wellbeing:
      // the complex is sample[n] * cos(angle), sample[n] * sin(angle)
      double angle = (2 * M_PI) * i * j / (double)sample.size();
      std::complex<double> contribution(sample[j] * cos(angle),
                                        -sample[j] * sin(angle));
      dftResult[i] += contribution;
    }
  }
  return dftResult;
}

// calculate magnititude
double dftToMagntide(const std::vector<std::complex<double>>& dftResult) {
  // magntitude = sqrt(real, pow2 + imaginary number, pow 2)
  double magnitude = 0.0;

  for (const auto& complexNumber : dftResult) {
    magnitude += std::sqrt(std::pow(complexNumber.real(), 2) +
                           std::pow(complexNumber.imag(), 2));
  }

  return magnitude;
}

/*
std::vector<std::complex<double>> fastFourierTransform(
    std::vector<double>& sample) {
  const double DEG_TO_RAD = M_PI / 180.0;
  const int fullCircle = 360;
  const int numToGo = 10;
  for (int i = 0; i < fullCircle; i += numToGo) {
    std::cout << "Angle " << i << ": sin = " << sin(i * DEG_TO_RAD)
              << ", cos = " << cos(i * DEG_TO_RAD) << "\n";
  }

}
  */

int main() {
  // Read WAV file
  WavFile wav = read_wav("../sound/file_example_WAV_10MG.wav");

  // Convert raw bytes to 16-bit samples
  std::vector<int16_t> samples = convertBytes(wav.header, wav.soundData);

  // Split samples into blocks of 1024 samples (or smaller depending on file)
  std::vector<std::vector<double>> sampleBlocks = sampleToBlock(samples);

  if (sampleBlocks.empty()) {
    std::cerr << "No sample blocks found!\n";
    return 1;
  }

  // Run DFT on the first block
  std::vector<std::complex<double>> dftResult =
      discreteFourierTransform(sampleBlocks[0]);

  return 0;
}

// Print important header data
void printHeader(WavHeader& header) {
  std::cout << "========== WAV FILE INFO ==========\n";
  std::cout << "Chunk ID: "
            << std::string(header.riff.begin(), header.riff.end()) << "\n";
  std::cout << "Chunk Size: " << header.chunkSize << "\n";
  std::cout << "Format: " << std::string(header.wave.begin(), header.wave.end())
            << "\n";
  std::cout << "Subchunk1 ID: "
            << std::string(header.fmt.begin(), header.fmt.end()) << "\n";
  std::cout << "Subchunk1 Size: " << header.subchunk1Size << "\n";

  // Audio Format
  std::cout << "Audio Format: " << header.audioFormat;
  if (header.audioFormat == 1) {
    std::cout << " (PCM)";
  } else if (header.audioFormat == 3) {
    std::cout << " (IEEE Float)";
  } else {
    std::cout << " (Unknown)";
  }
  std::cout << "\n";

  // Channels
  std::cout << "Channels: " << header.numChannels;
  if (header.numChannels == 1) {
    std::cout << " (Mono)";
  } else if (header.numChannels == 2) {
    std::cout << " (Stereo)";
  } else {
    std::cout << "";
  }
  std::cout << "\n";

  std::cout << "Sample Rate: " << header.sampleRate << " Hz\n";
  std::cout << "Byte Rate: " << header.byteRate << "\n";
  std::cout << "Block Align: " << header.blockAlign << "\n";
  std::cout << "Bits per Sample: " << header.bitsPerSample << "\n";
  std::cout << "Subchunk2 ID: "
            << std::string(header.data.begin(), header.data.end()) << "\n";
  std::cout << "Subchunk2 Size (data): " << header.dataSize << " bytes\n";
  std::cout << "===================================\n";
}