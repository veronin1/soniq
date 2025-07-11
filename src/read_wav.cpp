#include <array>
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

  if (std::memcmp(header.riff.data(), "RIFF", 4) != 0) {
    throw std::runtime_error("File is not valid WAV file (missing 'RIFF')");
  }
  if (std::memcmp(header.wave.data(), "WAVE", 4) != 0) {
    throw std::runtime_error("File is not valid WAV file (missing 'WAVE')");
  }
  if (std::memcmp(header.fmt.data(), "fmt ", 4) != 0) {
    throw std::runtime_error("File is not valid WAV file (missing 'fmt ')");
  }
  if (std::memcmp(header.data.data(), "data", 4) != 0) {
    throw std::runtime_error("File is not valid WAV file (missing 'data')");
  }

  // read samples into soundData vector
  std::vector<char> soundDataBuffer(header.dataSize);
  wavFile.read(soundDataBuffer.data(), header.dataSize);

  std::vector<std::byte> soundData(header.dataSize);
  soundData.reserve(header.dataSize);
  for (char sample : soundDataBuffer) {
    soundData.push_back(static_cast<std::byte>(sample));
  }

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
          static_cast<uint8_t>(byte2) << 8 | static_cast<uint8_t>(byte1));

      samples.push_back(sample);
    }
  }
  return samples;
}