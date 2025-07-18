#!/bin/sh

rm -rf build/
mkdir -p build
cd build || exit 1

cmake ..
cmake --build .

