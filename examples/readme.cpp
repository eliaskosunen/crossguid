// Copyright (c) 2014 Graeme Hill (http://graemehill.ca)
// Copyright (c) 2018 Elias Kosunen (https://eliaskosunen.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// This file is a part of crossguid:
//   https://github.com/eliaskosunen/crossguid

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
