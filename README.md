# soniq

soniq is a C++ program that plays a user-selected audio file with real-time sound output and displays an audio visualiser using either the DFT (Discrete Fourier Transform) algorithm or the radix-2 Cooley-Tukey Fast Fourier Transform (FFT).

## Demo (so far)

<<<<<<< HEAD
<img src="example/first_working_example.gif" width="500" />
=======
[![Watch demo video](https://img.youtube.com/vi/L6JaSroQlTQ/0.jpg)](https://youtu.be/L6JaSroQlTQ)
>>>>>>> 14e2643 (Added youtube example to embed video content)

## Requirements

-   CMake >= 3.10
-   C++17-compatible compiler (e.g. `clang++` or `g++`)
-   (Optional) `clang-tidy` for static analysis

## Build Instructions

```sh
mkdir build
cd build
cmake ..
make
```

The final executable will be located at:

```sh
./bin/soniq
```

## Run File

```sh
./bin/soniq
```
