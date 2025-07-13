#ifndef AUDIO_PROCESSING_HPP
#define AUDIO_PROCESSING_HPP

#include <complex>
#include <vector>

#include "read_wav.hpp"

std::vector<int16_t> convertBytes(WavHeader header,
                                  std::vector<std::byte>& soundData);
std::vector<std::vector<double>> sampleToBlock(std::vector<int16_t>& samples);
std::vector<std::complex<double>> discreteFourierTransform(
    const std::vector<double>& sample);
std::vector<double> dftToMagnitude(
    const std::vector<std::complex<double>>& dftResult);

#endif