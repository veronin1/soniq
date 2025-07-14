#include "visualise_data.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

const double maxHeight = 40;

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