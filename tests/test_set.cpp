#include <algorithm>
#include <chrono>
#include <frozen/set.h>
#include <iostream>
#include <set>

#include "bench.hpp"
#include "catch.hpp"

TEST_CASE("empty frozen set", "[set]") {
  constexpr frozen::set<int, 0> ze_set{};

  constexpr auto empty = ze_set.empty();
  REQUIRE(empty);

  constexpr auto size = ze_set.size();
  REQUIRE(size == 0);

  constexpr auto max_size = ze_set.max_size();
  REQUIRE(max_size == 0);

  constexpr auto count = ze_set.count(3);
  REQUIRE(count == 0);

  constexpr auto find = ze_set.find(5);
  REQUIRE(find == ze_set.end());

  constexpr auto range = ze_set.equal_range(0);
  REQUIRE(std::get<0>(range) == ze_set.end());
  REQUIRE(std::get<1>(range) == ze_set.end());

  constexpr auto lower_bound = ze_set.lower_bound(1);
  REQUIRE(lower_bound == ze_set.end());

  constexpr auto upper_bound = ze_set.upper_bound(1);
  REQUIRE(upper_bound == ze_set.end());

  auto constexpr begin = ze_set.begin(), end = ze_set.end();
  REQUIRE(begin == end);

  auto constexpr key_comp = ze_set.key_comp();
  auto constexpr key_comparison = key_comp(1, 2);
  REQUIRE(key_comparison);

  auto constexpr value_comp = ze_set.value_comp();
  auto constexpr value_comparison = value_comp(11, 12);
  REQUIRE(value_comparison);

  auto constexpr cbegin = ze_set.cbegin(), cend = ze_set.cend();
  REQUIRE(cbegin == cend);

  std::for_each(ze_set.begin(), ze_set.end(), [](int) {});
  REQUIRE(std::distance(ze_set.rbegin(), ze_set.rend()) == 0);
  REQUIRE(std::count(ze_set.crbegin(), ze_set.crend(), 3) == 0);
}

TEST_CASE("singleton frozen set", "[set]") {
  constexpr frozen::set<short, 1> ze_set{1};

  constexpr auto empty = ze_set.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_set.size();
  REQUIRE(size == 1);

  constexpr auto max_size = ze_set.max_size();
  REQUIRE(max_size == 1);

  constexpr auto count1 = ze_set.count(1);
  REQUIRE(count1 == 1);

  constexpr auto count2 = ze_set.count(2);
  REQUIRE(count2 == 0);

  const auto find1 = ze_set.find(1);
  REQUIRE(find1 == ze_set.begin());

  const auto find5 = ze_set.find(5);
  REQUIRE(find5 == ze_set.end());

  const auto range0 = ze_set.equal_range(0);
  REQUIRE(std::get<0>(range0) == ze_set.end());
  REQUIRE(std::get<1>(range0) == ze_set.end());

  const auto range1 = ze_set.equal_range(1);
  REQUIRE(std::get<0>(range1) == ze_set.begin());
  REQUIRE(std::get<1>(range1) == ze_set.end());

  const auto lower_bound0 = ze_set.lower_bound(0);
  REQUIRE(lower_bound0 == ze_set.end());

  const auto lower_bound1 = ze_set.lower_bound(1);
  REQUIRE(lower_bound1 == ze_set.find(1));

  const auto lower_bound2 = ze_set.lower_bound(2);
  REQUIRE(lower_bound2 == ze_set.end());

  const auto upper_bound0 = ze_set.upper_bound(0);
  REQUIRE(upper_bound0 == ze_set.end());

  const auto upper_bound1 = ze_set.upper_bound(1);
  REQUIRE(upper_bound1 == ze_set.end());

  const auto upper_bound2 = ze_set.upper_bound(2);
  REQUIRE(upper_bound2 == ze_set.end());

  auto const begin = ze_set.begin(), end = ze_set.end();
  REQUIRE((begin + 1) == end);

  auto const key_comp = ze_set.key_comp();
  auto const key_comparison = key_comp(1, 2);
  REQUIRE(key_comparison);

  auto const value_comp = ze_set.value_comp();
  auto const value_comparison = value_comp(11, 12);
  REQUIRE(value_comparison);

  auto const cbegin = ze_set.cbegin(), cend = ze_set.cend();
  REQUIRE(cbegin == (cend - 1));

  std::for_each(ze_set.begin(), ze_set.end(), [](int) {});
  REQUIRE(std::distance(ze_set.rbegin(), ze_set.rend()) == 1);
  REQUIRE(std::count(ze_set.crbegin(), ze_set.crend(), 3) == 0);
  REQUIRE(std::count(ze_set.crbegin(), ze_set.crend(), 1) == 1);
}

