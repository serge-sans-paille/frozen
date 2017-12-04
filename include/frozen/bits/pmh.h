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

// Represents the two hash tables created by pmh algorithm
template <std::size_t M, class Hasher>
struct pmh_tables {
  std::array<int64_t, M> first_table_;
  std::array<uint64_t, M> second_table_;
  Hasher hash_;

  // Looks up a given key, to find its expected index in std::array<Item, N>
  // Always returns a valid index, must use KeyEqual test after to confirm.
  template <typename KeyType>
  constexpr uint64_t lookup(const KeyType & key) const {
    auto const d = first_table_[hash_(key) % M];
    auto const index = (d < 0) ? (-d - 1) : (hash_(key, d) % M);
    return second_table_[index];
  }
};

constexpr std::size_t next_candidate(std::size_t seed) {
  // parameters from Numerical Recipes for an uint32_t, see https://en.wikipedia.org/wiki/Linear_congruential_generator
  constexpr std::size_t a = 1664525;
  constexpr std::size_t c = 1013904223;
  return (a * seed + c) >> 1;
}

template <std::size_t M, class Item, std::size_t N, class Hash, class Key>
pmh_tables<M, Hash> constexpr make_pmh_tables(const std::array<Item, N> &
                                                               items,
                                                           Hash const &hash,
                                                           Key const &key) {
  // Step 1: Place all of the keys into buckets
  cvector<bucket<M>, M> buckets;
  cvector<uint64_t, M> values;
  cvector<int64_t, M> G;

  auto *it = &std::get<0>(items);

  for (std::size_t i = 0; i < N; ++i)
    buckets[hash(key(it[i])) % M].push_back(1 + i);

  // Step 2: Sort the buckets and process the ones with the most items first.
  bits::quicksort(buckets.begin(), buckets.begin() + M - 1, bucket_size_compare{});

  std::size_t b = 0;
  for (; b < M; ++b) {
    auto &bucket = buckets[b];
    auto const bsize = bucket.size();
    if (bsize <= 1)
      break;

    // Repeatedly try different values of d until we find a hash function
    // that places all items in the bucket into free slots
    std::size_t d = 1;
    cvector<std::size_t, M> slots;

    while (slots.size() < bsize) {
      auto slot = hash(key(it[bucket[slots.size()] - 1]), d) % M;

      if (values[slot] != 0 || !all_different_from(slots, slot)) {
        slots.clear();
        d = next_candidate(d);
        continue;
      }

      slots.push_back(slot);
    }

    G[hash(key(it[bucket[0] - 1])) % M] = d;
    for (std::size_t i = 0; i < bsize; ++i)
      values[slots[i]] = bucket[i];
  }

  // Only buckets with 1 item remain. Process them more quickly by directly
  // placing them into a free slot. Use a negative value of d to indicate
  // this.
  cvector<int64_t, M> freelist;

  for (std::size_t i = 0; i < M; ++i)
    if (values[i] == 0)
      freelist.push_back(i);

  for (std::size_t i = b; i < M; ++i) {
    auto &bucket = buckets[i];
    if (bucket.size() == 0)
      break;
    auto slot = freelist.back();
    freelist.pop_back();
    // We subtract one to ensure it's negative even if the zeroeth slot was
    // used.
    G[hash(key(it[bucket[0] - 1])) % M] = -slot - 1;
    values[slot] = bucket[0];
  }
  for (std::size_t i = 0; i < M; ++i)
    if (values[i])
      values[i]--;

  return {G.to_array(), values.to_array(), hash};
}

} // namespace bits

} // namespace frozen

#endif
