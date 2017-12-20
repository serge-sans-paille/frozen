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

#ifndef FROZEN_LETITGO_BASIC_TYPES_H
#define FROZEN_LETITGO_BASIC_TYPES_H

#include <array>

namespace frozen {

namespace bits {

template <class T, std::size_t N>
class cvector {
  T data [N] = {}; // zero-initialization for scalar type T, default-initialized otherwise
  std::size_t dsize = 0;

public:
  // Container typdefs
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // Constructors
  constexpr cvector(void) = default;
  constexpr cvector(size_type count, const T& value) : dsize(count) {
    for (std::size_t i = 0; i < N; ++i)
      data[i] = value;
  }

  // Iterators
  constexpr iterator begin() noexcept { return data; }
  constexpr iterator end() noexcept { return data + dsize; }

  // Capacity
  constexpr size_type size() const { return dsize; }

  // Element access
  constexpr       reference operator[](std::size_t index) { return data[index]; }
  constexpr const_reference operator[](std::size_t index) const { return data[index]; }

  constexpr       reference back() { return data[dsize - 1]; }
  constexpr const_reference back() const { return data[dsize - 1]; }

  // Modifiers
  constexpr void push_back(const T & a) { data[dsize++] = a; }
  constexpr void push_back(T && a) { data[dsize++] = std::move(a); }
  constexpr void pop_back() { --dsize; }

  constexpr void clear() { dsize = 0; }

  // Convert to std::array
  constexpr std::array<T, N> to_array() {
    std::array<T, N> a{};
    auto * ta = &std::get<0>(a);

    for(std::size_t i = 0; i < N; i++)
        ta[i] = data[i];

    return a;
  }
};


} // namespace bits

} // namespace frozen

#endif
