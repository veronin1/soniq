#!/bin/sh

rm -rf build/
mkdir -p build
cd build || exit 1

export CC=clang
export CXX=clang++

cmake ..
cmake --build .

