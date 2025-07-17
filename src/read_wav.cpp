#include "read_wav.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

WavFile read_wav(const std::string& filename) {
  std::ifstream wavFile(filename, std::ios::binary);
  if (!wavFile) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  // read first 12 bytes for RIFF, filesize, WAVE
  RiffHeader riffHeader{};
  std::vector<char> riffHeaderBuffer(sizeof(riffHeader));
  wavFile.read(riffHeaderBuffer.data(), sizeof(riffHeader));
  if (!wavFile) {
    throw std::runtime_error("Failed to read WAV header");
  }
  std::memcpy(&riffHeader, riffHeaderBuffer.data(), sizeof(riffHeader));

  if (std::memcmp(riffHeader.chunkID.data(), "RIFF", 4) != 0 ||
      std::memcmp(riffHeader.format.data(), "WAVE", 4) != 0) {
    throw std::runtime_error("Invalid WAV file format");
  }

  // loop to read next chunk ID, chunk size, if chunk = 'fmt ', if chunk ==
  // 'data', load samples`

  ChunkHeader chunkHeader{};
  FmtChunk fmtChunk{};

  while (true) {
    std::vector<char> genericChunkBuffer(sizeof(chunkHeader));
    wavFile.read(genericChunkBuffer.data(), sizeof(chunkHeader));
    if (!wavFile) {
      throw std::runtime_error("Failed to read WAV header");
    }
    std::memcpy(&chunkHeader, genericChunkBuffer.data(), sizeof(chunkHeader));

    if (std::memcmp(chunkHeader.chunkID.data(), "fmt ", 4) == 0) {
      std::vector<char> fmtBuffer(chunkHeader.chunkSize);
      wavFile.read(fmtBuffer.data(), sizeof(ChunkHeader));
      if (!wavFile) {
        throw std::runtime_error("Failed to read WAV header");
      }
      std::memcpy(&fmtChunk, fmtBuffer.data(),
                  std::min(static_cast<uint32_t>(sizeof(FmtChunk)),
                           chunkHeader.chunkSize));
    }

    /*
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
    */
  }

  // Print important header data
  void printHeader(WavHeader & header) {
    std::cout << "========== WAV FILE INFO ==========\n";
    std::cout << "Chunk ID: "
              << std::string(header.riff.begin(), header.riff.end()) << "\n";
    std::cout << "Chunk Size: " << header.chunkSize << "\n";
    std::cout << "Format: "
              << std::string(header.wave.begin(), header.wave.end()) << "\n";
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