#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>

constexpr std::array<char, 4> expectedRiff{'R', 'I', 'F', 'F'};
constexpr size_t wavHeaderSize = 44;

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
}
