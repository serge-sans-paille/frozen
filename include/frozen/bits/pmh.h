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

#include <array>
#include <tuple>

namespace frozen {

namespace bits {

template <class T, size_t N>
constexpr auto bucket_size(std::array<T, N> const &bucket) {
  std::size_t i = 0;
  auto ptr = &std::get<0>(bucket);
  while (i < N && ptr[i])
    ++i;
  return i;
}

struct bucket_size_compare {
  template <class T, size_t N>
  auto constexpr operator()(std::array<T, N> const &b0,
                            std::array<T, N> const &b1) const {
    return bucket_size(b0) > bucket_size(b1);
  }
};

template <class Item, std::size_t N, std::size_t M, class Hash, class Key>
std::tuple<
    std::array<Item, N>, std::array<int64_t, M>,
    std::array<uint64_t, M>> constexpr make_array_of_items(std::array<Item, N>
                                                               items,
                                                           Hash const &hash,
                                                           Key const &key) {
  // Step 1: Place all of the keys into buckets
  std::array<std::array<std::size_t, M>, M> tbuckets{{{{0}}}};
  std::array<uint64_t, M> values{{0}};
  std::array<int64_t, M> G{{0}};

  auto *g = &std::get<0>(G);
  auto *v = &std::get<0>(values);
  auto *tb = &std::get<0>(tbuckets);
  auto *it = &std::get<0>(items);

  for (std::size_t i = 0; i < N; ++i) {
    auto const h = hash(key(it[i])) % M;
    std::size_t j = 0;
    auto *tbh = &std::get<0>(tb[h]);
    while (tbh[j])
      ++j;
    tbh[j] = 1 + i;
  }

  auto buckets = bits::quicksort(tbuckets, bucket_size_compare{});

  // Step 2: Sort the buckets and process the ones with the most items first.
  std::size_t b = 0;
  for (; b < M; ++b) {
    auto &bucket = (&std::get<0>(buckets))[b];
    auto const bsize = bucket_size(bucket);
    if (bsize <= 1)
      break;

    auto *bt = &std::get<0>(bucket);

    // Repeatedly try different values of d until we find a hash function
    // that places all items in the bucket into free slots
    std::size_t d = 1;
    std::size_t item = 0;
    std::size_t slots[M] = {0};

    while (item < bsize) {
      auto slot = hash(key(it[bt[item] - 1]), d) % M;
      if (v[slot] != 0) {
        d += 1;
        for (std::size_t j = 0; j < item; ++j)
          slots[j] = 0;
        item = 0;
        continue;
      }
      bool all_different = true;
      for (std::size_t k = 0; k < item; ++k) {
        if (slots[k] == slot) {
          all_different = false;
          break;
        }
      }
      if (!all_different) {
        d += 1;
        for (std::size_t j = 0; j < item; ++j)
          slots[j] = 0;
        item = 0;
        continue;
      }

      slots[item] = slot;
      item += 1;
    }

    g[hash(key(it[bt[0] - 1])) % M] = d;
    for (std::size_t i = 0; i < bsize; ++i)
      v[slots[i]] = bt[i];
  }

  // Only buckets with 1 item remain. Process them more quickly by directly
  // placing them into a free slot. Use a negative value of d to indicate
  // this.
  int64_t freelist[M] = {0};
  std::size_t freelist_size = 0;

  for (std::size_t i = 0; i < M; ++i)
    if (v[i] == 0)
      freelist[freelist_size++] = i;

  for (std::size_t i = b; i < M; ++i) {
    auto &bucket = (&std::get<0>(buckets))[i];
    if (bucket_size(bucket) == 0)
      break;
    auto *bt = &std::get<0>(bucket);
    auto slot = freelist[--freelist_size];
    // We subtract one to ensure it's negative even if the zeroeth slot was
    // used.
    g[hash(key(it[bt[0] - 1])) % M] = -slot - 1;
    v[slot] = bt[0];
  }
  for (std::size_t i = 0; i < M; ++i)
    if (v[i])
      v[i] -= 1;

  return {items, G, values};
}

} // namespace bits

} // namespace frozen

#endif
