cmake_minimum_required(VERSION 3.10)
project(SimpleStrTests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Library headers
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Catch2 (Homebrew)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/lib/cmake/Catch2")
find_package(Catch2 3 REQUIRED)

# ---------------- Tests ----------------
add_executable(tests tests/test_simplestr.cpp)
target_include_directories(tests PRIVATE ${INCLUDE_DIR})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# ---------------- Benchmark ----------------
add_executable(benchmark bench/benchmark_all.cpp)
target_include_directories(benchmark PRIVATE ${INCLUDE_DIR})
target_compile_options(benchmark PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native -flto -DNDEBUG>
)
target_link_libraries(benchmark PRIVATE Catch2::Catch2)