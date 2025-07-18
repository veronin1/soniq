#include <raylib.h>

#include <algorithm>
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
    const WavFile wav = read_wav(argv[1]);
    InitAudioDevice();
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const Music music = LoadMusicStream(argv[1]);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(targetFps);
    auto samples = convertBytes(wav);
    auto blocks = sampleToBlock(samples);
    PlayMusicStream(music);

    size_t currentBlock = 0;
    float timePlayed = 0.0F;
    while (!WindowShouldClose() && currentBlock < blocks.size()) {
      UpdateMusicStream(music);

      timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
      timePlayed = std::min(timePlayed, 1.0F);
      const float currentIndex = timePlayed * float(blocks.size());
      auto fftResult = fastFourierTransform(blocks[currentBlock]);
      auto mags = computeMagnitude(fftResult);
      BeginDrawing();
      ClearBackground(BLACK);
      waveformVisualiser(mags, GetScreenWidth(), GetScreenHeight());
      EndDrawing();
      currentBlock = (size_t)currentIndex;
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}