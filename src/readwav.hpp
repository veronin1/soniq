#ifndef readwav_hpp
#define readwav_hpp

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

constexpr size_t bits16perSample = 16;

// Struct for WavHeader
#pragma pack(push, 1)
struct WavHeader {
  std::array<char, 4> riff;  // "RIFF"
  uint32_t chunkSize;        // overall file size - 8 bytes
  std::array<char, 4> wave;  // "WAVE"
  std::array<char, 4> fmt;   // "fmt "
  uint32_t subchunk1Size;    // 16 for PCM
  uint16_t audioFormat;      // 1 = PCM, 3 = float
  uint16_t numChannels;      // 1 = mono, 2 = stereo
  uint32_t sampleRate;       // e.g. 44100 Hz
  uint32_t byteRate;         // == SampleRate * NumChannels * BitsPerSample/8
  uint16_t blockAlign;       // == NumChannels * BitsPerSample/8
  uint16_t bitsPerSample;    // e.g. 16
  std::array<char, 4> data;  // "data"
  uint32_t dataSize;         // size of the sound data in bytes
};
#pragma pack(pop)

struct WavFile {
  WavHeader header;
  std::vector<std::byte> soundData;
};

WavFile read_wav(const std::string& filename);
std::vector<int16_t> convertBytes(WavHeader header,
                                  std::vector<std::byte>& soundData);
void printHeader(WavHeader& header);
std::vector<std::vector<double>> sampleToBlock(std::vector<int16_t>& samples);

#endif