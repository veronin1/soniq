#!/bin/sh

mkdir -p build
cd build || exit 1

cmake ..
cmake --build .

../bin/soniq ../sound/file_example_WAV_10MG.wav
