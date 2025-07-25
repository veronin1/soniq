#ifndef VISUALISE_DATA_HPP
#define VISUALISE_DATA_HPP

#include <vector>

void printBars(const std::vector<double>& magnitudes);
void waveformVisualiser(const std::vector<float>& magnitudes, int windowWidth,
                        int windowHeight);
#endif
