cmake_minimum_required(VERSION 3.10)
project(SimpleStrTests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Catch2 (Homebrew path on macOS)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/lib/cmake/Catch2")

# Find Catch2
find_package(Catch2 3 REQUIRED)

# Test executable
add_executable(tests
    tests/test_simplestr.cpp
)

# Link Catch2 main
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
