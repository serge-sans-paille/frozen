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

#ifndef FROZEN_LETITGO_ELSA_H
#define FROZEN_LETITGO_ELSA_H

namespace frozen {

template <class T> struct elsa {
  static_assert(std::is_integral<T>::value,
                "only supports integral types, specialize for other types");

  constexpr std::size_t operator()(T const &value) const { return value; }

  constexpr std::size_t operator()(T const &value, std::size_t seed) const {
    // murmur3 finalizer, see https://github.com/aappleby/smhasher/wiki/MurmurHash3
    std::size_t key = value ^ seed;
    key ^= (key >> 33);
    key *= 0xff51afd7ed558ccd;
    key ^= (key >> 33);
    key *= 0xc4ceb9fe1a85ec53;
    key ^= (key >> 33);
    return key;
  }
};

template <class T> using anna = elsa<T>;
} // namespace frozen

#endif
