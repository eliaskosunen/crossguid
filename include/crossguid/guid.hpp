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

#pragma once

#include <array>
#include <iosfwd>
#include <string>

// Check for C++14 constexpr
#ifndef XG_HAS_RELAXED_CONSTEXPR
#if (defined(__cpp_constexpr) && __cpp_constexpr >= 201304L) || \
    (defined(__GNUC__) && __GNUC__ >= 5) ||                     \
    (defined(__clang__) && defined(__clang_minor__) &&          \
     (__clang__ * 100 + __clang_minor__) >= 304) ||             \
    (defined(_MSC_VER) && _MSC_VER >= 1910)
#define XG_HAS_RELAXED_CONSTEXPR 1
#else
#define XG_HAS_RELAXED_CONSTEXPR 0
#endif
#endif  // !defined(XG_HAS_RELAXED_CONSTEXPR)

#if XG_HAS_RELAXED_CONSTEXPR
/// \exclude
#define XG_CONSTEXPR14 constexpr
#else
/// \exclude
#define XG_CONSTEXPR14 /*constexpr*/
#endif

namespace xg {
    /// A GUID (Globally Unique IDentifier)/UUID (Universally Unique
    /// IDentifier).
    ///
    /// The byte representation of a GUID is a 128-bit unsigned integer, stored
    /// at a byte (`unsigned char`) array of 16 elements.
    ///
    /// The textual representation of a GUID is a string with a total of 36
    /// characters, displayed in 5 groups separated by hyphens, in the form
    /// 8-4-4-4-12, for a total of 36 characters (32 hexadecimal digits and 4
    /// hyphens).
    ///
    /// There is a special GUID value, called the nil value, which has all of
    /// its bits in its byte representation set to `0`. The nil value represents
    /// an invalid GUID.
    class guid {
    public:
        /// \effects Constructs a nil GUID.
        /// \output_section Constructors
        XG_CONSTEXPR14 guid() noexcept = default;

        /// \effects Constructs a GUID from a byte representation.
        constexpr explicit guid(std::array<unsigned char, 16> b) noexcept
            : _bytes{std::move(b)}
        {
        }
        /// \effects Constructs a GUID from a byte representation.
        explicit guid(const unsigned char (&b)[16]) noexcept : _bytes{{0}}
        {
            std::copy(b, b + 16, _bytes.begin());
        }
        /// \effects Constructs a GUID from a textual representation.
        /// If `data` is not a valid GUID textual representation, `*this` will
        /// contain a nil GUID.
        ///
        /// \throws Nothing
        ///
        /// \remarks `data` doesn't need to be in the canonical textual
        /// representation form, like returned by
        /// [`str()`](standardese://str_ret/). Hyphens (`-`) are skipped in the
        /// parsing process, so there can be any number of hyphens.
        /// Alphabetic hexadecimal digits (a-f) can be both uppercase or
        /// lowercase.
        explicit guid(const char* s);

        /// \effects Populates `s` with the textual representation of the GUID
        /// contained in `*this`.
        /// Equivalent to: `s.clear(); str_to(std::back_inserter(s));`
        ///
        /// \throws Any exceptions thrown by `s`.
        ///
        /// \unique_name str_ref
        /// \output_section Textual representation
        void str(std::string& s) const;
        /// \effects Equivalent to: `std::string s; guid.str(s); return s;`.
        ///
        /// \returns The textual representation of the GUID contained in
        /// `*this`.
        ///
        /// \throws Any exceptions thrown by `s`.
        ///
        /// \unique_name str_ret
        std::string str() const;

        /// \requires `OutputIt` must meet the requirements of
        /// `LegacyOutputIterator`.
        /// `std::iterator_traits<OutputIt>::value_type` must be `char`.
        /// The range beginning at `it` must be long enough to hold the 36
        /// characters needed for the textual representation of a GUID.
        ///
        /// \effects Assigns `char`s for the textual representation of the GUID
        /// contained in `*this` to the range beginning at `it`.
        ///
        /// \returns Iterator one past the last element assigned.
        ///
        /// \throws Any exceptions throw by incrementing, dereferencing or
        /// assigning to `it`.
        ///
        /// \unique_name str_to
        template <typename OutputIt>
        OutputIt str_to(OutputIt it) const;

