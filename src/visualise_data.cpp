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
  for (size_t i = 0; i < magnitudes.size(); ++i) {
    scaled = (magnitudes[i] / largestMagnitude) * maxHeight;
    scaledValues.push_back(scaled);
  }
}