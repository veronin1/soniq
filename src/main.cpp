#include <iostream>

#include "readwav.hpp"

int main() {
  WavFile wav = read_wav("../sound/file_example_WAV_10MG.wav");

  std::vector<int16_t> samples = convertBytes(wav.header, wav.soundData);

  for (size_t i = 0; i < nums; ++i) {
    std::cout << "Sample[" << i << "]: " << samples[i] << '\n';
  }
  return 0;
}