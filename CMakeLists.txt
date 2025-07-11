cmake_minimum_required(VERSION 3.10)
project(soniq)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

add_executable(soniq
    src/main.cpp
    src/read_audio.cpp
)

target_compile_options(soniq PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wformat=2
    -Wconversion
    -Wnull-dereference
    -Wdouble-promotion
    -Werror
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fsanitize=address
)

target_link_libraries(soniq PRIVATE
    -fsanitize=address
)

set_target_properties(soniq PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
