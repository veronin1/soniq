#include "visualise_data.hpp"

#include <raylib.h>

#include <algorithm>
#include <iostream>
#include <vector>

const double maxHeight = 40;
const size_t width = 1280;
const size_t height = 720;
const char* const title = "soniq";

void printBars(const std::vector<double>& magnitudes) {
  double largestMagnitude = 0;

  for (double integer : magnitudes) {
    largestMagnitude = std::max(largestMagnitude, integer);
  }

  std::vector<double> scaledValues;
  double scaled = 0.0;

  for (double mag : magnitudes) {
    scaled = (mag / largestMagnitude) * maxHeight;
    scaledValues.push_back(scaled);
  }

  for (double& current : scaledValues) {
    int barLength = static_cast<int>(current);

    for (int j = 0; j < barLength; ++j) {
      std::cout << '#';
    }
    std::cout << '\n';
  }
}

// use raylib to visualise the data
void waveformVisualiser(const std::vector<double>& magnitudes) {
  double largestMagnitude = 0;

  for (double integer : magnitudes) {
    largestMagnitude = std::max(largestMagnitude, integer);
  }

  std::vector<double> scaledValues;
  double scaled = 0.0;

  for (double mag : magnitudes) {
    scaled = (mag / largestMagnitude) * maxHeight;
    scaledValues.push_back(scaled);
  }

  // raylib
  InitWindow(width, height, title);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    const int initialX = 50;
    const int barWidth = 40;
    const int PosY = height - 50;
    const int amountToIncrease = 10;
    int posX = initialX;
    for (const double& val : scaledValues) {
      DrawRectangle(posX, PosY - int(val), barWidth, int(val), WHITE);
      posX += barWidth + amountToIncrease;
    }

    EndDrawing();
  }

  CloseWindow();
}