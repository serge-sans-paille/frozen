#include <frozen/vector.h>

#include "bench.hpp"
#include "catch.hpp"

/*
TEST_CASE("empty frozen vector", "[vector]") {
  constexpr frozen::vector<int, 0> ze_vector{};

  constexpr auto empty = ze_vector.empty();
  REQUIRE(empty);

  constexpr auto size = ze_vector.size();
  REQUIRE(size == 0);

  constexpr auto max_size = ze_vector.max_size();
  REQUIRE(max_size == 0);

  constexpr auto capacity = ze_vector.capacity();
  REQUIRE(capacity == 0);

  auto constexpr begin = ze_vector.begin(), end = ze_vector.end();
  REQUIRE(begin == end);

  auto constexpr cbegin = ze_vector.cbegin(), cend = ze_vector.cend();
  REQUIRE(cbegin == cend);

  std::for_each(ze_vector.begin(), ze_vector.end(), [](int) {});
  REQUIRE(std::distance(ze_vector.rbegin(), ze_vector.rend()) == 0);
  REQUIRE(std::count(ze_vector.crbegin(), ze_vector.crend(), 3) == 0);
}
*/

TEST_CASE("singleton frozen vector", "[vector]") {
  constexpr frozen::vector<short, 1> ze_vector{1};

  constexpr auto empty = ze_vector.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_vector.size();
  REQUIRE(size == 1);

  constexpr auto max_size = ze_vector.max_size();
  REQUIRE(max_size == 1);

  constexpr auto capacity = ze_vector.capacity();
  REQUIRE(capacity == 1);

  const auto value_op = ze_vector[0];
  REQUIRE(value_op == 1);

  const auto value_at = ze_vector.at(0);
  REQUIRE(value_at == 1);

  auto const begin = ze_vector.begin(), end = ze_vector.end();
  REQUIRE((begin + 1) == end);

  auto const cbegin = ze_vector.cbegin(), cend = ze_vector.cend();
  REQUIRE(cbegin == (cend - 1));

  std::for_each(ze_vector.begin(), ze_vector.end(), [](int) {});
  REQUIRE(std::distance(ze_vector.rbegin(), ze_vector.rend()) == 1);
  REQUIRE(std::count(ze_vector.crbegin(), ze_vector.crend(), 3) == 0);
  REQUIRE(std::count(ze_vector.crbegin(), ze_vector.crend(), 1) == 1);
}

TEST_CASE("non-full frozen vector", "[vector]") {
  constexpr frozen::vector<short, 4> ze_vector{1};

  constexpr auto empty = ze_vector.empty();
  REQUIRE(!empty);

  constexpr auto size = ze_vector.size();
  REQUIRE(size == 1);

  constexpr auto max_size = ze_vector.max_size();
  REQUIRE(max_size == 4);

  constexpr auto capacity = ze_vector.capacity();
  REQUIRE(capacity == 4);

  const auto value_op = ze_vector[0];
  REQUIRE(value_op == 1);

  const auto value_at = ze_vector.at(0);
  REQUIRE(value_at == 1);

  auto const begin = ze_vector.begin(), end = ze_vector.end();
  REQUIRE((begin + 1) == end);

  auto const cbegin = ze_vector.cbegin(), cend = ze_vector.cend();
  REQUIRE(cbegin == (cend - 1));

  std::for_each(ze_vector.begin(), ze_vector.end(), [](int) {});
  REQUIRE(std::distance(ze_vector.rbegin(), ze_vector.rend()) == 1);
  REQUIRE(std::count(ze_vector.crbegin(), ze_vector.crend(), 3) == 0);
  REQUIRE(std::count(ze_vector.crbegin(), ze_vector.crend(), 1) == 1);
}

TEST_CASE("vector from array", "[vector]") {
  constexpr short data1[4]{1, 2, 3, 4};
  constexpr frozen::vector<short, 4> ze_vector1(data1);

  constexpr auto value1 = ze_vector1[2];
  REQUIRE(value1 == 3);

  constexpr short data2[3]{1, 2, 3};
  constexpr frozen::vector<short, 4> ze_vector2(data2);

  constexpr auto value2 = ze_vector2[2];
  REQUIRE(value2 == 3);
}

TEST_CASE("vector from initializer", "[vector]") {
  constexpr frozen::vector<short, 4> ze_vector1({1, 2, 3, 4});
  constexpr auto value1 = ze_vector1[2];
  REQUIRE(value1 == 3);

  constexpr frozen::vector<short, 4> ze_vector2({1, 2, 3});
  constexpr auto value2 = ze_vector2[2];
  REQUIRE(value2 == 3);
}
