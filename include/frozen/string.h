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

#ifndef FROZEN_LETITGO_STRING_H
#define FROZEN_LETITGO_STRING_H

#include <functional>
#include <string>
#include <utility>

#include "frozen/bits/elsa.h"

namespace frozen {

struct string {
  char const *const data;
  std::size_t const size;
  constexpr string(char const *data, std::size_t size)
      : data(data), size(size) {}
  string(std::string const &s) : data(s.data()), size(s.size()) {}

  constexpr bool operator==(string other) const {
    if (size != other.size)
      return false;
    for (std::size_t i = 0; i < size; ++i)
      if (data[i] != other.data[i])
        return false;
    return true;
  }
};

template <> struct elsa<string> {
  constexpr std::size_t operator()(string value) const {
    std::size_t d = 5381;
    for (std::size_t i = 0; i < value.size; ++i)
      d = d * 33 + value.data[i];
    return d;
  }
  constexpr std::size_t operator()(string value, std::size_t seed) const {
    std::size_t d = seed;
    for (std::size_t i = 0; i < value.size; ++i)
      d = (d * 0x01000193) ^ value.data[i];
    return d;
  }
};

namespace string_literals {

constexpr string operator"" _s(const char *data, std::size_t size) {
  return {data, size};
}
} // namespace string_literals
} // namespace frozen

namespace std {
template <> struct hash<frozen::string> {
  size_t operator()(frozen::string s) const {
    return frozen::elsa<frozen::string>{}(s);
  }
};
} // namespace std

#endif
