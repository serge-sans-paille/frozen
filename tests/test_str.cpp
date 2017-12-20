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
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::kmp::make_needle("n"));
    REQUIRE(index == haystack.begin());
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::kmp::make_needle("nn"));
    REQUIRE(std::distance(haystack.begin(), index) == 2);
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::kmp::make_needle("mm"));
    REQUIRE(index == haystack.end());
  }


  {
    std::string haystack = "ABC ABCDAB ABCDABCDABDE";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::kmp::make_needle("ABCDABD"));
    REQUIRE(std::distance(haystack.begin(), index) == 15);
  }

}

TEST_CASE("Boyer-Moore str search", "[str-search]") {

  {
    std::string haystack = "n";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::bm::make_needle("n"));
    REQUIRE(index == haystack.begin());
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::bm::make_needle("nn"));
    REQUIRE(std::distance(haystack.begin(), index) == 2);
  }

  {
    std::string haystack = "nmnn";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::bm::make_needle("mm"));
    REQUIRE(index == haystack.end());
  }


  {
    std::string haystack = "ABC ABCDAB ABCDABCDABDE";
    auto index = frozen::search(haystack.begin(), haystack.end(), frozen::bm::make_needle("ABCDABD"));
    REQUIRE(std::distance(haystack.begin(), index) == 15);
  }

}

TEST_CASE("Knuth-Morris-Pratt str perf", "[str-search-perf]") {
  std::string haystack = "ABC ABCDAB ABCDABCDABDE";
  for(int i = 0; i < 10; ++i)
    haystack = "AAAAAAAAAAAAAAAA" + haystack;

  auto std_start = std::chrono::steady_clock::now();
  benchmark::DoNotOptimize(haystack);
  benchmark::DoNotOptimize(haystack.find("ABCDABD"));
  auto std_stop = std::chrono::steady_clock::now();
  auto std_diff = std_stop - std_start;
  auto std_duration =
      std::chrono::duration<double, std::milli>(std_diff).count();
  std::cout << "str search: " << std_duration << " ms" << std::endl;

  auto frozen_start = std::chrono::steady_clock::now();
  benchmark::DoNotOptimize(haystack);
  benchmark::DoNotOptimize(frozen::search(haystack.begin(), haystack.end(), frozen::kmp::make_needle("ABCDABD")));
  auto frozen_stop = std::chrono::steady_clock::now();
  auto frozen_diff = frozen_stop - frozen_start;
  auto frozen_duration =
      std::chrono::duration<double, std::milli>(frozen_diff).count();
  std::cout << "frozen::search: " << frozen_duration << " ms"
            << std::endl;

}

TEST_CASE("str str perf bm", "[str-search-perf]") {
  std::string haystack = "ABC ABCDAB ABCDABCDABDE";
  for(int i = 0; i < 10; ++i)
    haystack = "AAAAAAAAAAAAAAAA" + haystack;

  auto std_start = std::chrono::steady_clock::now();
  benchmark::DoNotOptimize(haystack);
  benchmark::DoNotOptimize(haystack.find("ABCDABD"));
  auto std_stop = std::chrono::steady_clock::now();
  auto std_diff = std_stop - std_start;
  auto std_duration =
      std::chrono::duration<double, std::milli>(std_diff).count();
  std::cout << "str search: " << std_duration << " ms" << std::endl;

  auto frozen_start = std::chrono::steady_clock::now();
  benchmark::DoNotOptimize(haystack);
  benchmark::DoNotOptimize(frozen::search(haystack.begin(), haystack.end(), frozen::bm::make_needle("ABCDABD")));
  auto frozen_stop = std::chrono::steady_clock::now();
  auto frozen_diff = frozen_stop - frozen_start;
  auto frozen_duration =
      std::chrono::duration<double, std::milli>(frozen_diff).count();
  std::cout << "frozen::search: " << frozen_duration << " ms"
            << std::endl;

}
