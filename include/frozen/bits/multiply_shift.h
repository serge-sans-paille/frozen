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

namespace frozen {
namespace bits {

template <class Hash, unsigned num_bits>
struct multiply_shift {
  Hash hash;

  template <typename T>
  constexpr uint64_t operator()(const T & t, uint64_t seed) const {
    return (hash(t) * seed) >> (64 - num_bits);
  }
};

template <unsigned num_bits, class Hash>
constexpr multiply_shift<Hash, num_bits> adapt_hash(Hash h) {
  return {h};
}

} // end namespace bits
} // end namespace frozen

#endif
