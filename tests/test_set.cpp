#include <algorithm>
#include <frozen/set.h>
#include <iostream>
#include <set>
#include <type_traits>

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
  REQUIRE((std::size_t)std::distance(ze_set.rbegin(), ze_set.rend()) == ze_set.size());
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

}

TEST_CASE("frozen::set <> frozen::make_set", "[set]") {
  constexpr frozen::set<int, 128> from_ctor = { INIT_SEQ };
  constexpr int init_array[]{INIT_SEQ};
  constexpr auto from_c_array = frozen::make_set(init_array);
  constexpr auto from_std_array = frozen::make_set(std::array<int, 128>{{INIT_SEQ}});
  REQUIRE(std::equal(from_c_array.begin(), from_c_array.end(), from_std_array.begin()));

  SECTION("checking size and content") {
    REQUIRE(from_ctor.size() == from_c_array.size());
    for (auto v : from_c_array)
      REQUIRE(from_ctor.count(v));
    for (auto v : from_ctor)
      REQUIRE(from_c_array.count(v));
  }

  constexpr frozen::set<short, 0> frozen_empty_set = {};
  constexpr auto frozen_empty_set2 = frozen::make_set<short>();
  constexpr auto frozen_empty_set3 = frozen::make_set<short>({});

  SECTION("checking empty set") {
    REQUIRE(frozen_empty_set.empty());
    REQUIRE(frozen_empty_set.size() == 0);
    REQUIRE(frozen_empty_set.begin() == frozen_empty_set.end());

    REQUIRE(frozen_empty_set2.empty());
    REQUIRE(frozen_empty_set2.size() == 0);
    REQUIRE(frozen_empty_set2.begin() == frozen_empty_set2.end());

    REQUIRE(frozen_empty_set3.empty());
    REQUIRE(frozen_empty_set3.size() == 0);
    REQUIRE(frozen_empty_set3.begin() == frozen_empty_set3.end());
  }
}

TEST_CASE("frozen::set constexpr", "[set]") {
  constexpr frozen::set<unsigned, 2> ce = {3, 11};
  static_assert(*ce.begin() == 3, "");
  static_assert(*(ce.begin() + 1) == 11, "");
  static_assert(ce.size() == 2, "");
  static_assert(ce.count(3), "");
  static_assert(!ce.count(0), "");
  static_assert(ce.find(0) == ce.end(), "");
}

TEST_CASE("frozen::set of frozen::set", "[set]") {
  using s1 = frozen::set<unsigned, 1>;
  constexpr frozen::set<s1, 2> ce = {{3}, {11}};
  static_assert(*ce.begin() == s1({3}), "");
  static_assert(*(ce.begin() + 1) == s1({11}), "");
  static_assert(ce.size() == 2, "");
  static_assert(ce.count(s1({3})), "");
  static_assert(!ce.count(s1({0})), "");
  static_assert(ce.find(s1({0})) == ce.end(), "");
}

struct Foo {
  int x;
};
constexpr inline bool operator==(Foo const &a, Foo const &b) { return a.x == b.x; }
constexpr inline bool operator<(Foo const &a, Foo const &b) { return a.x < b.x; }
constexpr inline bool operator<(Foo const &a, int b) { return a.x < b; }
constexpr inline bool operator<(int a, Foo const &b) { return a < b.x; }

TEST_CASE("frozen::set heterogeneous container", "[set]") {
  constexpr std::array<Foo, 3> std_array{{{1}, {2}, {3}}};
  constexpr Foo c_array[]{{1}, {2}, {3}};
  constexpr auto from_std_array = frozen::make_set<Foo, std::less<>>(std_array);
  constexpr auto from_c_array = frozen::make_set<Foo, std::less<>>(c_array);

  REQUIRE(from_std_array == from_c_array);
  for (const auto& set : {from_std_array, from_c_array}) {
    REQUIRE(set.find(1) != set.end());
    REQUIRE(set.count(2) == 1);
    REQUIRE(set.count(42) == 0);
  }
}

#ifdef FROZEN_LETITGO_HAS_DEDUCTION_GUIDES

TEST_CASE("frozen::set deduction guide", "[set]") {
    constexpr frozen::set integersSet{1,2,3,4,5};
    static_assert(std::is_same<
            std::remove_cv_t<decltype(integersSet)>,
            frozen::set<int, 5>>::value, "wrong type deduced");
}

#endif // FROZEN_LETITGO_HAS_DEDUCTION_GUIDES
