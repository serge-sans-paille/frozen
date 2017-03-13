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

#ifndef FROZEN_LETITGO_UNORDERED_SET_H
#define FROZEN_LETITGO_UNORDERED_SET_H
#include <array>
#include <frozen/bits/elsa.h>
#include <frozen/bits/pmh.h>

#include <functional>
#include <tuple>

namespace frozen {

namespace bits {

struct Get {
  template <class T> constexpr T const &operator()(T const &key) const {
    return key;
  }
};

} // namespace bits

template <class Key, std::size_t N, typename Hash = elsa<Key>,
          class KeyEqual = std::equal_to<Key>>
class unordered_set {
  static constexpr std::size_t storage_size =
      bits::next_highest_power_of_two(N);
  using container_type = std::array<Key, N>;

  Hash const hash_;
  KeyEqual const equal_;
  std::tuple<container_type, std::array<int64_t, storage_size>,
             std::array<uint64_t, storage_size>>
      keys_;

public:
  /* typedefs */
  using key_type = Key;
  using value_type = Key;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using const_reference = typename container_type::const_reference;
  using reference = const_reference;
  using const_pointer = typename container_type::const_pointer;
  using pointer = const_pointer;
  using const_iterator = const_pointer;
  using iterator = const_iterator;

public:
  /* constructors */
  unordered_set(unordered_set const &) = default;
  constexpr unordered_set(std::initializer_list<Key> keys, Hash const &hash,
                          KeyEqual const &equal)
      : hash_{hash}, equal_{equal},
        keys_{bits::make_array_of_items<Key, N, storage_size>(
            bits::make_unordered_array<Key, N>(keys), hash_, bits::Get{})} {}
  constexpr unordered_set(std::initializer_list<Key> keys)
      : unordered_set{keys, Hash{}, KeyEqual{}} {}

  /* iterators */
  const_iterator begin() const { return std::get<0>(keys_).begin(); }
  const_iterator end() const { return std::get<0>(keys_).end(); }
  const_iterator cbegin() const { return std::get<0>(keys_).cbegin(); }
  const_iterator cend() const { return std::get<0>(keys_).cend(); }

  /* capacity */
  constexpr bool empty() const { return !N; }
  constexpr size_type size() const { return N; }
  constexpr size_type max_size() const { return N; }

  /* lookup */
  constexpr std::size_t count(Key const &key) const {
    auto const k = lookup(key);
    return equal_(k, key);
  }
  const_iterator find(Key const &key) const {
    auto const &k = lookup(key);
    if (equal_(k, key))
      return &k;
    else
      return std::get<0>(keys_).end();
  }

  std::pair<const_iterator, const_iterator> equal_range(Key const &key) const {
    auto const &k = lookup(key);
    if (equal_(k, key))
      return {&k, &k + 1};
    else
      return {std::get<0>(keys_).end(), std::get<0>(keys_).end()};
  }

  /* bucket interface */
  constexpr std::size_t bucket_count() const { return storage_size; }
  constexpr std::size_t max_bucket_count() const { return storage_size; }

  /* observers*/
  constexpr hasher hash_function() const { return hash_; }
  constexpr key_equal key_eq() const { return equal_; }

private:
  constexpr auto const &lookup(Key const &key) const {
    auto const d = std::get<1>(keys_)[hash_(key) % storage_size];
    auto const index = (d < 0) ? (-d - 1) : (hash_(key, d) % storage_size);
    auto kindex = std::get<2>(keys_)[index];
    auto const &k = std::get<0>(keys_)[kindex];
    return k;
  }
};
} // namespace frozen

#endif
