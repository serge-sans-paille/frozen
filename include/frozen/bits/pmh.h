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

namespace frozen {

namespace bits {

// Function object for sorting buckets in decreasing order of size
struct bucket_size_compare {
  template <typename B>
  bool constexpr operator()(B const &b0,
                            B const &b1) const {
    return b0.size() > b1.size();
  }
};

// Check if an item appears in a cvector
template<class T, size_t N>
constexpr bool all_different_from(cvector<T, N> & data, T & a) {
  for (std::size_t i = 0; i < data.size(); ++i)
    if (data[i] == a)
      return false;

  return true;
}

// Represents either an index to a data item array, or a seed to be used with
// a hasher. Seed must have high bit of 1, value has high bit of zero.
struct seed_or_index {
  using value_type = uint64_t;

private:
  static constexpr value_type MINUS_ONE = -1;
  static constexpr value_type HIGH_BIT = ~(MINUS_ONE >> 1);

  value_type value_ = 0;

public:
  constexpr value_type value() const { return value_; }
  constexpr bool is_seed() const { return value_ & HIGH_BIT; }

  constexpr seed_or_index(bool is_seed, value_type value)
    : value_(is_seed ? (value | HIGH_BIT) : (value & ~HIGH_BIT)) {}

  constexpr seed_or_index() = default;
  constexpr seed_or_index(const seed_or_index &) = default;
  constexpr seed_or_index & operator =(const seed_or_index &) = default;
};

// Represents the perfect hash function created by pmh algorithm
template <std::size_t M, class Hasher>
struct pmh_tables {
  uint64_t first_seed_;
  carray<seed_or_index, M> first_table_;
  carray<uint64_t, M> second_table_;
  Hasher hash_;

  // Looks up a given key, to find its expected index in carray<Item, N>
  // Always returns a valid index, must use KeyEqual test after to confirm.
  template <typename KeyType>
  constexpr uint64_t lookup(const KeyType & key) const {
    auto const d = first_table_[hash_(key, first_seed_) % M];
    if (!d.is_seed()) { return d.value(); }
    else { return second_table_[hash_(key, d.value()) % M]; }
  }
};

// Make pmh tables for given items, hash function, prg, etc.
template <std::size_t M, class Item, std::size_t N, class Hash, class Key, class PRG>
pmh_tables<M, Hash> constexpr make_pmh_tables(const carray<Item, N> &
                                                               items,
                                                           Hash const &hash,
                                                           Key const &key,
                                                           PRG prg) {
  // Step 0: Bucket max is 2 * sqrt M
  // TODO: Come up with justification for this, should it not be O(log M)?
  constexpr auto bucket_max = 2 * (1u << (log(M) / 2));

  // Step 1: Place all of the keys into buckets
  using bucket_t = cvector<std::size_t, bucket_max>;
  carray<bucket_t, M> buckets;

  uint64_t first_seed = prg();

  // Continue until all items are placed without exceeding bucket_max
  while (1) {
    for (std::size_t i = 0; i < N; ++i) {
      auto & bucket = buckets[hash(key(items[i]), first_seed) % M];
      if (bucket.size() == bucket_max) {
        first_seed = prg();
        buckets = {};
        continue;
      }
      bucket.push_back(i);
    }
    break;
  }

  // Step 2: Sort the buckets to process the ones with the most items first.
  bits::quicksort(buckets.begin(), buckets.end() - 1, bucket_size_compare{});

  // G becomes the first hash table in the resulting pmh function
  carray<seed_or_index, M> G; // Default constructed to "index 0"

  // H becomes the second hash table in the resulting pmh function
  constexpr uint64_t UNUSED = -1;
  carray<uint64_t, M> H;
  for (auto & h : H) { h = UNUSED; }

  // Step 3: Map the items in buckets into hash tables.
  for (const auto & bucket : buckets) {
    auto const bsize = bucket.size();

    if (bsize == 1) {
      // Store index to the (single) item in G
      G[hash(key(items[bucket[0]]), first_seed) % M] = {false, bucket[0]};
    } else if (bsize > 1) {

      // Repeatedly try different H of d until we find a hash function
      // that places all items in the bucket into free slots
      seed_or_index d{true, prg()};
      cvector<std::size_t, bucket_max> slots;

      while (slots.size() < bsize) {
        auto slot = hash(key(items[bucket[slots.size()]]), d.value()) % M;

        if (H[slot] != UNUSED || !all_different_from(slots, slot)) {
          slots.clear();
          d = {true, prg()};
          continue;
        }

        slots.push_back(slot);
      }

      // Put successful seed in G, and put indices to items in their slots
      G[hash(key(items[bucket[0]]), first_seed) % M] = d;
      for (std::size_t i = 0; i < bsize; ++i)
        H[slots[i]] = bucket[i];
    }
  }

  // Any unused entries in the H table have to get changed to zero.
  // This is because hashing should not fail or return an out-of-bounds entry.
  // A lookup fails after we apply user-supplied KeyEqual to the query and the
  // key found by hashing.
  for (std::size_t i = 0; i < M; ++i)
    if (H[i] == UNUSED)
      H[i] = 0;

  return {first_seed, G, H, hash};
}

} // namespace bits

} // namespace frozen

#endif
