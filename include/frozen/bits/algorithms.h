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

#ifndef FROZEN_LETITGO_ALGORITHMS_H
#define FROZEN_LETITGO_ALGORITHMS_H

#include <array>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

namespace frozen {

namespace bits {

auto constexpr next_highest_power_of_two(std::size_t v) {
  // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  constexpr auto trip_count = std::numeric_limits<decltype(v)>::digits;
  v--;
  for(std::size_t i = 1; i < trip_count; i <<= 1)
    v |= v >> i;
  v++;
  return v;
}

template <class T, std::size_t N, class Iter, std::size_t... I>
constexpr std::array<T, N> make_unordered_array(Iter &iter,
                                                std::index_sequence<I...>) {
  return {{((void)I, *iter++)...}};
}

template <class T, std::size_t N>
constexpr std::array<T, N>
make_unordered_array(std::initializer_list<T> const values) {
  auto iter = values.begin();
  return make_unordered_array<T, N>(iter, std::make_index_sequence<N>{});
}

// This is std::experimental::to_array
// http://en.cppreference.com/w/cpp/experimental/to_array
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
    to_array_impl(T (&a)[N], std::index_sequence<I...>)
{
  return { {a[I]...} };
}

template <class T, std::size_t N>
constexpr auto to_array(T (&a)[N]) -> std::array<std::remove_cv_t<T>, N>
{
  return to_array_impl(a, std::make_index_sequence<N>{});
}

template <typename Iter, typename Compare>
constexpr auto min_element(Iter begin, const Iter end,
                           Compare const &compare) {
  auto result = begin;
  while (begin != end) {
    if (compare(*begin, *result)) {
      result = begin;
    }
    ++begin;
  }
  return result;
}

template <class T>
constexpr void cswap(T &a, T &b) {
  auto tmp = a;
  a = b;
  b = tmp;
}

template <class... Tys, std::size_t... Is> 
constexpr void cswap(std::tuple<Tys...> &a, std::tuple<Tys...> &b, std::index_sequence<Is...>) {
  using swallow = int[];
  (void) swallow{(cswap(std::get<Is>(a), std::get<Is>(b)), 0)...};
}

template <class... Tys>
constexpr void cswap(std::tuple<Tys...> &a, std::tuple<Tys...> &b) {
  cswap(a, b, std::make_index_sequence<sizeof...(Tys)>());
}

template <typename Iterator, class Compare>
constexpr Iterator partition(Iterator left, Iterator right, Compare const &compare) {
  auto pivot = left + (right - left) / 2;
  auto value = *pivot;
  cswap(*right, *pivot);
  for (auto it = left; it < right; ++it) {
    if (compare(*it, value)) {
      cswap(*it, *left);
      left++;
    }
  }
  cswap(*right, *left);
  return left;
}

template <typename Iterator, class Compare>
constexpr void quicksort(Iterator left, Iterator right, Compare const &compare) {
  if (left < right) {
    auto new_pivot = bits::partition(left, right, compare);
    quicksort(left, new_pivot, compare);
    quicksort(new_pivot + 1, right, compare);
  }
}

template <typename T, std::size_t N, class Compare>
constexpr std::array<T, N> quicksort(std::array<T, N> const &array,
                                     Compare const &compare) {
  std::array<T, N> res = array;
  quicksort(&std::get<0>(res), &std::get<N - 1>(res), compare);
  return res;
}

template <class T, class Compare> struct LowerBound {
  T const &value_;
  Compare const &compare_;
  constexpr LowerBound(T const &value, Compare const &compare)
      : value_(value), compare_(compare) {}

  template <class ForwardIt>
  inline constexpr ForwardIt doit(ForwardIt first,
                                  std::integral_constant<std::size_t, 0>) {
    return first;
  }

  template <class ForwardIt, std::size_t N>
  inline constexpr ForwardIt doit(ForwardIt first,
                                  std::integral_constant<std::size_t, N>) {
    auto constexpr step = N / 2;
    auto it = first + step;
    if (compare_(*it, value_)) {
      auto constexpr next_count = N - (step + 1);
      return doit(it + 1, std::integral_constant<std::size_t, next_count>{});
    } else {
      auto constexpr next_count = step;
      return doit(first, std::integral_constant<std::size_t, next_count>{});
    }
  }
};

template <std::size_t N, class ForwardIt, class T, class Compare>
constexpr ForwardIt lower_bound(ForwardIt first, const T &value,
                                Compare const &compare) {
  return LowerBound<T, Compare>{value, compare}.doit(
      first, std::integral_constant<std::size_t, N>{});
}

template <std::size_t N, class Compare, class ForwardIt, class T>
constexpr bool binary_search(ForwardIt first, const T &value,
                             Compare const &compare) {
  ForwardIt where = lower_bound<N>(first, value, compare);
  return (!(where == first + N) && !(compare(value, *where)));
}

} // namespace bits
} // namespace frozen

#endif