        /// \returns A constant reference to the byte representation of the GUID
        /// contained in `*this`.
        ///
        /// \output_section Byte representation
        constexpr const std::array<unsigned char, 16>& bytes() const noexcept
        {
            return _bytes;
        }
        /// \returns A pointer to the beginning of the byte representation of
        /// the GUID contained in `*this`.
        ///
        /// \notes The byte representation of a GUID is 16 bytes long.
        const unsigned char* data() const noexcept
        {
            return _bytes.data();
        }

        /// \returns `true` if `*this` represents a valid GUID.
        /// \unique_name operator_bool
        /// \output_section Operators
        explicit operator bool() const noexcept;

        /// \effects Swaps the GUID contained in `*this` with that contained in
        /// `other`.
        void swap(guid& other) noexcept;

    private:
        void zeroify() noexcept;

        std::array<unsigned char, 16> _bytes{{0}};
    };

    /// \effects Streams the textual representation of `guid` into `s`.
    /// \returns `s`
    std::ostream& operator<<(std::ostream& s, const guid& guid);

    /// Creates a valid GUID.
    /// \effects Creates a GUID using platform APIs.
    /// \returns A valid [xg::guid]().
    guid make_guid();

    /// Creates a GUID from a byte representation.
    /// \requires Range starting from `p` must be at least 16 elements long.
    guid make_guid_from_bytes(unsigned char* p);

    /// \returns `true` if the GUIDs contained in `lhs` and `rhs` compare equal.
    inline bool operator==(const guid& lhs, const guid& rhs) noexcept
    {
        return lhs.bytes() == rhs.bytes();
    }

    /// \returns `false` if the GUIDs contained in `lhs` and `rhs` compare
    /// equal.
    inline bool operator!=(const guid& lhs, const guid& rhs) noexcept
    {
        return !(operator==(lhs, rhs));
    }

    // definitions

    /// \exclude
    inline guid::operator bool() const noexcept
    {
        guid empty;
        return *this != empty;
    }

    /// \exclude
    inline void guid::str(std::string& s) const
    {
        s.clear();
        s.resize(36);
        str_to(s.begin());
    }
    /// \exclude
    inline std::string guid::str() const
    {
        std::string s;
        str(s);
        return s;
    }

    /// \exclude
    template <typename OutputIt>
    OutputIt guid::str_to(OutputIt it) const
    {
        char one[9], two[5], three[5], four[5], five[13];

        snprintf(one, 9, "%02x%02x%02x%02x", _bytes[0], _bytes[1], _bytes[2],
                 _bytes[3]);
        snprintf(two, 5, "%02x%02x", _bytes[4], _bytes[5]);
        snprintf(three, 5, "%02x%02x", _bytes[6], _bytes[7]);
        snprintf(four, 5, "%02x%02x", _bytes[8], _bytes[9]);
        snprintf(five, 13, "%02x%02x%02x%02x%02x%02x", _bytes[10], _bytes[11],
                 _bytes[12], _bytes[13], _bytes[14], _bytes[15]);

        it = std::copy(one, one + 8, it);
        *it++ = '-';
        it = std::copy(two, two + 4, it);
        *it++ = '-';
        it = std::copy(three, three + 4, it);
        *it++ = '-';
        it = std::copy(four, four + 4, it);
        *it++ = '-';
        it = std::copy(five, five + 12, it);
        return it;
    }

    /// \exclude
    inline void guid::zeroify() noexcept
    {
        std::fill(_bytes.begin(), _bytes.end(), static_cast<unsigned char>(0));
    }

    /// \exclude
    inline void guid::swap(guid& other) noexcept
    {
        _bytes.swap(other._bytes);
    }
}  // namespace xg

namespace std {
    template <>
    inline void swap(xg::guid& lhs, xg::guid& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <>
    struct less<xg::guid> {
        inline bool operator()(const xg::guid& lhs, const xg::guid& rhs) const
        {
            return lhs.bytes() < rhs.bytes();
        }
    };

    template <>
    struct hash<xg::guid> {
        std::size_t operator()(const xg::guid& guid) const;
    };
}  // namespace std
