# SimpleStr

![C++](https://img.shields.io/badge/C%2B%2B-23-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

Ultra-fast, Pythonic string manipulation library for C++17+, header-only, with benchmarks demonstrating performance gains over Python and `std::string`.

TODO: add documentation, add tests and bench for replace

---

## Index

1. [Overview](#overview)  
2. [Design Goals](#design-goals)  
3. [Scope & Limitations](#scope--limitations)  
4. [Usage Examples](#usage-examples)  
5. [Integration & Build](#integration--build)  
6. [Benchmarking](#benchmarking)
7. [License](#license)  

---

## Overview

`SimpleStr` provides a **Python-inspired, modern C++ interface** for common string operations. It focuses on high-performance, clean syntax, and zero dependencies while preserving C++ type safety and modern standards.  

Key features:

- **Pythonic API**: familiar operations like `split`, `join`, `replace`, `lower`, `upper`, `strip`, and more.  
- **Header-only, zero dependencies**: drop-in integration with any C++17+ project.  
- **High-performance**: optimized for x86, x64, ARM, and cross-platform builds.  
- **Comprehensive string operations**: slicing, repeating, searching, and case conversion.  
- **Safe & deterministic**: operations return new strings; no undefined behavior.

---

## Design Goals

- Offer a **minimal, Python-like interface** for strings in C++.  
- Achieve **performance comparable or better than Python and `std::string`** for typical operations.  
- Keep the library **header-only** for easy inclusion in projects.  
- Support **cross-platform builds** (macOS, Linux, Windows) with architecture-aware optimizations where possible.  

---

## Scope & Limitations

**Out of scope**:

- Unicode-aware transformations (ASCII only for `lower`/`upper`).  
- Regex-based string operations.  
- Multithreading or concurrent modifications of `Str` instances.  
- Replacement for full-featured libraries like Boost.StringAlgo.  

---

## Lifetime, Ownership & Safety

- `Str` **owns its underlying string**.  
- All operations **return new `Str` objects**; no shared mutable state.  
- No memory leaks; standard C++ string lifetime rules apply.  
- Designed for **stack and heap safe operations**; avoids dangling references.  

---

## Usage Examples

### Include the library
```cpp
#include "SimpleStr.hpp"
using namespace SimpleStr;
```

### Constructing and converting
```cpp
Str s("Hello, World!");
std::string stdStr = s.str();
std::string_view view = s;
```

### Splitting
```cpp
std::vectorstd::string_view￼ parts = s.split(’,’);
std::vectorstd::string_view￼ words = s.split();
```

### Joining
```cpp
std::vector<Str> wordsVec = { "Hello", "SimpleStr" };
Str joined = s.join(wordsVec, " ");
```

### Replace
```cpp
Str replaced = s.replace("World", "C++");
```

### Lower / Upper
```cpp
Str lowerStr = s.lower();
Str upperStr = s.upper();
```

### Strip
```cpp
Str stripped = s.strip();
```

### Count / Contains
```cpp
int count = s.count("l");
bool hasHello = s.contains("Hello");
```

### Repeat
```
Str repeated = s * 3;
```

---

## Integration & Build

Being **header-only**, SimpleStr requires no separate build step.

**Steps to integrate**:

1. Copy `include/SimpleStr.hpp` into your project include path.  
2. Include it in your source files:
```cpp
#include "SimpleStr.hpp"
```
3. Compile with a C++17+ compiler:
```bash
g++ -std=c++23 main.cpp -o main
./main
```
**Optional CMake Integration**:
```cmake
cmake_minimum_required(VERSION 3.10)
project(DummyProject)

set(CMAKE_CXX_STANDARD 23)

include_directories(path/to/SimpleStr/include)

add_executable(dummy main.cpp)
```

---

## Benchmarking

Benchmarks demonstrate that SimpleStr is often faster than Python and comparable or faster than `std::string` for typical operations.

| Operation   | Mark | SStr (ms) | vs Python | vs std |
|------------:|:----:|-----------:|----------:|-------:|
| splitChar   | ✔    | 4.67       | -11.20   | -0.23 |
| splitWS     | ✔    | 7.62       | -13.43   | -0.86 |
| join        | ✔    | 3.11       | -26.40   | -1.57 |
| replace     | ✔    | 1.96       | -4.01    | -1.31 |
| lower       | ✔    | 0.56       | -2.32    | -2.65 |
| upper       | ✔    | 0.65       | -2.24    | -2.38 |
| strip       | ✔    | 0.15       | +0.00    | +0.00 |
| count       | ✔    | 0.67       | -2.50    | -0.00 |
| contains    | ✔    | 0.21       | -1.82    | +0.00 |
| repeat      | ✘    | 0.02       | +0.01    | -0.19 |

> **Notes:**  
> - Negative numbers indicate SimpleStr is faster than the comparison baseline (Python or `std::string`).  
> - “Mark” indicates whether SimpleStr is at least as fast as both Python and `std::string` (✔) or not (✘).  
> - Benchmarks were collected with `bench/benchmark_all.cpp`, median of multiple runs.  
> - Library includes architecture-aware optimizations and branchless ASCII case conversion.

---

## License

This project is licensed under the **MIT License**. See the LICENSE file for full terms.

Based on the design of Pythonic string operations and inspired by high-performance header-only C++ libraries.
