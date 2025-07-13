#include "visualise_data.hpp"

#include <cstddef>
#include <vector>

void printBars(const std::vector<double>& magnitudes) {
  double largestMagnitude = 0;

  for (double integer : magnitudes) {
    if (integer > largestMagnitude) {
      largestMagnitude = integer;
    }
  }
}