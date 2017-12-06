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
#include <frozen/bits/multiply_shift.h>
#include <frozen/bits/pmh.h>
#include <frozen/bits/prg.h>

#include <functional>
#include <tuple>
#include <utility>

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
      bits::next_highest_power_of_two(N) * (N < 32 ? 2 : 1); // size adjustment to prevent high collision rate for small sets
  static constexpr unsigned hash_bits = bits::log_base_two(storage_size);
  using AdaptedHash = decltype(bits::maybe_adapt_hash<hash_bits, Key>(std::declval<Hash>()));
  using container_type = std::array<Key, N>;
  using tables_type = bits::pmh_tables<storage_size, AdaptedHash>;

  KeyEqual const equal_;
  container_type items_;
  tables_type tables_;

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
  constexpr unordered_set(container_type keys, Hash const &hash,
                          KeyEqual const &equal)
      : equal_{equal}
      , items_(keys)
      , tables_{bits::make_pmh_tables<storage_size>(
            items_, bits::maybe_adapt_hash<hash_bits, Key>(hash), bits::Get{}, bits::LCG{})} {}
  explicit constexpr unordered_set(container_type keys)
      : unordered_set{keys, Hash{}, KeyEqual{}} {}

  constexpr unordered_set(std::initializer_list<Key> list)
      : unordered_set(bits::make_unordered_array<Key, N>(list)) {}

  constexpr unordered_set(std::initializer_list<Key> list, Hash const & hash, KeyEqual const & equal)
      : unordered_set(bits::make_unordered_array<Key, N>(list), hash, equal) {}

  /* iterators */
  const_iterator begin() const { return items_.begin(); }
  const_iterator end() const { return items_.end(); }
  const_iterator cbegin() const { return items_.cbegin(); }
  const_iterator cend() const { return items_.cend(); }

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
      return items_.end();
  }

  std::pair<const_iterator, const_iterator> equal_range(Key const &key) const {
    auto const &k = lookup(key);
    if (equal_(k, key))
      return {&k, &k + 1};
    else
      return {items_.end(), items_.end()};
  }

  /* bucket interface */
  constexpr std::size_t bucket_count() const { return storage_size; }
  constexpr std::size_t max_bucket_count() const { return storage_size; }

  /* observers*/
  constexpr hasher hash_function() const { return tables_.hash_; }
  constexpr key_equal key_eq() const { return equal_; }

private:
  constexpr auto const &lookup(Key const &key) const {
    return items_[tables_.lookup(key)];
  }
};

template <typename T, std::size_t N>
constexpr auto make_unordered_set(T const (&keys)[N]) {
  return unordered_set<T, N>{bits::to_array(keys)};
}

} // namespace frozen

#endif
