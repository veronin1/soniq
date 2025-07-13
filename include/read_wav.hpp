#ifndef READ_WAV_HPP
#define READ_WAV_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

const int bits16perSample = 16;

#pragma pack(push, 1)
struct WavHeader {
  std::array<char, 4> riff;  // "RIFF"
  uint32_t chunkSize;        // Size of the entire file in bytes
  std::array<char, 4> wave;  // "WAVE"
  std::array<char, 4> fmt;   // "fmt "
  uint32_t subchunk1Size;    // Size of the fmt chunk (16 for PCM)
  uint16_t audioFormat;      // Audio format (1 for PCM)
  uint16_t numChannels;      // Number of channels (1 for mono, 2 for stereo)
  uint32_t sampleRate;       // Sample rate (e.g., 44100 Hz)
  uint32_t
      byteRate;  // Byte rate (sampleRate * numChannels * bitsPerSample / 8)
  uint16_t blockAlign;       // Block align (numChannels * bitsPerSample / 8)
  uint16_t bitsPerSample;    // Bits per sample (e.g., 16)
  std::array<char, 4> data;  // "data"
  uint32_t dataSize;         // Size of the data chunk
};
#pragma pack(pop)

struct WavFile {
  WavHeader header;
  std::vector<std::byte> soundData;
};

WavFile read_wav(const std::string& filename);
void printHeader(WavHeader& header);

#endif