#include "visualise_data.hpp"

#include <raylib.h>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

void printBars(const std::vector<double>& magnitudes, int windowHeight) {
  if (magnitudes.empty()) {
    return;
  }

  const double maxHeight = windowHeight * 0.8;

  double largestMagnitude = 0;

  for (const double integer : magnitudes) {
    largestMagnitude = std::max(largestMagnitude, integer);
  }

  std::vector<double> scaledValues;
  double scaled = 0.0;

  for (const double mag : magnitudes) {
    scaled = (mag / largestMagnitude) * maxHeight;
    scaledValues.push_back(scaled);
  }

  for (const double& current : scaledValues) {
    const int barLength = static_cast<int>(current);

    for (int j = 0; j < barLength; ++j) {
      std::cout << '#';
    }
    std::cout << '\n';
  }
}

// use raylib to visualise the data
void waveformVisualiser(const std::vector<float>& magnitudes, int windowWidth,
                        int windowHeight) {
  if (magnitudes.empty()) {
    return;
  }

  // set maxHeight as 80% of total window Height
  const double maxHeight = windowHeight * 0.8;

  // find largest magnitude
  float largestMagnitude =
      *std::max_element(magnitudes.begin(), magnitudes.end());
  if (largestMagnitude == 0) {
    largestMagnitude = 1.0F;
  }

  // smooth and scale the magnitudes
  std::vector<float> scaledValues;
  std::deque<float> previousValues;
  const size_t smoothingWindow = 2;
  for (const float magnitude : magnitudes) {
    previousValues.push_back(magnitude);
    if (previousValues.size() > smoothingWindow) {
      previousValues.pop_front();
    }

    const float average =
        std::accumulate(previousValues.begin(), previousValues.end(), 0.0F) /
        static_cast<float>(previousValues.size());
    const float scaled =
        ((average / largestMagnitude) * static_cast<float>(maxHeight));

    scaledValues.push_back(scaled);
  }

  // visualise bars
  const int initialX = 50;
  const int barWidth = 15;
  const int PosY = windowHeight - 50;
  const int amountToIncrease = 10;
  int posX = initialX;

  const int numberOfBars =
      std::min((windowWidth - 2 * initialX) / (barWidth + amountToIncrease),
               static_cast<int>(scaledValues.size()));

  std::vector<float> numOfBars(static_cast<size_t>(numberOfBars));
  for (size_t i = 0; i < numOfBars.size(); ++i) {
    numOfBars[i] = scaledValues[i];
  }

  for (const float& val : numOfBars) {
    DrawRectangle(posX, PosY - static_cast<int>(val), barWidth,
                  static_cast<int>(val), WHITE);
    posX += barWidth + amountToIncrease;
  }
}
