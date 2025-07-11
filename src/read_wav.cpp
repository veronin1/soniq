#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

constexpr std::array<char, 4> expectedRiff{'R', 'I', 'F', 'F'};

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
