# vrock.lib's

[![CMake Tests](https://github.com/Visual-Rock/vrock.libs/actions/workflows/CMake-Tests.yml/badge.svg)](https://github.com/Visual-Rock/vrock.libs/actions/workflows/CMake-Tests.yml)

A collection of libraries for C++ projects.

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
- log:
    * a simple logging facility
    * customizable logging patterns
    * multithreading support

## 🚀 Getting Started

### Prerequisites

You need to install the following software:

* cmake
* a recent c++ compiler (gcc, clang, MSVC)
* vcpkg
* docs (optional)
    * Doxygen
    * [Sphinx](https://www.sphinx-doc.org/en/master/usage/installation.html) + sphinx-sitemap and sphinx-rtd-theme
    * [Breathe](https://breathe.readthedocs.io/en/latest/installation.html)

### How to build

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
   | LOG      | includes the log module                       |
   | PDF      | includes the pdf module                       |
   | DOCS     | builds the documentation for all modules      |
   | EXAMPLES | includes all examples of the selected modules |
   | TESTS    | builds the tests for all selected modules     |

3. Build
   ```sh
   cmake --build <build-dir>
   ```

4. Build the Docs
   ```sh
   cmake --build <build-dir> --target Docs
   ```

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