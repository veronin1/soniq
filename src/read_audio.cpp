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


    for (size_t i = 0; i < expectedRiff.size(); ++i) {
        if (static_cast<char>(fileHeaderRiff[i]) != expectedRiff[i]) {
            throw std::runtime_error("Input file is not a valid WAV file.");
        }
    }

    std::array<char, wavHeaderSize> wavHeader{};
}
