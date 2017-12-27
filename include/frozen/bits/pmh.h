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

// Represents either an index to a data item array, or a seed to be used with
// a hasher.
struct seed_or_index {
  bool is_seed;
  uint64_t value;
};

// Represents the two hash tables created by pmh algorithm
template <std::size_t M, class Hasher>
struct pmh_tables {
  carray<seed_or_index, M> first_table_;
  carray<uint64_t, M> second_table_;
  Hasher hash_;

  // Looks up a given key, to find its expected index in carray<Item, N>
  // Always returns a valid index, must use KeyEqual test after to confirm.
  template <typename KeyType>
  constexpr uint64_t lookup(const KeyType & key) const {
    auto const d = first_table_[hash_(key) % M];
    if (!d.is_seed) { return d.value; }
    else { return second_table_[hash_(key, d.value) % M]; }
  }
};

template <std::size_t M, class Item, std::size_t N, class Hash, class Key>
pmh_tables<M, Hash> constexpr make_pmh_tables(const carray<Item, N> &
                                                               items,
                                                           Hash const &hash,
                                                           Key const &key) {
  // Step 1: Place all of the keys into buckets
  carray<bucket<M>, M> buckets;

  for (std::size_t i = 0; i < N; ++i)
    buckets[hash(key(items[i])) % M].push_back(i);

  // Step 2: Sort the buckets to process the ones with the most items first.
  bits::quicksort(buckets.begin(), buckets.end() - 1, bucket_size_compare{});

  // G becomes the first hash table in the resulting pmh function
  carray<seed_or_index, M> G;

  // values becomes the second hash table in the resulting pmh function
  constexpr uint64_t UNUSED = -1;
  carray<uint64_t, M> values;
  for (auto & x : values) { x = UNUSED; }

  // Step 3: Map the items in buckets into hash tables.
  // - If bucket is singleton, store index of item in G
  // - Otherwise, find a seed that hashes all items in bucket to unused slots.
  //   Store that seed in G, and store indices of those items in their slots.
  for (const auto & bucket : buckets) {
    auto const bsize = bucket.size();

    if (bsize == 1) {
      G[hash(key(items[bucket[0]])) % M] = { false, bucket[0]};
    } else if (bsize > 1) {

      // Repeatedly try different values of d until we find a hash function
      // that places all items in the bucket into free slots
      std::size_t d = 1;
      cvector<std::size_t, M> slots;

      while (slots.size() < bsize) {
        auto slot = hash(key(items[bucket[slots.size()]]), d) % M;

        if (values[slot] != UNUSED || !all_different_from(slots, slot)) {
          slots.clear();
          d++;
          continue;
        }

        slots.push_back(slot);
      }

      G[hash(key(items[bucket[0]])) % M] = { true, d };
      for (std::size_t i = 0; i < bsize; ++i)
        values[slots[i]] = bucket[i];
    }
  }

  // Any unused entries in the values table have to get changed to zero.
  // This is because hashing should not fail or return an out-of-bounds entry.
  // A lookup fails after we apply user-supplied KeyEqual to the query and the
  // key found by hashing.
  for (std::size_t i = 0; i < M; ++i)
    if (values[i] == UNUSED)
      values[i] = 0;

  return {G, values, hash};
}

} // namespace bits

} // namespace frozen

#endif
