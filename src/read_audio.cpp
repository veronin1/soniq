#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>

constexpr std::array<char, 4> expectedRiff{'R', 'I', 'F', 'F'};
constexpr size_t wavHeaderSize = 44;

void read_audio(const std::string& filename) {
  std::ifstream wavFile(filename, std::ios::binary);
  if (!wavFile) {
    throw std::ios_base::failure("Failed to open file: " + filename);
  }

  // Read first 4 bytes and verify RIFF header
  std::array<std::byte, expectedRiff.size()> fileHeaderRiff{};
  std::array<char, expectedRiff.size()> riffBuffer{};
  wavFile.read(riffBuffer.data(), expectedRiff.size());
  std::memcpy(fileHeaderRiff.data(), riffBuffer.data(), expectedRiff.size());

  if (!std::equal(expectedRiff.begin(), expectedRiff.end(),
                  riffBuffer.begin())) {
    throw std::runtime_error("File is not a valid WAV file (missing RIFF)");
  }

  // Read first 44 bytes for WAV header
  std::array<char, wavHeaderSize> wavHeader{};
  wavFile.read(wavHeader.data(), wavHeaderSize);
}
