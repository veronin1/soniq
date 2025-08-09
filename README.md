# soniq

soniq is a C++ program that plays a user-selected audio file with real-time sound output and displays an audio visualiser using either the DFT (Discrete Fourier Transform) algorithm or the radix-2 Cooley-Tukey Fast Fourier Transform (FFT).

## Demo

[![Watch the demo video](https://img.youtube.com/vi/L6JaSroQlTQ/0.jpg)](https://youtu.be/L6JaSroQlTQ)

## Requirements

-   CMake >= 3.10
-   C++17-compatible compiler (e.g., clang++ or g++)
-   raylib

## Build Instructions

```sh
mkdir build
cd build
cmake ..
make
```

## Run

```sh
./bin/soniq
```
