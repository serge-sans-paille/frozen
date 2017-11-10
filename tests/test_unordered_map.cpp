#include <chrono>
#include <frozen/unordered_map.h>
#include <iostream>
#include <unordered_map>

#include "bench.hpp"
#include "catch.hpp"
TEST_CASE("singleton frozen unordered map", "[unordered map]") {
  constexpr frozen::unordered_map<int, double, 1> ze_map{{1, 2.}};

  constexpr auto empty = ze_map.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_map.size();
  REQUIRE(size == 1);

  constexpr auto max_size = ze_map.max_size();
  REQUIRE(max_size == 1);

  constexpr auto nocount = ze_map.count(3);
  REQUIRE(nocount == 0);

  constexpr auto count = ze_map.count(1);
  REQUIRE(count == 1);

  constexpr auto find = ze_map.at(1);
  REQUIRE(find == 2.);

  auto notfound = ze_map.find(3);
  REQUIRE(notfound == ze_map.end());

  auto found = ze_map.find(1);
  REQUIRE(found == ze_map.begin());

  auto range = ze_map.equal_range(1);
  REQUIRE(std::get<0>(range) == ze_map.begin());
  REQUIRE(std::get<1>(range) == ze_map.end());

  auto begin = ze_map.begin(), end = ze_map.end();
  REQUIRE(begin != end);

  auto constexpr key_hash = ze_map.hash_function();
  auto constexpr key_hashed = key_hash(1);
  REQUIRE(key_hashed);

  auto constexpr key_eq = ze_map.key_eq();
  auto constexpr value_comparison = key_eq(11, 11);
  REQUIRE(value_comparison);

  auto cbegin = ze_map.cbegin(), cend = ze_map.cend();
  REQUIRE(cbegin < cend);

  std::for_each(ze_map.begin(), ze_map.end(), [](std::tuple<int, float>) {});

  static_assert(std::is_same<typename decltype(ze_map)::key_type, int>::value, "");
  static_assert(std::is_same<typename decltype(ze_map)::mapped_type, double>::value, "");
}

TEST_CASE("frozen::unordered_map <> std::unordered_map", "[unordered_map]") {
#define INIT_SEQ                                                               \
    {19, 12}, {1, 12}, {2, 12}, {4, 12}, {5, 12}, {6, 12}, {7, 12}, {8, 12},   \
        {9, 12}, {10, 12}, {11, 12}, {111, 12}, {1112, 12}, {1115, 12},        \
        {1118, 12}, {1110, 12}, {1977, 12}, {177, 12}, {277, 12}, {477, 12},   \
        {577, 12}, {677, 12}, {777, 12}, {877, 12}, {977, 12}, {1077, 12},     \
        {1177, 12}, {11177, 12}, {111277, 12}, {111577, 12}, {111877, 12},     \
        {111077, 12}, {1999, 12}, {199, 12}, {299, 12}, {499, 12}, {599, 12},  \
        {699, 12}, {799, 12}, {899, 12}, {999, 12}, {1099, 12}, {1199, 12},    \
        {11199, 12}, {111299, 12}, {111599, 12}, {111899, 12}, {111099, 12},   \
        {197799, 12}, {17799, 12}, {27799, 12}, {47799, 12}, {57799, 12},      \
        {67799, 12}, {77799, 12}, {87799, 12}, {97799, 12}, {107799, 12},      \
        {117799, 12}, {1117799, 12}, {11127799, 12}, {11157799, 12},           \
        {11187799, 12}, {11107799, 12}, {1988, 12}, {188, 12}, {288, 12},      \
        {488, 12}, {588, 12}, {688, 12}, {788, 12}, {888, 12}, {988, 12},      \
        {1088, 12}, {1188, 12}, {11188, 12}, {111288, 12}, {111588, 12},       \
        {111888, 12}, {111088, 12}, {197788, 12}, {17788, 12}, {27788, 12},    \
        {47788, 12}, {57788, 12}, {67788, 12}, {77788, 12}, {87788, 12},       \
        {97788, 12}, {107788, 12}, {117788, 12}, {1117788, 12},                \
        {11127788, 12}, {11157788, 12}, {11187788, 12}, {11107788, 12},        \
        {199988, 12}, {19988, 12}, {29988, 12}, {49988, 12}, {59988, 12},      \
        {69988, 12}, {79988, 12}, {89988, 12}, {99988, 12}, {109988, 12},      \
        {119988, 12}, {1119988, 12}, {11129988, 12}, {11159988, 12},           \
        {11189988, 12}, {11109988, 12}, {19779988, 12}, {1779988, 12},         \
        {2779988, 12}, {4779988, 12}, {5779988, 12}, {6779988, 12},            \
        {7779988, 12}, {8779988, 12}, {9779988, 12}, {10779988, 12},           \
        {11779988, 12}, {111779988, 12}, {1112779988, 12}, {1115779988, 12},   \
        {1118779988, 12}, {                                                    \
      1110779988, 13                                                           \
    }

  const std::unordered_map<int, int> std_map = { INIT_SEQ };
  constexpr frozen::unordered_map<int, int, 128> frozen_map = { INIT_SEQ };
  SECTION("checking size and content") {
    REQUIRE(std_map.size() == frozen_map.size());
    for (auto v : std_map)
      REQUIRE(frozen_map.count(std::get<0>(v)));
    for (auto v : frozen_map)
      REQUIRE(std_map.count(std::get<0>(v)));
  }

  SECTION("checking minimal performance requirements") {
    auto std_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (int j = 0; j < 10000; ++j) {
        benchmark::DoNotOptimize(i);
        benchmark::DoNotOptimize(j);
        benchmark::DoNotOptimize(std_map.count(i + j));
      }
    auto std_stop = std::chrono::steady_clock::now();
    auto std_diff = std_stop - std_start;
    auto std_duration =
        std::chrono::duration<double, std::milli>(std_diff).count();
    std::cout << "std::unordered_map<int, int>: " << std_duration << " ms" << std::endl;


    auto frozen_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (int j = 0; j < 10000; ++j) {
        benchmark::DoNotOptimize(i);
        benchmark::DoNotOptimize(j);
        benchmark::DoNotOptimize(frozen_map.count(i + j));
      }
    auto frozen_stop = std::chrono::steady_clock::now();
    auto frozen_diff = frozen_stop - frozen_start;
    auto frozen_duration =
        std::chrono::duration<double, std::milli>(frozen_diff).count();
    std::cout << "frozen::unordered_map<int, int>: " << frozen_duration << " ms" << std::endl;

    // REQUIRE(std_duration > frozen_duration);
  }

  static_assert(std::is_same<typename decltype(std_map)::key_type,
                             typename decltype(frozen_map)::key_type>::value, "");
  static_assert(std::is_same<typename decltype(std_map)::mapped_type,
                             typename decltype(frozen_map)::mapped_type>::value, "");
}

TEST_CASE("frozen::unordered_map <> frozen::make_unordered_map", "[unordered_map]") {
  constexpr frozen::unordered_map<int, int, 128> frozen_map = { INIT_SEQ };
  constexpr auto frozen_map2 = frozen::make_unordered_map<int, int>({INIT_SEQ});

  SECTION("checking size and content") {
    REQUIRE(frozen_map.size() == frozen_map2.size());
    for (auto v : frozen_map2)
      REQUIRE(frozen_map.count(std::get<0>(v)));
    for (auto v : frozen_map)
      REQUIRE(frozen_map2.count(std::get<0>(v)));
  }
}