TEST_CASE("triple frozen set", "[set]") {
  constexpr frozen::set<unsigned long, 3> ze_set{10, 20, 30};

  constexpr auto empty = ze_set.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_set.size();
  REQUIRE(size == 3);

  constexpr auto max_size = ze_set.max_size();
  REQUIRE(max_size == 3);

  constexpr auto count1 = ze_set.count(1);
  REQUIRE(count1 == 0);

  constexpr auto count10 = ze_set.count(10);
  REQUIRE(count10 == 1);

  const auto find10 = ze_set.find(10);
  REQUIRE(find10 == ze_set.begin());

  const auto find15 = ze_set.find(15);
  REQUIRE(find15 == ze_set.end());

  const auto range0 = ze_set.equal_range(0);
  REQUIRE(std::get<0>(range0) == ze_set.end());
  REQUIRE(std::get<1>(range0) == ze_set.end());

  const auto range1 = ze_set.equal_range(10);
  REQUIRE(std::get<0>(range1) == ze_set.begin());
  REQUIRE(std::get<1>(range1) == ze_set.begin() + 1);

  const auto lower_bound0 = ze_set.lower_bound(0);
  REQUIRE(lower_bound0 == ze_set.end());

  for (auto val : ze_set) {
    const auto lower_bound = ze_set.lower_bound(val);
    REQUIRE(lower_bound == ze_set.find(val));
  }

  const auto lower_bound2 = ze_set.lower_bound(40);
  REQUIRE(lower_bound2 == ze_set.end());

  const auto upper_bound0 = ze_set.upper_bound(0);
  REQUIRE(upper_bound0 == ze_set.end());

  const auto upper_bound1 = ze_set.upper_bound(10);
  REQUIRE(upper_bound1 == (ze_set.begin() + 1));

  const auto upper_bound2 = ze_set.upper_bound(40);
  REQUIRE(upper_bound2 == ze_set.end());

  auto const begin = ze_set.begin(), end = ze_set.end();
  REQUIRE((begin + ze_set.size()) == end);

  auto const key_comp = ze_set.key_comp();
  auto const key_comparison = key_comp(1, 2);
  REQUIRE(key_comparison);

  auto const value_comp = ze_set.value_comp();
  auto const value_comparison = value_comp(11, 12);
  REQUIRE(value_comparison);

  auto const cbegin = ze_set.cbegin(), cend = ze_set.cend();
  REQUIRE(cbegin == (cend - ze_set.size()));

  std::for_each(ze_set.begin(), ze_set.end(), [](int) {});
  REQUIRE(std::distance(ze_set.rbegin(), ze_set.rend()) == ze_set.size());
  REQUIRE(std::count(ze_set.crbegin(), ze_set.crend(), 3) == 0);
  REQUIRE(std::count(ze_set.crbegin(), ze_set.crend(), 20) == 1);
}

TEST_CASE("frozen::set <> std::set", "[set]") {
#define INIT_SEQ                                                               \
  19, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 111, 1112, 1115, 1118, 1110, 1977, 177,  \
      277, 477, 577, 677, 777, 877, 977, 1077, 1177, 11177, 111277, 111577,    \
      111877, 111077, 1999, 199, 299, 499, 599, 699, 799, 899, 999, 1099,      \
      1199, 11199, 111299, 111599, 111899, 111099, 197799, 17799, 27799,       \
      47799, 57799, 67799, 77799, 87799, 97799, 107799, 117799, 1117799,       \
      11127799, 11157799, 11187799, 11107799, 1988, 188, 288, 488, 588, 688,   \
      788, 888, 988, 1088, 1188, 11188, 111288, 111588, 111888, 111088,        \
      197788, 17788, 27788, 47788, 57788, 67788, 77788, 87788, 97788, 107788,  \
      117788, 1117788, 11127788, 11157788, 11187788, 11107788, 199988, 19988,  \
      29988, 49988, 59988, 69988, 79988, 89988, 99988, 109988, 119988,         \
      1119988, 11129988, 11159988, 11189988, 11109988, 19779988, 1779988,      \
      2779988, 4779988, 5779988, 6779988, 7779988, 8779988, 9779988, 10779988, \
      11779988, 111779988, 1112779988, 1115779988, 1118779988, 1110779988

  const std::set<int> std_set = {INIT_SEQ};
  constexpr frozen::set<int, 128> frozen_set = {INIT_SEQ};

  SECTION("checking size and content") {
    REQUIRE(std_set.size() == frozen_set.size());
    for (auto v : std_set)
      REQUIRE(frozen_set.count(v));
    for (auto v : frozen_set)
      REQUIRE(std_set.count(v));
  }

  SECTION("printing minimal performance requirements") {
    auto std_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (int j = 0; j < 10000; ++j) {
        benchmark::DoNotOptimize(i);
        benchmark::DoNotOptimize(j);
        benchmark::DoNotOptimize(std_set.count(i+j));
      }
    auto std_stop = std::chrono::steady_clock::now();
    auto std_diff = std_stop - std_start;
    auto std_duration =
        std::chrono::duration<double, std::milli>(std_diff).count();
    std::cout << "std::set<int>: " << std_duration << " ms" << std::endl;

    auto frozen_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (int j = 0; j < 10000; ++j) {
        benchmark::DoNotOptimize(i);
        benchmark::DoNotOptimize(j);
        benchmark::DoNotOptimize(frozen_set.count(i + j));
      }
    auto frozen_stop = std::chrono::steady_clock::now();
    auto frozen_diff = frozen_stop - frozen_start;
    auto frozen_duration =
        std::chrono::duration<double, std::milli>(frozen_diff).count();
    std::cout << "frozen::set<int>: " << frozen_duration << " ms" << std::endl;

    REQUIRE(std_duration > frozen_duration);
  }
}
