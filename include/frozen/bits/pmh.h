/*
 * Frozen
 * Copyright 2016 QuarksLab
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

// inspired from http://stevehanov.ca/blog/index.php?id=119
#ifndef FROZEN_LETITGO_PMH_H
#define FROZEN_LETITGO_PMH_H

#include <frozen/bits/algorithms.h>
#include <frozen/bits/basic_types.h>

#include <array>
#include <tuple>

namespace frozen {

namespace bits {

template <size_t N>
using bucket = cvector<std::size_t, N>;

struct bucket_size_compare {
  template <size_t N>
  auto constexpr operator()(bucket<N> const &b0,
                            bucket<N> const &b1) const {
    return b0.size() > b1.size();
  }
};

template<class T, size_t N>
constexpr bool all_different_from(cvector<T, N> & data, T & a) {
  for (std::size_t i = 0; i < data.size(); ++i)
    if (data[i] == a)
      return false;

  return true;
}

// Represents either an index into data items, or a seed for use in
// next hash table. Special bit keeps track of which.
class maybe_seed {
  static constexpr uint64_t special_bit = (1ull << 63);
  uint64_t value_ = 0;

public:
  constexpr maybe_seed(bool is_seed, uint64_t value)
    : value_(is_seed ? (value | special_bit) : (value & (~special_bit))) {}

  constexpr maybe_seed() = default;
  constexpr maybe_seed(const maybe_seed &) = default;
  constexpr maybe_seed & operator =(const maybe_seed &) = default;
  
  constexpr bool is_seed() const { return value_ & special_bit; }
  constexpr uint64_t value() const { return value_; }
};

// Represents the two hash tables created by pmh algorithm
template <std::size_t M, class Hasher>
struct pmh_tables {
  uint64_t first_seed_;
  std::array<maybe_seed, M> first_table_;
  std::array<uint64_t, M> second_table_;
  Hasher hash_;

  // Looks up a given key, to find its expected index in std::array<Item, N>
  // Always returns a valid index, must use KeyEqual test after to confirm.
  template <typename KeyType>
  constexpr uint64_t lookup(const KeyType & key) const {
    auto const & d = first_table_[hash_(key, first_seed_) % M];
    if (!d.is_seed()) { return d.value(); }
    return second_table_[hash_(key, d.value()) % M];
  }
};

template <std::size_t M>
constexpr bool bucket_score_is_good(const cvector<bucket<M>, M> & buckets, uint64_t limit) {
  uint64_t sum = 0;

  unsigned b = 0;
  for (; b < M; ++b) {
    unsigned temp = buckets[b].size();
    if (temp > 1) {
        if (temp > limit) { return false; }
        temp *= temp;
        if (temp > limit) { return false; }
        sum += temp;
        if (sum > limit) { return false; }
    }
  }
  return true;
}

template <std::size_t M, class Item, std::size_t N, class Hash, class Key, class PRG>
pmh_tables<M, Hash> constexpr make_pmh_tables(const std::array<Item, N> &
                                                               items,
                                                           Hash const &hash,
                                                           Key const &key,
                                                           PRG prg) {
  // Step 1: Place all of the keys into buckets
  uint64_t first_seed = 0;
  cvector<bucket<M>, M> buckets;

  // Goal: Sum of squares of bucket sizes (for buckets >= 2) not more than 10 * N
  constexpr uint64_t bucket_score_limit = 10 * N;

  auto *it = &std::get<0>(items);

  do {
    first_seed = prg();
    buckets = {};

    for (std::size_t i = 0; i < N; ++i)
      buckets[hash(key(it[i]), first_seed) % M].push_back(1 + i);

  } while(!bucket_score_is_good(buckets, bucket_score_limit));

  // Step 2: Sort the buckets and process the ones with the most items first.
  bits::quicksort(buckets.begin(), buckets.begin() + M - 1, bucket_size_compare{});

  cvector<uint64_t, M> values;
  cvector<maybe_seed, M> G;

  std::size_t b = 0;
  for (; b < M; ++b) {
    auto &bucket = buckets[b];
    auto const bsize = bucket.size();
    if (bsize > 1) {
      // Repeatedly try different values of d until we find a hash function
      // that places all items in the bucket into free slots
      maybe_seed d{true, prg()};
      cvector<std::size_t, M> slots;

      while (slots.size() < bsize) {
        std::size_t slot = hash(key(it[bucket[slots.size()] - 1]), d.value()) % M;

        if (values[slot] != 0 || !all_different_from(slots, slot)) {
          slots.clear();
          d = maybe_seed{true, prg()};
          continue;
        }

        slots.push_back(slot);
      }

      G[hash(key(it[bucket[0] - 1]), first_seed) % M] = d;
      for (std::size_t i = 0; i < bsize; ++i)
        values[slots[i]] = bucket[i];
    } else if (bsize == 1) {
      G[hash(key(it[bucket[0] - 1]), first_seed) % M] = maybe_seed{false, bucket[0] - 1};
    }
  }

  // Step 3: Adjust the values down by one. This is because when we originally pushed
  // item sinto buckets, we pushed back 1+i.
  //
  // It is okay to send "empty" values to 0, because after any lookup, KeyEqual will (must)
  // be used to check if we actually found the desired key.
  for (std::size_t i = 0; i < M; ++i)
    if (values[i])
      values[i]--;

  return {first_seed, G.to_array(), values.to_array(), hash};
}

} // namespace bits

} // namespace frozen

#endif
