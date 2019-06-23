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

#include "crossguid/guid.hpp"

#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

namespace xg {
    namespace detail {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        // converts a single hex char to a number (0 - 15)
        static unsigned char hex_digit_to_char(unsigned char ch)
        {
            // 0-9
            if (ch > 47 && ch < 58) {
                return ch - 48;
            }

            // a-f
            if (ch > 96 && ch < 103) {
                return ch - 87;
            }

            // A-F
            if (ch > 64 && ch < 71) {
                return ch - 55;
            }

            return 0;
        }

        static bool is_valid_hex_char(unsigned char ch)
        {
            return std::isxdigit(ch) != 0;
        }

        // converts the two hexadecimal characters to an unsigned char (a byte)
        static unsigned char hex_pair_to_char(unsigned char a, unsigned char b)
        {
            return hex_digit_to_char(a) * 16 + hex_digit_to_char(b);
        }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    }  // namespace detail

    guid::guid(const char* s) : _bytes{{0}}
    {
        char char1{}, char2{};
        std::size_t next_byte = 0;
        bool looking_for_first_char = true;

        const auto end = s + std::strlen(s);
        for (; s != end; ++s) {
            auto ch = *s;

            if (ch == '-') {
                continue;
            }

            if (next_byte >= 16 ||
                !detail::is_valid_hex_char(static_cast<unsigned char>(ch))) {
                // Invalid string, so bail
                zeroify();
                return;
            }

            if (looking_for_first_char) {
                char1 = ch;
                looking_for_first_char = false;
            }
            else {
                char2 = ch;
                auto byte =
                    detail::hex_pair_to_char(static_cast<unsigned char>(char1),
                                             static_cast<unsigned char>(char2));
                _bytes[next_byte++] = byte;
                looking_for_first_char = true;
            }
        }

        // if there were fewer than 16 bytes in the string then the guid is bad
        if (next_byte < 16) {
            zeroify();
            return;
        }
    }

    std::ostream& operator<<(std::ostream& s, const guid& guid)
    {
        auto write_byte = [](std::ostream& stream, const unsigned char* byte) {
            stream << std::setw(2) << static_cast<int>(*byte);
        };
        auto write_n_bytes = [&write_byte](std::ostream& stream,
                                           const unsigned char*& ptr,
                                           std::size_t n) {
            auto end = ptr + n;
            for (; ptr != end; ++ptr) {
                write_byte(stream, ptr);
            }
        };

        std::ios_base::fmtflags f(
            s.flags());  // politely don't leave the ostream in hex mode

        s << std::hex << std::setfill('0');

        auto data = guid.bytes().data();
        write_n_bytes(s, data, 4);
        s << '-';
        write_n_bytes(s, data, 2);
        s << '-';
        write_n_bytes(s, data, 2);
        s << '-';
        write_n_bytes(s, data, 2);
        s << '-';
        write_n_bytes(s, data, 6);

        s.flags(f);
        return s;
    }

    guid make_guid_from_bytes(unsigned char* p)
    {
        std::array<unsigned char, 16> data;
        std::copy(p, p + 16, data.data());
        return guid(data);
    }

// linux friendly implementation
// could work on other systems that have libuuid available
#ifdef GUID_LIBUUID
    guid make_guid()
    {
        std::array<unsigned char, 16> data;
        static_assert(std::is_same<unsigned char[16], uuid_t>::value,
                      "Wrong type!");
        uuid_generate(data.data());
        return guid{std::move(data)};
    }
#endif

// mac and ios version
#ifdef GUID_CFUUID
    guid make_guid()
    {
        auto id = CFUUIDCreate(NULL);
        auto bytes = CFUUIDGetUUIDBytes(id);
        CFRelease(id);

        std::array<unsigned char, 16> arr = {
            {bytes.byte0, bytes.byte1, bytes.byte2, bytes.byte3, bytes.byte4,
             bytes.byte5, bytes.byte6, bytes.byte7, bytes.byte8, bytes.byte9,
             bytes.byte10, bytes.byte11, bytes.byte12, bytes.byte13,
             bytes.byte14, bytes.byte15}};
        return guid{std::move(arr)};
    }
#endif

// windows version
#ifdef GUID_WINDOWS
    guid make_guid()
    {
        GUID id;
        CoCreateGuid(&id);

        std::array<unsigned char, 16> bytes = {
            static_cast<unsigned char>((id.Data1 >> 24) & 0xFF),
            static_cast<unsigned char>((id.Data1 >> 16) & 0xFF),
            static_cast<unsigned char>((id.Data1 >> 8) & 0xFF),
            static_cast<unsigned char>((id.Data1) & 0xff),

            static_cast<unsigned char>((id.Data2 >> 8) & 0xFF),
            static_cast<unsigned char>((id.Data2) & 0xff),

            static_cast<unsigned char>((id.Data3 >> 8) & 0xFF),
            static_cast<unsigned char>((id.Data3) & 0xFF),

            static_cast<unsigned char>(id.Data4[0]),
            static_cast<unsigned char>(id.Data4[1]),
            static_cast<unsigned char>(id.Data4[2]),
            static_cast<unsigned char>(id.Data4[3]),
            static_cast<unsigned char>(id.Data4[4]),
            static_cast<unsigned char>(id.Data4[5]),
            static_cast<unsigned char>(id.Data4[6]),
            static_cast<unsigned char>(id.Data4[7])};

        return guid{std::move(bytes)};
    }
#endif

    namespace detail {
        template <typename...>
        struct hash;

        template <typename T>
        struct hash<T> : public std::hash<T> {
            using std::hash<T>::hash;
        };

        template <typename T, typename... Rest>
        struct hash<T, Rest...> {
            inline std::size_t operator()(const T& v, const Rest&... rest)
            {
                std::size_t seed = hash<Rest...>{}(rest...);
                seed ^= hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                return seed;
            }
        };
    }  // namespace detail
}  // namespace xg

namespace std {
    std::size_t hash<xg::guid>::operator()(const xg::guid& guid) const
    {
        const uint64_t* p =
            reinterpret_cast<const uint64_t*>(guid.bytes().data());
        return xg::detail::hash<uint64_t, uint64_t>{}(p[0], p[1]);
    }
}  // namespace std
