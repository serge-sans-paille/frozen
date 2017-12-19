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
#include <string.h>

#include "frozen/bits/basic_types.h"
#include "frozen/bits/elsa.h"

namespace frozen {

class string {
  char const *data_;
  std::size_t size_;

public:
  constexpr string(char const *data, std::size_t size)
      : data_(data), size_(size) {}

  string(std::string const &s) : data_(s.data()), size_(s.size()) {}

  constexpr string(const string &) noexcept = default;
  constexpr string &operator=(const string &) noexcept = default;

  constexpr std::size_t size() const { return size_; }

  constexpr char operator[](std::size_t i) const { return data_[i]; }

  constexpr bool operator==(string other) const {
    if (size_ != other.size_)
      return false;
    for (std::size_t i = 0; i < size_; ++i)
      if (data_[i] != other.data_[i])
        return false;
    return true;
  }

  constexpr bool operator<(const string &other) const {
    unsigned i = 0;
    while (i < size() && i < other.size()) {
      if ((*this)[i] < other[i]) {
        return true;
      }
      if ((*this)[i] > other[i]) {
        return false;
      }
      ++i;
    }
    return size() < other.size();
  }

  constexpr const char *data() const { return data_; }
};

template <> struct elsa<string> {
  constexpr std::size_t operator()(string value) const {
    std::size_t d = 5381;
    for (std::size_t i = 0; i < value.size(); ++i)
      d = d * 33 + value[i];
    return d;
  }
  constexpr std::size_t operator()(string value, std::size_t seed) const {
    std::size_t d = seed;
    for (std::size_t i = 0; i < value.size(); ++i)
      d = (d * 0x01000193) ^ value[i];
    return d;
  }
};

namespace kmp {

template <std::size_t size> class string_needle {
  bits::cvector<std::ptrdiff_t, size> cache_;
  std::array<char, size> data_;

  static constexpr bits::cvector<std::ptrdiff_t, size>
  build_kmp_cache(std::array<char, size> needle) {
    char const *needle_view = needle.data();
    std::ptrdiff_t cnd = 0;
    bits::cvector<std::ptrdiff_t, size> cache;
    for (std::size_t pos = 0; pos < size; ++pos)
      cache[pos] = -1;

    for (std::size_t pos = 1; pos < size; ++pos) {
      if (needle_view[pos] == needle_view[cnd]) {
        cache[pos] = cache[cnd];
        cnd += 1;
      } else {
        cache[pos] = cnd;
        cnd = cache[cnd];
        while (cnd >= 0 && needle_view[pos] != needle_view[cnd])
          cnd = cache[cnd];
        cnd += 1;
      }
    }
    return cache;
  }

public:
  constexpr string_needle(std::array<char, size> data)
      : cache_{build_kmp_cache(data)}, data_(data) {}

  template <std::size_t... I>
  constexpr string_needle(char const data[size], std::index_sequence<I...>)
      : string_needle(std::array<char, size>{{data[I]...}}) {}
  constexpr string_needle(char const data[size])
      : string_needle(data, std::make_index_sequence<size>()) {}

  constexpr char operator[](std::size_t i) const {
    return (&std::get<0>(data_))[i];
  }
  constexpr std::ptrdiff_t step(std::size_t i) const { return cache_[i]; }
};

template <std::size_t N>
constexpr string_needle<N - 1> make_needle(char const (&data)[N]) {
  return {data};
}

} // namespace kmp

template <class IteratorTy, std::size_t N>
IteratorTy search(IteratorTy begin, IteratorTy end,
                  kmp::string_needle<N> const &needle) {
  std::size_t i = 0;
  IteratorTy iter = begin;
  while (iter != end) {
    if (needle[i] == *iter) {
      if (i == (N - 1))
        return iter - i;
      ++i;
      ++iter;
    } else {
      if (needle.step(i) > -1) {
        i = needle.step(i);
      } else {
        ++iter;
        i = 0;
      }
    }
  }
  return end;
}

namespace bm {

// text book implementation from
// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm

template <std::size_t size> class string_needle {
  using skip_table_type = std::array<char, sizeof(char) << 8>;
  skip_table_type skip_table_;

  using suffix_table_type = std::array<std::ptrdiff_t, size>;
  suffix_table_type suffix_table_;

  std::array<char, size> data_;

  constexpr auto build_skip_table(std::array<char, size> const &needle) {
    skip_table_type skip_table{{0}};
    skip_table.fill(size);
    for (std::size_t i = 0; i < size - 1; ++i)
      skip_table[needle[i]] -= i + 1;
    return skip_table;
  }

  constexpr bool is_prefix(std::array<char, size> const &needle,
                           std::size_t pos) {
    return strncmp(needle.data(), needle.data() + pos, size - pos) == 0;
  }

  constexpr std::size_t suffix_length(std::array<char, size> const &needle,
                                      std::size_t pos) {
    // increment suffix length slen to the first mismatch or beginning
    // of the word
    for (std::size_t slen = 0; slen < pos ; slen++)
      if (needle[pos - slen] != needle[size - 1 - slen])
        return slen;

    return pos;
  }

  constexpr auto build_suffix_table(std::array<char, size> const &needle) {
    suffix_table_type suffix{{0}};
    std::ptrdiff_t last_prefix_index = size - 1;

    // first loop
    for (std::ptrdiff_t p = size - 1; p >= 0; p--) {
      if (is_prefix(needle, p + 1))
        last_prefix_index = p + 1;

      suffix[p] = last_prefix_index + (size - 1 - p);
    }

    // second loop
    for (std::size_t p = 0; p < size - 1; p++) {
      auto slen = suffix_length(needle, p);
      if (needle[p - slen] != needle[size - 1 - slen])
        suffix[size - 1 - slen] = size - 1 - p + slen;

    }
    return suffix;
  }

public:
  constexpr string_needle(std::array<char, size> const &data)
      : skip_table_{build_skip_table(data)},
        suffix_table_{build_suffix_table(data)},
        data_(data) {}

  template <std::size_t... I>
  constexpr string_needle(char const data[size], std::index_sequence<I...>)
      : string_needle(std::array<char, size>{{data[I]...}}) {}
  constexpr string_needle(char const data[size])
      : string_needle(data, std::make_index_sequence<size>()) {}

  constexpr char operator[](std::size_t i) const {
    return (&std::get<0>(data_))[i];
  }
  constexpr auto const &skip() const { return skip_table_; }
  constexpr auto const &suffix() const { return suffix_table_; }
};

template <std::size_t N>
constexpr string_needle<N - 1> make_needle(char const (&data)[N]) {
  return {data};
}

} // namespace bm

template <class IteratorTy, std::size_t N>
IteratorTy search(IteratorTy begin, IteratorTy end,
                  bm::string_needle<N> const &needle) {
  if (N == 0)
    return begin;

  IteratorTy iter = begin + N - 1;
  while (iter < end) {
    std::ptrdiff_t j = N - 1;
    while (j > 0 && (*iter == needle[j])) {
      --iter;
      --j;
    }
    if (*iter == needle[0])
      return iter;

    auto skip = needle.skip()[*iter];
    auto suffix = needle.suffix()[j];
    std::size_t step = skip > suffix ? skip : suffix;
    iter += step;
  }
  return end;
}

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
