#include <frozen/string.h>
#include <frozen/algorithm.h>
#include <string>
#include <chrono>
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

template<typename Func>
void testStringSearchPerf(char const *message, Func test) {
  std::string haystack = "ABC ABCDAB ABCDABCDABDE";
  for(int i = 0; i < 10; ++i)
    haystack = "AAAAAAAAAAAAAAAA" + haystack;

  auto start = std::chrono::steady_clock::now();
  test(haystack);
  auto stop = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration<double, std::milli>(stop - start).count();
  std::cout << std::setw(20) << std::left << message << duration << " ms" << std::endl;
}

TEST_CASE("Standard string find perf", "[str-search-perf]") {
  testStringSearchPerf("str find:", [](const std::string & haystack) {
    benchmark::DoNotOptimize(haystack);
    benchmark::DoNotOptimize(haystack.find("ABCDABD"));
  });
}

TEST_CASE("Standard string search perf", "[str-search-perf]") {
  const std::string needle = "ABCDABD";
  testStringSearchPerf("std::search:", [&needle](const std::string & haystack) {
    benchmark::DoNotOptimize(haystack);
    benchmark::DoNotOptimize(std::search(haystack.begin(), haystack.end(), needle.begin() , needle.end()));
  });
}

TEST_CASE("Knuth-Morris-Pratt string search perf", "[str-search-perf]") {
  testStringSearchPerf("KMP frozen::search:", [](const std::string & haystack) {
    benchmark::DoNotOptimize(haystack);
    benchmark::DoNotOptimize(frozen::search(haystack.begin(), haystack.end(), frozen::make_knuth_morris_pratt_searcher("ABCDABD")));
  });
}

TEST_CASE("Boyer-Moore string search perf", "[str-search-perf]") {
  testStringSearchPerf("BM frozen::search:", [](const std::string & haystack) {
    benchmark::DoNotOptimize(haystack);
    benchmark::DoNotOptimize(frozen::search(haystack.begin(), haystack.end(), frozen::make_boyer_moore_searcher("ABCDABD")));
  });
}
