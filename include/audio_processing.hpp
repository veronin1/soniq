#ifndef AUDIO_PROCESSING_HPP
#define AUDIO_PROCESSING_HPP

#include <complex>
#include <vector>

#include "read_wav.hpp"

std::vector<int16_t> convertBytes(const WavFile& wav);
std::vector<std::vector<double>> sampleToBlock(std::vector<int16_t>& samples);
std::vector<std::complex<double>> discreteFourierTransform(
    const std::vector<double>& sample);
std::vector<std::complex<double>> fastFourierTransform(
    const std::vector<double>& sample);
std::vector<double> computeMagnitude(
    const std::vector<std::complex<double>>& fourierResult);

#endif