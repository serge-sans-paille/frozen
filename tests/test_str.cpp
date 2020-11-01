#include <frozen/string.h>
#include <frozen/algorithm.h>
#include <string>
#include <iostream>

#ifdef FROZEN_LETITGO_HAS_STRING_VIEW
#include <string_view>
#include <type_traits>
#include <tuple>
#endif

#include "bench.hpp"
#include "catch.hpp"

using namespace frozen::string_literals;
using namespace std::literals;

template<typename Char>
void test_string_view() {
#ifdef FROZEN_LETITGO_HAS_STRING_VIEW
  constexpr auto strings = []() -> std::tuple<
    const Char(&)[12], frozen::basic_string<Char>, std::basic_string_view<Char>,
    const Char(&)[23], frozen::basic_string<Char>, std::basic_string_view<Char>
  > {
    if constexpr (std::is_same_v<char, Char>) {
      return {
        "Let it go !", "Let it go !"_s, "Let it go !"sv,
        "Let it go, let it go !", "Let it go, let it go !"_s, "Let it go, let it go !"sv
      };
    } else if constexpr (std::is_same_v<wchar_t, Char>) {
      return {
        L"Let it go !", L"Let it go !"_s, L"Let it go !"sv,
        L"Let it go, let it go !", L"Let it go, let it go !"_s, L"Let it go, let it go !"sv
      };
    } else if constexpr (std::is_same_v<char16_t, Char>) {
      return {
        u"Let it go !", u"Let it go !"_s, u"Let it go !"sv,
        u"Let it go, let it go !", u"Let it go, let it go !"_s, u"Let it go, let it go !"sv
      };
    } else if constexpr (std::is_same_v<char32_t, Char>) {
      return {
        U"Let it go !", U"Let it go !"_s, U"Let it go !"sv,
        U"Let it go, let it go !", U"Let it go, let it go !"_s, U"Let it go, let it go !"sv
      };
    }
#ifdef FROZEN_LETITGO_HAS_CHAR8T
    else if constexpr (std::is_same_v<char8_t, Char>) {
      return {
        u8"Let it go !", u8"Let it go !"_s, u8"Let it go !"sv,
        u8"Let it go, let it go !", u8"Let it go, let it go !"_s, u8"Let it go, let it go !"sv
      };
    }
#endif
  }();
  
  const auto [
    letitgo,
    letitgo_s,
    letitgo_sv,
    letitgoletitgo,
    letitgoletitgo_s,
    letitgoletitgo_sv
  ] = strings;

  {
    frozen::basic_string<Char> letItGo = letitgo_sv;
    REQUIRE(letItGo == letitgo);
    REQUIRE(letItGo == letitgo_s);
    REQUIRE(letItGo == letitgo_sv);

    letItGo = letitgoletitgo_sv;
    REQUIRE(letItGo == letitgoletitgo);
    REQUIRE(letItGo == letitgoletitgo_s);
    REQUIRE(letItGo == letitgoletitgo_sv);
  }

  {
    constexpr frozen::basic_string<Char> letItGo = std::get<2>(strings);
    static_assert(letItGo == std::get<0>(strings), "frozen::string constexpr");
    static_assert(letItGo == std::get<1>(strings), "frozen::string constexpr literal");
    static_assert(letItGo == std::get<2>(strings), "frozen::string constexpr string view");
  }
#endif
}

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
  
  test_string_view<char>();
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
  
  test_string_view<wchar_t>();
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
  
  test_string_view<char16_t>();
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
  
  test_string_view<char32_t>();
}

#ifdef FROZEN_LETITGO_HAS_CHAR8T
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
  
  test_string_view<char8_t>();
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
