#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>

constexpr std::array<char, 4> riff{'R', 'I', 'F', 'F'};

void read_audio(const std::string& filename) {

    // open file in binary mode
    std::ifstream wavFile(filename, std::ios::binary);
    if (!wavFile) {
        throw std::ios_base::failure("Failed to open file: " + filename);
    }

    std::array<std::byte, 4> riffArray{};

    std::array<char, riffArray.size()> temp_buffer{};
    wavFile.read(temp_buffer.data(), riffArray.size());
    std::memcpy(riffArray.data(), temp_buffer.data(), riffArray.size());


    for (size_t i = 0; i < riffArray.size(); ++i) {
        if (static_cast<char>(riffArray[i]) != riff[i]) {
            throw std::runtime_error("Input file is not a valid WAV file.");
        }
    }
}
