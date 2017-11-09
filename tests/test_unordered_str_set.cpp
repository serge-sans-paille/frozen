#include <chrono>
#include <frozen/string.h>
#include <frozen/unordered_set.h>
#include <iostream>
#include <unordered_set>

#include "bench.hpp"
#include "catch.hpp"

using namespace frozen::string_literals;

TEST_CASE("tripleton int frozen unordered set", "[unordered set]") {
  constexpr frozen::unordered_set<frozen::string, 3> ze_set{"1"_s, "2"_s,
                                                            "3"_s};

  constexpr auto empty = ze_set.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_set.size();
  REQUIRE(size == 3);

  constexpr auto max_size = ze_set.max_size();
  REQUIRE(max_size == 3);

  constexpr auto nocount = ze_set.count("4"_s);
  REQUIRE(nocount == 0);

  constexpr auto count = ze_set.count("1"_s);
  REQUIRE(count == 1);

  auto notfound = ze_set.find("4"_s);
  REQUIRE(notfound == ze_set.end());

  auto found = ze_set.find("1"_s);
  REQUIRE(found == ze_set.begin());

  auto range = ze_set.equal_range("1"_s);
  REQUIRE(std::get<0>(range) != ze_set.end());

  auto begin = ze_set.begin(), end = ze_set.end();
  REQUIRE(begin != end);

  auto constexpr key_hash = ze_set.hash_function();
  auto constexpr key_hashed = key_hash("1"_s);
  REQUIRE(key_hashed);

  auto constexpr key_eq = ze_set.key_eq();
  auto constexpr value_comparison = key_eq("11"_s, "11"_s);
  REQUIRE(value_comparison);

  auto cbegin = ze_set.cbegin(), cend = ze_set.cend();
  REQUIRE(cbegin != cend);

  std::for_each(ze_set.begin(), ze_set.end(), [](frozen::string const &) {});
}

TEST_CASE("frozen::unordered_set<str> <> std::unordered_set",
          "[unordered_set]") {
#define INIT_SEQ                                                               \
  "19"_s, "1"_s, "2"_s, "4"_s, "5"_s, "6"_s, "7"_s, "8"_s, "9"_s, "10"_s,      \
      "11"_s, "111"_s, "1112"_s, "1115"_s, "1118"_s, "1110"_s, "1977"_s,       \
      "177"_s, "277"_s, "477"_s, "577"_s, "677"_s, "777"_s, "877"_s, "977"_s,  \
      "1077"_s, "1177"_s, "11177"_s, "111277"_s, "111577"_s, "111877"_s,       \
      "111077"_s, "1999"_s, "199"_s, "299"_s, "499"_s, "599"_s, "699"_s,       \
      "799"_s, "899"_s, "999"_s, "1099"_s, "1199"_s, "11199"_s, "111299"_s,    \
      "111599"_s, "111899"_s, "111099"_s, "197799"_s, "17799"_s, "27799"_s,    \
      "47799"_s, "57799"_s, "67799"_s, "77799"_s, "87799"_s, "97799"_s,        \
      "107799"_s, "117799"_s, "1117799"_s, "11127799"_s, "11157799"_s,         \
      "11187799"_s, "11107799"_s, "1988"_s, "188"_s, "288"_s, "488"_s,         \
      "588"_s, "688"_s, "788"_s, "888"_s, "988"_s, "1088"_s, "1188"_s,         \
      "11188"_s, "111288"_s, "111588"_s, "111888"_s, "111088"_s, "197788"_s,   \
      "17788"_s, "27788"_s, "47788"_s, "57788"_s, "67788"_s, "77788"_s,        \
      "87788"_s, "97788"_s, "107788"_s, "117788"_s, "1117788"_s, "11127788"_s, \
      "11157788"_s, "11187788"_s, "11107788"_s, "199988"_s, "19988"_s,         \
      "29988"_s, "49988"_s, "59988"_s, "69988"_s, "79988"_s, "89988"_s,        \
      "99988"_s, "109988"_s, "119988"_s, "1119988"_s, "11129988"_s,            \
      "11159988"_s, "11189988"_s, "11109988"_s, "19779988"_s, "1779988"_s,     \
      "2779988"_s, "4779988"_s, "5779988"_s, "6779988"_s, "7779988"_s,         \
      "8779988"_s, "9779988"_s, "10779988"_s, "11779988"_s, "111779988"_s,     \
      "1112779988"_s, "1115779988"_s, "1118779988"_s, "1110779988"_s

  const std::unordered_set<frozen::string> std_set = {INIT_SEQ};
  constexpr frozen::unordered_set<frozen::string, 128> frozen_set = {INIT_SEQ};
  SECTION("checking size and content") {
    REQUIRE(std_set.size() == frozen_set.size());
    for (auto v : std_set)
      REQUIRE(frozen_set.count(v));

    for (auto v : frozen_set)
      REQUIRE(std_set.count(v));
  }

  SECTION("checking minimal performance requirements") {
    std::array<frozen::string, 128> data{{INIT_SEQ}};

    auto std_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (auto val : data) {
        benchmark::DoNotOptimize(val);
        benchmark::DoNotOptimize(std_set.count(val));
      }
    auto std_stop = std::chrono::steady_clock::now();
    auto std_diff = std_stop - std_start;
    auto std_duration =
        std::chrono::duration<double, std::milli>(std_diff).count();
    std::cout << "std::unordered_set<str>: " << std_duration << " ms"
              << std::endl;

    auto frozen_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (auto val : data) {
        benchmark::DoNotOptimize(val);
        benchmark::DoNotOptimize(frozen_set.count(val));
      }
    auto frozen_stop = std::chrono::steady_clock::now();
    auto frozen_diff = frozen_stop - frozen_start;
    auto frozen_duration =
        std::chrono::duration<double, std::milli>(frozen_diff).count();
    std::cout << "frozen::unordered_set<str>: " << frozen_duration << " ms"
              << std::endl;

    REQUIRE(std_duration > frozen_duration);
  }
}
