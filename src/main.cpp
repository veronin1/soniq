#include <exception>
#include <iostream>

#include "audio_processing.hpp"
#include "read_wav.hpp"

constexpr size_t nums = 10;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::cerr << "Usage: " << argv[0] << " <wav_file_path>\n";
    return 1;
  }
  try {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    WavFile wav = read_wav(argv[1]);
    std::vector<int16_t> samples = convertBytes(wav.header, wav.soundData);
    for (size_t i = 0; i < nums; ++i) {
      std::cout << "Sample[" << i << "]: " << samples[i] << '\n';
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}