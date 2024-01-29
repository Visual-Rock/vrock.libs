# vrock.lib's

[![CMake Tests](https://github.com/Visual-Rock/vrock.libs/actions/workflows/CMake-Tests.yml/badge.svg)](https://github.com/Visual-Rock/vrock.libs/actions/workflows/CMake-Tests.yml)

A collection of librarys for C++

## 🧐 Features

Here're some of the project's best features:

- utils:
    * Coroutine Helpers like await and when\_all
    * Coroutine classes like Task and Generator
    * Helper functions for futures and shared futures
    * a class for Lazy evaluation
- security:
    * facilities for AES encryption and decryption
    * facilities for SHA2 and md5 hashes
    * random number generation

## 🚀 Getting Started

### Prerequisites

You need to install the following software:

* cmake
* a c++ compiler (gcc, clang, MSVC)
* vcpkg

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/Visual-Rock/vrock.libs.git
   ```

2. Run Cmake
   ```sh
   cmake . -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake
   ```

   Add the following options to include parts of the collection
   (prefix all listed options with `-DVROCKLIBS_`):

   | Name     | Description                                   |
   |----------|-----------------------------------------------|
   | SECURITY | includes the security module                  |
   | PDF      | includes the pdf module                       |
   | DOCS     | builds the documentation for all modules      |
   | EXAMPLES | includes all examples of the selected modules |
   | TESTS    | builds the tests for all selected modules     |

## 💻 Built with

Technologies used in the project:

* vcpkg
* security:
    * cryptopp
* pdf:
    * icu
    * zlib

## 🛡️ License:

This project is licensed under the MIT License