#include "read_wav.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <ios>
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

  ChunkHeader chunkHeader{};
  FmtChunk fmtChunk{};
  WavFile wav{};
  while (true) {
    std::vector<char> genericChunkBuffer(sizeof(chunkHeader));
    wavFile.read(genericChunkBuffer.data(), sizeof(chunkHeader));
    if (!wavFile) {
      break;
    }
    std::memcpy(&chunkHeader, genericChunkBuffer.data(), sizeof(chunkHeader));

    if (std::memcmp(chunkHeader.chunkID.data(), "fmt ", 4) == 0) {
      std::vector<char> fmtBuffer(chunkHeader.chunkSize);
      wavFile.read(fmtBuffer.data(), chunkHeader.chunkSize);
      if (!wavFile) {
        throw std::runtime_error("Failed to read FMT data");
      }
      std::memcpy(&fmtChunk, fmtBuffer.data(),
                  std::min(static_cast<uint32_t>(sizeof(FmtChunk)),
                           chunkHeader.chunkSize));
      wav.metadata.fmtChunk = fmtChunk;
    } else if (std::memcmp(chunkHeader.chunkID.data(), "data", 4) == 0) {
      wav.soundData.resize(chunkHeader.chunkSize);
      std::vector<char> audioBuffer(chunkHeader.chunkSize);
      wavFile.read(audioBuffer.data(), chunkHeader.chunkSize);
      if (!wavFile) {
        throw std::runtime_error("Failed to read WAV data");
      }
      std::memcpy(wav.soundData.data(), audioBuffer.data(),
                  chunkHeader.chunkSize);
      break;
    } else {
      wavFile.seekg(chunkHeader.chunkSize, std::ios_base::cur);
      if (!wavFile) {
        throw std::runtime_error("Failed to read WAV header");
      }
    }
  }

  printHeader(wav.metadata);

  return wav;
}

// Print important header data
void printHeader(const WavMetadata& metadata) {
  const auto& riff = metadata.riffHeader;
  const auto& fmt = metadata.fmtChunk;

  std::cout << "========== WAV FILE INFO ==========\n";

  std::cout << "Chunk ID: "
            << std::string(riff.chunkID.data(), riff.chunkID.size()) << "\n";
  std::cout << "Chunk Size: " << riff.chunkSize << "\n";
  std::cout << "Format: " << std::string(riff.format.data(), riff.format.size())
            << "\n";

  // For "fmt " chunk, the chunkID isn't stored in FmtChunk, so just hardcode
  // it:
  std::cout << "Subchunk1 ID: fmt \n";

  std::cout << "Audio Format: " << fmt.audioFormat;
  if (fmt.audioFormat == 1) {
    std::cout << " (PCM)";
  } else if (fmt.audioFormat == 3) {
    std::cout << " (IEEE Float)";
  } else {
    std::cout << " (Unknown)";
  }
  std::cout << "\n";

  std::cout << "Channels: " << fmt.numChannels;
  if (fmt.numChannels == 1) {
    std::cout << " (Mono)";
  } else if (fmt.numChannels == 2) {
    std::cout << " (Stereo)";
  }
  std::cout << "\n";

  std::cout << "Sample Rate: " << fmt.sampleRate << " Hz\n";
  std::cout << "Byte Rate: " << fmt.byteRate << "\n";
  std::cout << "Block Align: " << fmt.blockAlign << "\n";
  std::cout << "Bits per Sample: " << fmt.bitsPerSample << "\n";

  std::cout << "===================================\n";
}