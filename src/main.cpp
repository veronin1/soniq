#include <exception>
#include <iostream>

#include "audio_processing.hpp"
#include "read_wav.hpp"
#include "visualise_data.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::cerr << "Usage: " << argv[0] << " <wav_file_path>\n";
    return 1;
  }
  try {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    WavFile wav = read_wav(argv[1]);
    auto samples = convertBytes(wav.header, wav.soundData);
    auto blocks = sampleToBlock(samples);
    for (const auto& block : blocks) {
      auto fftResult = fastFourierTransform(block);
      auto mags = computeMagnitude(fftResult);
      waveformVisualiser(mags);
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}