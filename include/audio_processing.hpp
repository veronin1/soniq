#ifndef AUDIO_PROCESSING_HPP
#define AUDIO_PROCESSING_HPP

#include <complex>
#include <vector>

#include "read_wav.hpp"

std::vector<int16_t> convertBytes(const WavFile& wav);
std::vector<std::vector<float>> sampleToBlock(std::vector<int16_t>& samples);
std::vector<std::complex<double>> discreteFourierTransform(
    const std::vector<double>& sample);
std::vector<std::complex<float>> fastFourierTransform(
    const std::vector<float>& sample);
std::vector<float> computeMagnitude(
    const std::vector<std::complex<float>>& fourierResult);

#endif