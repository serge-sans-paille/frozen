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

#ifndef FROZEN_LETITGO_MULTIPLY_SHIFT_H
#define FROZEN_LETITGO_MULTIPLY_SHIFT_H

#include <cstdint>
#include <type_traits>
#include <utility>

namespace frozen {
namespace bits {

// Take an unseeded hash, and make it seeded using a 64 bit multiply-shift
template <class Hash, unsigned num_bits>
struct multiply_shift : Hash {
  constexpr multiply_shift(Hash h) : Hash(h) {}

  template <typename T>
  constexpr uint64_t operator()(const T & t, uint64_t seed) const {
    const Hash & hash = *this;
    return (hash(t) * seed) >> (64 - num_bits);
  }
};

// Detection idiom to detect if a hash object supports seeds
template <class... Ts>
using void_t = void;

template <class Hasher, class Key, class=void>
struct supports_seeds : std::false_type{};

template <class Hasher, class Key>
struct supports_seeds<Hasher, Key, void_t<decltype(std::declval<Hasher>()(std::declval<Key>(), uint64_t{}))>>
: std::true_type{};

// Adapt a hash only if necessary
template <unsigned num_bits, class Key, class Hash, std::enable_if_t<supports_seeds<Hash, Key>::value>* = nullptr>
constexpr Hash maybe_adapt_hash(Hash h) {
  return h;
}

template <unsigned num_bits, class Key, class Hash, std::enable_if_t<!supports_seeds<Hash, Key>::value>* = nullptr>
constexpr multiply_shift<Hash, num_bits> maybe_adapt_hash(Hash h) {
  return {h};
}

} // end namespace bits
} // end namespace frozen

#endif
