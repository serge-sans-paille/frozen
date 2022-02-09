#include <frozen/array.h>

#include "bench.hpp"
#include "catch.hpp"

TEST_CASE("empty frozen array", "[array]") {
  constexpr frozen::array<int, 0> ze_array{};

  constexpr auto empty = ze_array.empty();
  REQUIRE(empty);

  constexpr auto size = ze_array.size();
  REQUIRE(size == 0);

  constexpr auto max_size = ze_array.max_size();
  REQUIRE(max_size == 0);

  auto constexpr begin = ze_array.begin(), end = ze_array.end();
  REQUIRE(begin == end);

  auto constexpr cbegin = ze_array.cbegin(), cend = ze_array.cend();
  REQUIRE(cbegin == cend);

  std::for_each(ze_array.begin(), ze_array.end(), [](int) {});
  REQUIRE(std::distance(ze_array.rbegin(), ze_array.rend()) == 0);
  REQUIRE(std::count(ze_array.crbegin(), ze_array.crend(), 3) == 0);
}

TEST_CASE("singleton frozen array", "[array]") {
  constexpr frozen::array<short, 1> ze_array{1};

  constexpr auto empty = ze_array.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_array.size();
  REQUIRE(size == 1);

  constexpr auto max_size = ze_array.max_size();
  REQUIRE(max_size == 1);

  const auto value_op = ze_array[0];
  REQUIRE(value_op == 1);

  const auto value_at = ze_array.at(0);
  REQUIRE(value_at == 1);

  auto const begin = ze_array.begin(), end = ze_array.end();
  REQUIRE((begin + 1) == end);

  auto const cbegin = ze_array.cbegin(), cend = ze_array.cend();
  REQUIRE(cbegin == (cend - 1));

  std::for_each(ze_array.begin(), ze_array.end(), [](int) {});
  REQUIRE(std::distance(ze_array.rbegin(), ze_array.rend()) == 1);
  REQUIRE(std::count(ze_array.crbegin(), ze_array.crend(), 3) == 0);
  REQUIRE(std::count(ze_array.crbegin(), ze_array.crend(), 1) == 1);
}

TEST_CASE("array from array", "[array]") {
  constexpr short data[4]{1, 2, 3, 4};
  constexpr frozen::array<short, 4> ze_array(data);
  constexpr auto value = ze_array[2];
  REQUIRE(value == 3);
}

TEST_CASE("array from initializer", "[array]") {
  constexpr frozen::array<short, 4> ze_array({1, 2, 3, 4});
  constexpr auto value = ze_array[2];
  REQUIRE(value == 3);
}
