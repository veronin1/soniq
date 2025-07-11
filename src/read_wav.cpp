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
  std::cout << "Audio Format: " << header.audioFormat
            << (header.audioFormat == 1   ? " (PCM)"
                : header.audioFormat == 3 ? " (IEEE Float)"
                                          : " (Unknown)")
            << "\n";
  std::cout << "Channels: " << header.numChannels
            << (header.numChannels == 1   ? " (Mono)"
                : header.numChannels == 2 ? " (Stereo)"
                                          : "")
            << "\n";
  std::cout << "Sample Rate: " << header.sampleRate << " Hz\n";
  std::cout << "Byte Rate: " << header.byteRate << "\n";
  std::cout << "Block Align: " << header.blockAlign << "\n";
  std::cout << "Bits per Sample: " << header.bitsPerSample << "\n";
  std::cout << "Subchunk2 ID: "
            << std::string(header.data.begin(), header.data.end()) << "\n";
  std::cout << "Subchunk2 Size (data): " << header.dataSize << " bytes\n";
  std::cout << "===================================\n";
}