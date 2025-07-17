#ifndef READ_WAV_HPP
#define READ_WAV_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

const int bits16perSample = 16;

#pragma pack(push, 1)
struct RiffHeader {
  std::array<char, 4> chunkID;  // "RIFF"
  uint32_t chunkSize;           // File size minus 8 bytes
  std::array<char, 4> format;   // "WAVE"
};

// Generic chunk header (next 8 bytes)
struct ChunkHeader {
  std::array<char, 4> chunkID;  // chunk identifier (e.g. "fmt ", "data")
  uint32_t chunkSize;           // size of chunk data
};

// Format chunk ("fmt ") structure
struct FmtChunk {
  uint16_t audioFormat;    // Audio format (1 = PCM)
  uint16_t numChannels;    // Number of channels
  uint32_t sampleRate;     // Sample rate
  uint32_t byteRate;       // Byte rate
  uint16_t blockAlign;     // Block align
  uint16_t bitsPerSample;  // Bits per sample
  // Note: Can have extra format bytes for non-PCM formats
};

#pragma pack(pop)

struct WavMetadata {
  RiffHeader riffHeader;
  FmtChunk fmtChunk;
};

struct WavFile {
  WavMetadata metadata;
  std::vector<std::byte> soundData;
};

WavFile read_wav(const std::string& filename);
void printHeader(const WavMetadata& metadata);

#endif