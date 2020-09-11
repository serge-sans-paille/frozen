#include <frozen/string.h>
#include <frozen/algorithm.h>
#include <string>
#include <iostream>

#include "bench.hpp"
#include "catch.hpp"

using namespace frozen::string_literals;

TEST_CASE("Various string operation", "[string]") {
  {
    frozen::string letItGo = "Let it go !";
    REQUIRE(letItGo == "Let it go !");
    REQUIRE(letItGo == "Let it go !"_s);

    letItGo = "Let it go, let it go !";
    REQUIRE(letItGo == "Let it go, let it go !");
    REQUIRE(letItGo == "Let it go, let it go !"_s);
  }

  {
    constexpr frozen::string letItGo = "Let it go !";
    static_assert(letItGo == "Let it go !",   "frozen::string constexpr");
    static_assert(letItGo == "Let it go !"_s, "frozen::string constexpr literal");
  }
}

TEST_CASE("Various wstring operation", "[string]") {
  {
    frozen::wstring letItGo = L"Let it go !";
    REQUIRE(letItGo == L"Let it go !");
    REQUIRE(letItGo == L"Let it go !"_s);

    letItGo = L"Let it go, let it go !";
    REQUIRE(letItGo == L"Let it go, let it go !");
    REQUIRE(letItGo == L"Let it go, let it go !"_s);
  }

  {
    constexpr frozen::wstring letItGo = L"Let it go !";
    static_assert(letItGo == L"Let it go !",   "frozen::wstring constexpr");
    static_assert(letItGo == L"Let it go !"_s, "frozen::wstring constexpr literal");
  }
}

TEST_CASE("Various u16string operation", "[string]") {
  {
    frozen::u16string letItGo = u"Let it go !";
    REQUIRE(letItGo == u"Let it go !");
    REQUIRE(letItGo == u"Let it go !"_s);

    letItGo = u"Let it go, let it go !";
    REQUIRE(letItGo == u"Let it go, let it go !");
    REQUIRE(letItGo == u"Let it go, let it go !"_s);
  }

  {
    constexpr frozen::u16string letItGo = u"Let it go !";
    static_assert(letItGo == u"Let it go !",   "frozen::u16string constexpr");
    static_assert(letItGo == u"Let it go !"_s, "frozen::u16string constexpr literal");
  }
}

TEST_CASE("Various u32string operation", "[string]") {
  {
    frozen::u32string letItGo = U"Let it go !";
    REQUIRE(letItGo == U"Let it go !");
    REQUIRE(letItGo == U"Let it go !"_s);

    letItGo = U"Let it go, let it go !";
    REQUIRE(letItGo == U"Let it go, let it go !");
    REQUIRE(letItGo == U"Let it go, let it go !"_s);
  }

  {
    constexpr frozen::u32string letItGo = U"Let it go !";
    static_assert(letItGo == U"Let it go !",   "frozen::u32string constexpr");
    static_assert(letItGo == U"Let it go !"_s, "frozen::u32string constexpr literal");
  }
}

#ifdef __cpp_char8_t
TEST_CASE("Various u8string operation", "[string]") {
  {
    frozen::u8string letItGo = u8"Let it go !";
    REQUIRE(letItGo == u8"Let it go !");
    REQUIRE(letItGo == u8"Let it go !"_s);

    letItGo = u8"Let it go, let it go !";
    REQUIRE(letItGo == u8"Let it go, let it go !");
    REQUIRE(letItGo == u8"Let it go, let it go !"_s);
  }

  {
    constexpr frozen::u8string letItGo = u8"Let it go !";
    static_assert(letItGo == u8"Let it go !",   "frozen::u8string constexpr");
    static_assert(letItGo == u8"Let it go !"_s, "frozen::u8string constexpr literal");
  }
}
#endif

TEST_CASE("Knuth-Morris-Pratt str search", "[str-search]") {

  {
    std::string haystack = "n";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_knuth_morris_pratt_searcher("n"));
    REQUIRE(index == haystack.begin());
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_knuth_morris_pratt_searcher("nn"));
    REQUIRE(std::distance(haystack.begin(), index) == 2);
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_knuth_morris_pratt_searcher("mm"));
    REQUIRE(index == haystack.end());
  }


  {
    std::string haystack = "ABC ABCDAB ABCDABCDABDE";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_knuth_morris_pratt_searcher("ABCDABD"));
    REQUIRE(std::distance(haystack.begin(), index) == 15);
  }

}

TEST_CASE("Boyer-Moore str search", "[str-search]") {

  {
    std::string haystack = "n";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_boyer_moore_searcher("n"));
    REQUIRE(index == haystack.begin());
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_boyer_moore_searcher("nn"));
    REQUIRE(std::distance(haystack.begin(), index) == 2);
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_boyer_moore_searcher("mm"));
    REQUIRE(index == haystack.end());
  }


  {
    std::string haystack = "ABC ABCDAB ABCDABCDABDE";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::make_boyer_moore_searcher("ABCDABD"));
    REQUIRE(std::distance(haystack.begin(), index) == 15);
  }

}
