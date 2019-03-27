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

#include <doctest.h>

#include <iostream>
#include <sstream>
#include <type_traits>
#include <unordered_map>

TEST_CASE("construction")
{
    SUBCASE("default construction")
    {
        auto g = xg::guid{};
        CHECK(!g);
        CHECK(g == xg::guid{});
    }
    SUBCASE("make_guid")
    {
        auto g = xg::make_guid();
        CHECK(g);
        CHECK(g != xg::make_guid());
    }
    SUBCASE("string")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};
        CHECK(g);
        CHECK(g != xg::make_guid());
        CHECK(g.str() == str);
    }
}

TEST_CASE("textual representation")
{
    SUBCASE("str return")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};
        CHECK(g.str() == str);
    }
    SUBCASE("str reference")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};
        std::string cmpstr{};
        g.str(cmpstr);
        CHECK(str == cmpstr);
    }
    SUBCASE("str_to begin")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};

        std::string cmpstr(36, '\0');
        g.str_to(cmpstr.begin());
        CHECK(str == cmpstr);
    }
    SUBCASE("str_to back_inserter")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};

        std::string cmpstr{};
        g.str_to(std::back_inserter(cmpstr));
        CHECK(str == cmpstr);
    }
    SUBCASE("ostream")
    {
        auto str = "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e";
        auto g = xg::guid{str};

        std::ostringstream oss;
        oss << g;
        CHECK(oss.str() == str);
    }
}

TEST_CASE("byte representation")
{
    std::array<unsigned char, 16> bytes = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                            0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                                            0x0d, 0x0e, 0x0f, 0xdd}};
    xg::guid from_bytes(bytes);
    xg::guid from_string("0102030405060708090a0b0c0d0e0fdd");
    CHECK(from_bytes);
    CHECK(from_string);
    CHECK(from_bytes == from_string);
    CHECK(std::equal(from_bytes.bytes().begin(), from_bytes.bytes().end(),
                     bytes.begin()));
    CHECK(std::equal(from_bytes.data(), from_bytes.data() + 16, bytes.begin()));
}

TEST_CASE("misc operations")
{
    SUBCASE("equality and swap")
    {
        auto first = xg::make_guid();
        auto second = xg::make_guid();
        auto third = first;
        auto fourth = second;

        CHECK(first == third);
        CHECK(second == fourth);
        CHECK(first != second);

        first.swap(second);
        CHECK(first == fourth);
        CHECK(second == third);
        CHECK(first != second);
    }
    SUBCASE("hashing")
    {
        auto g1 = xg::make_guid();
        auto g2 = xg::make_guid();
        auto g3 = xg::make_guid();
        std::unordered_map<xg::guid, int> m = {{g1, 1}, {g2, 2}};

        auto it1 = m.find(g1);
        REQUIRE(it1 != m.end());
        CHECK(it1->first == g1);
        CHECK(it1->second == 1);

        auto it2 = m.find(g2);
        REQUIRE(it2 != m.end());
        CHECK(it2->first == g2);
        CHECK(it2->second == 2);

        auto it3 = m.find(g3);
        CHECK(it3 == m.end());
    }
}

TEST_CASE("errors")
{
    xg::guid empty{};
    CHECK(!empty);

    SUBCASE("too few chars")
    {
        xg::guid two_too_few("7bcd757f-5b10-4f9b-af69-1a1f226f3b");
        xg::guid one_too_few("16d1bd03-09a5-47d3-944b-5e326fd52d2");
        CHECK(two_too_few == empty);
        CHECK(one_too_few == empty);
        CHECK(!two_too_few);
        CHECK(!one_too_few);
    }
    SUBCASE("too many chars")
    {
        xg::guid two_too_many("7bcd757f-5b10-4f9b-af69-1a1f226f3beeff");
        xg::guid one_too_many("16d1bd03-09a5-47d3-944b-5e326fd52d27a");
        CHECK(two_too_many == empty);
        CHECK(one_too_many == empty);
        CHECK(!two_too_many);
        CHECK(!one_too_many);
    }
    SUBCASE("bad string")
    {
        xg::guid bad_string("!!bad-guid-string!!");
        CHECK(bad_string == empty);
        CHECK(!bad_string);
    }
}
