// include catch before import std to avoid STL issues
#include "bench.hpp"
#include "catch.hpp"

#ifdef FROZEN_USE_STD_MODULE
import std;
#else
#include <algorithm>
#include <iostream>
#include <random>
#endif

#ifdef FROZEN_USE_MODULE
import frozen;
#else 
#include <frozen/random.h>
#endif

TEST_CASE("linear_congruential_engine", "[random]") {

  frozen::linear_congruential_engine<std::uint32_t, 48271u, 0, 0x7fffffff> dist0;
  std::linear_congruential_engine<std::uint32_t, 48271u, 0, 0x7fffffff> rdist0;
  REQUIRE(dist0.min() == rdist0.min());
  REQUIRE(dist0.max() == rdist0.max());
  REQUIRE(dist0() == rdist0());
  REQUIRE(dist0() == rdist0());
  REQUIRE(dist0() == rdist0());

  auto next0 = dist0();
  (void) next0;
  dist0.discard(3);
  rdist0.discard(4);
  REQUIRE(dist0() == rdist0());

  frozen::linear_congruential_engine<std::uint32_t, 48271u, 0, 0x7fffffff> odist0;
  std::linear_congruential_engine<std::uint32_t, 48271u, 0, 0x7fffffff> ordist0;
  REQUIRE(rdist0() != ordist0());
  REQUIRE(dist0() != odist0());
  REQUIRE(!(dist0() == odist0()));


  frozen::minstd_rand dist1;
  (void)dist1;
  frozen::minstd_rand dist2;
  (void)dist2;
  frozen::linear_congruential_engine<std::size_t, 3, 3, 0> dist3;
  (void)dist3;

}

