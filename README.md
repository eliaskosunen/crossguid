# CrossGuid

[![Build Status (Linux/macOS)](https://travis-ci.com/eliaskosunen/crossguid.svg?branch=master)](https://travis-ci.com/eliaskosunen/crossguid)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/fntdbjhb2amffo69/branch/master?svg=true)](https://ci.appveyor.com/project/eliaskosunen/crossguid/branch/master)
[![License](https://img.shields.io/github/license/eliaskosunen/crossguid.svg)](https://github.com/eliaskosunen/crossguid/blob/master/LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17%2F20-blue.svg)](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17%2F20-blue.svg)

```cpp
#include <crossguid/guid.hpp>
#include <iostream>
#include <unordered_map>

int main()
{
    // Creating a new GUID
    auto g = xg::make_guid();
    // guid is DefaultConstructible
    xg::guid empty{};
    // and can also be created from a string (const char*)
    xg::guid from_string{"c405c66c-ccbb-4ffd-9b62-c286c0fd7a3b"};
    // or an array of bytes (unsigned char)
    std::array<unsigned char, 16> bytes_std_array{{0}};
    xg::guid from_std_array{std::move(bytes_std_array)};
    unsigned char bytes_c_array[16] = {0};
    xg::guid from_c_array{bytes_c_array};

    // guid is convertible to a bool
    if (g) {
        std::cout << "This GUID is fine!\n";
    }
    if (!empty) {
        std::cout << "This GUID is _not_ fine!\n";
    }

    // guids are EqualityComparable
    if (g != from_string) {
        std::cout << "Different GUIDs are not equal\n";
    }

    // guids can be converted to a std::string
    auto guid_str = g.str();
    // or be streamed to a std::ostream
    std::cout << "Here's my GUID: " << g << '\n';

    // accessing raw bytes
    const std::array<unsigned char, 16>& bytes = g.bytes();

    // guid specializes std::hash and std::less,
    // so they can be used as keys in std::map, std::set etc.
    std::unordered_map<xg::guid, int> hashmap{};
}
```

This repository is a source-incompatible fork of https://github.com/graeme-hill/crossguid,
a library by Graeme Hill.

CrossGuid is a minimal, cross platform, C++ GUID library.
It uses the best native GUID/UUID generator on the given platform.

## Changes to the original repository

 * Dropped Android support
 * Supports C++11 and 14 (doesn't depend on `std::string_view`)
 * Compiles without warnings, even on the most aggressive levels (see `CMakeLists.txt`)
 * Names are in `snake_case`, to be more in line with the C++ standard library
 * Doesn't auto-convert to `std::string` to prevent surprises
 * `isValid` member function replaced with `operator bool`
 * `newGuid` renamed to `make_guid` for better consistency with stdlib and to remove connotation of allocation
 * `constexpr` enabled and `noexcept` annotated, in line with the Lakos rule
 * Does the Right Thing (tm) by not defining `operator<` and specializing `std::less` instead
 * Faster compile times (doesn't include redundant headers)

## Dependencies

CrossGuid depends on the standard guid generation facilities on your platform,
which may need to be installed separately. See below for instructions for your specific platform.

### Linux

On Linux, CrossGuid uses `libuuid`. The library may already be installed on your system, but the header files
may be missing. For example, on Ubuntu, the package `uuid-dev` may need to be installed.

### Windows

On Windows, CrossGuid uses the WinAPI function `CoCreateGuid`, which is avaliable on all Windows systems from 2000 onwards.

### macOS/iOS

On Apple systems, CrossGuid uses `CFUUIDCreate` from `CoreFoundation`.

## Installation

CrossGuid can easily be integrated into your project if you use CMake.
Just clone it in your project, and use `add_subdirectory`.
The target `crossguid::crossguid` becomes available, which can be then linked against.

If you don't use CMake in your project, or for some reason wish to build CrossGuid separately,
it can be done with the standard CMake procedure:

```sh
mkdir build
cd build
cmake ..
make -j && make install
```

### Tests

Tests will only be built by default if CrossGuid is compiled as a standalone project,
or if `CROSSGUID_TESTS` is set to `ON` in CMake.

```sh
cmake -DCROSSGUID_TESTS=ON ..
```

The tests can then be run with `CTest`.

```sh
ctest
```

## API Documentation

Documentation for the latest commit to `master` is hosted [online](http://docs.eliaskosunen.com/crossguid/doc_guid.html).

API documentation can be generated with [standardese](https://github.com/foonathan/standardese).

```sh
cd build
# Generate compile_commands.json
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cd ../doc
./generate-doc.sh (path-to-standardese-tool-binary)
```

## License

The MIT License (MIT)

Copyright (c) 2014 Graeme Hill (http://graemehill.ca)  
Copyright (c) 2019 Elias Kosunen (https://eliaskosunen.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
