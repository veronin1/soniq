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
void waveformVisualiser(const std::vector<double>& magnitudes, int windowWidth,
                        int windowHeight) {
  if (magnitudes.empty()) {
    return;
  }

  // set maxHeight as 80% of total window Height
  const double maxHeight = windowHeight * 0.8;

  // find largest magnitude
  double largestMagnitude =
      *std::max_element(magnitudes.begin(), magnitudes.end());
  if (largestMagnitude == 0) {
    largestMagnitude = 1;
  }

  // smooth and scale the magnitudes
  std::vector<double> scaledValues;
  std::deque<double> previousValues;
  const size_t smoothingWindow = 5;
  for (const double magnitude : magnitudes) {
    previousValues.push_back(magnitude);
    if (previousValues.size() > smoothingWindow) {
      previousValues.pop_front();
    }

    const double average =
        std::accumulate(previousValues.begin(), previousValues.end(), 0.0) /
        (double)previousValues.size();
    const double scaled = (average / largestMagnitude) * maxHeight;

    scaledValues.push_back(scaled);
  }

  // visualise bars
  const int initialX = 50;
  const int barWidth = 15;
  const int PosY = windowHeight - 50;
  const int amountToIncrease = 10;
  int posX = initialX;

  const int numberOfBars =
      (windowWidth - 2 * initialX) / (barWidth + amountToIncrease);

  std::vector<double> numOfBars(static_cast<size_t>(numberOfBars));
  for (size_t i = 0; i < numOfBars.size(); ++i) {
    numOfBars[i] = scaledValues[i];
  }

  for (const double& val : numOfBars) {
    DrawRectangle(posX, PosY - int(val), barWidth, int(val), WHITE);
    posX += barWidth + amountToIncrease;
  }
}