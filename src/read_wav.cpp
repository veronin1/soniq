#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

constexpr std::array<char, 4> expectedRiff{'R', 'I', 'F', 'F'};
constexpr size_t wavHeaderSize = 44;
constexpr size_t dataSizeOffset = 40;

void read_wav(const std::string& filename) {
  std::ifstream wavFile(filename, std::ios::binary);
  if (!wavFile) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  std::array<char, wavHeaderSize> wavHeader{};
  wavFile.read(wavHeader.data(), wavHeaderSize);
  if (!std::equal(expectedRiff.begin(), expectedRiff.end(),
                  wavHeader.begin())) {
    throw std::runtime_error("File is not a valid WAV file (missing RIFF)");
  }

  // size of actual sound data is stored at offset 40 as a uint32 in wav header
  uint32_t dataSize = 0;
  std::memcpy(&dataSize, &wavHeader[dataSizeOffset], sizeof(dataSize));
  if (dataSize == 0) {
    throw std::runtime_error("WAV file contains no sound data");
  }

  // read samples into soundData vector
  std::vector<char> soundDataBuffer(dataSize);
  wavFile.read(soundDataBuffer.data(), dataSize);

  std::vector<std::byte> soundData(dataSize);
  soundData.reserve(dataSize);
  for (char sample : soundDataBuffer) {
    soundData.push_back(static_cast<std::byte>(sample));
  }
}
