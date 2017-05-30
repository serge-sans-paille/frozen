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

#ifndef FROZEN_SET_H
#define FROZEN_SET_H

#include "bits/algorithms.h"
#include <array>

namespace frozen {

template <class Key, std::size_t N, class Compare = std::less<Key>> class set {
  using container_type = std::array<Key, N>;
  Compare const compare_;
  container_type const keys_;

public:
  /* container typedefs*/
  using key_type = Key;
  using value_type = Key;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::size_type;
  using key_compare = Compare;
  using value_compare = Compare;
  using reference = typename container_type::const_reference;
  using const_reference = reference;
  using pointer = typename container_type::const_pointer;
  using const_pointer = pointer;
  using iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::const_reverse_iterator;
  using const_iterator = iterator;
  using const_reverse_iterator = reverse_iterator;

public:
  /* constructors */
  constexpr set(const set &other) = default;
  constexpr set(std::initializer_list<Key> keys, Compare const &comp)
      : compare_{comp}, keys_{bits::quicksort(
                            bits::make_unordered_array<Key, N>(keys),
                            compare_)} {}
  constexpr set(std::initializer_list<Key> keys) : set{keys, Compare{}} {}

  /* capacity */
  constexpr bool empty() const { return !N; }
  constexpr size_type size() const { return N; }
  constexpr size_type max_size() const { return N; }

  /* lookup */
  constexpr std::size_t count(Key const &key) const {
    return bits::binary_search<N>(&std::get<0>(keys_), key, compare_);
  }

  const_iterator find(Key const &key) const {
    const_iterator where = lower_bound(key);
    if ((where != end()) && !compare_(key, *where))
      return where;
    else
      return end();
  }

  std::pair<const_iterator, const_iterator> equal_range(Key const &key) const {
    auto const lower = lower_bound(key);
    if (lower == end())
      return {lower, lower};
    else
      return {lower, lower + 1};
  }

  const_iterator lower_bound(Key const &key) const {
    auto const where = bits::lower_bound<N>(&std::get<0>(keys_), key, compare_);
    if ((where != end()) && !compare_(key, *where))
      return where;
    else
      return end();
  }

  const_iterator upper_bound(Key const &key) const {
    auto const where = bits::lower_bound<N>(&std::get<0>(keys_), key, compare_);
    if ((where != end()) && !compare_(key, *where))
      return where + 1;
    else
      return end();
  }

  /* observers */
  constexpr key_compare key_comp() const { return compare_; }
  constexpr key_compare value_comp() const { return compare_; }

  /* iterators */
  const_iterator begin() const { return keys_.begin(); }
  const_iterator cbegin() const { return keys_.cbegin(); }
  const_iterator end() const { return keys_.end(); }
  const_iterator cend() const { return keys_.cend(); }

  const_reverse_iterator rbegin() const { return keys_.rbegin(); }
  const_reverse_iterator crbegin() const { return keys_.crbegin(); }
  const_reverse_iterator rend() const { return keys_.rend(); }
  const_reverse_iterator crend() const { return keys_.crend(); }
};

template <class Key, class Compare> class set<Key, 0, Compare> {
  using container_type = std::array<Key, 1>; // just for the type definitions
  Compare const compare_;

public:
  /* container typedefs*/
  using key_type = Key;
  using value_type = Key;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::size_type;
  using key_compare = Compare;
  using value_compare = Compare;
  using reference = typename container_type::const_reference;
  using const_reference = reference;
  using pointer = typename container_type::const_pointer;
  using const_pointer = pointer;
  using iterator = pointer;
  using reverse_iterator = pointer;
  using const_iterator = const_pointer;
  using const_reverse_iterator = const_pointer;

public:
  /* constructors */
  constexpr set(const set &other) = default;
  constexpr set(std::initializer_list<Key>, Compare const &comp)
      : compare_{comp} {}
  constexpr set(std::initializer_list<Key> keys) : set{keys, Compare{}} {}

  /* capacity */
  constexpr bool empty() const { return true; }
  constexpr size_type size() const { return 0; }
  constexpr size_type max_size() const { return 0; }

  /* lookup */
  constexpr std::size_t count(Key const &) const { return 0; }

  constexpr const_iterator find(Key const &) const { return end(); }

  constexpr std::pair<const_iterator, const_iterator>
  equal_range(Key const &) const { return {end(), end()}; }

  constexpr const_iterator lower_bound(Key const &) const { return end(); }

  constexpr const_iterator upper_bound(Key const &) const { return end(); }

  /* observers */
  constexpr key_compare key_comp() const { return compare_; }
  constexpr key_compare value_comp() const { return compare_; }

  /* iterators */
  constexpr const_iterator begin() const { return nullptr; }
  constexpr const_iterator cbegin() const { return nullptr; }
  constexpr const_iterator end() const { return nullptr; }
  constexpr const_iterator cend() const { return nullptr; }

  constexpr const_reverse_iterator rbegin() const { return nullptr; }
  constexpr const_reverse_iterator crbegin() const { return nullptr; }
  constexpr const_reverse_iterator rend() const { return nullptr; }
  constexpr const_reverse_iterator crend() const { return nullptr; }
};
} // namespace frozen

#endif
