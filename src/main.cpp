#include <raylib.h>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>

#include "audio_processing.hpp"
#include "read_wav.hpp"
#include "visualise_data.hpp"

const size_t width = 1280;
const size_t height = 720;
const char* const title = "soniq";
const int targetFps = 60;
const size_t blockSize = 1024;

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

    if (!IsAudioDeviceReady()) {
      std::cerr << "Audio device not ready\n";
      return 1;
    }  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const Music music = LoadMusicStream(argv[1]);

    if (music.ctxData == nullptr) {
      std::cerr << "Music failed to load\n";
      return 1;
    }
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(targetFps);
    auto samples = convertBytes(wav);
    PlayMusicStream(music);
    const size_t totalBlocks = (samples.size() / blockSize - 1) / blockSize;

    size_t currentBlock = 0;
    float timePlayed = 0.0F;
    std::vector<std::optional<std::vector<float>>> cachedMagnitudes(
        totalBlocks);

    while (!WindowShouldClose() && currentBlock < cachedMagnitudes.size()) {
      UpdateMusicStream(music);
      auto block = getBlock(samples, currentBlock);

      timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
      timePlayed = std::min(timePlayed, 1.0F);
      const float currentIndex = timePlayed * float(cachedMagnitudes.size());
      BeginDrawing();
      ClearBackground(BLACK);

      if (!cachedMagnitudes[currentBlock].has_value()) {
        cachedMagnitudes[currentBlock] =
            computeMagnitude(fastFourierTransform(block, 0, 1));
      }

      waveformVisualiser(cachedMagnitudes[currentBlock].value(),
                         GetScreenWidth(), GetScreenHeight());

      EndDrawing();
      currentBlock = static_cast<size_t>(currentIndex);
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