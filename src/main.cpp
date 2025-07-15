#include <raylib.h>

#include <exception>
#include <iostream>

#include "audio_processing.hpp"
#include "read_wav.hpp"
#include "visualise_data.hpp"

const size_t width = 1280;
const size_t height = 720;
const char* const title = "soniq";
const int targetFps = 60;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::cerr << "Usage: " << argv[0] << " <wav_file_path>\n";
    return 1;
  }
  try {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    WavFile wav = read_wav(argv[1]);
    InitWindow(width, height, title);
    SetTargetFPS(targetFps);
    auto samples = convertBytes(wav.header, wav.soundData);
    auto blocks = sampleToBlock(samples);
    while (!WindowShouldClose()) {
      for (const auto& block : blocks) {
        auto fftResult = fastFourierTransform(block);
        auto mags = computeMagnitude(fftResult);
        BeginDrawing();
        ClearBackground(BLACK);
        waveformVisualiser(mags);
        EndDrawing();
      }
    }
    CloseWindow();

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}