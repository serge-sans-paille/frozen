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
#include <tuple>
#include <limits>

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
  // the order is not guaranteed by the standard,
  // *BUT* we don't care as we sort it later
  return {{((void)I, *iter++)...}};
}

template <class T, std::size_t N>
constexpr std::array<T, N>
make_unordered_array(std::initializer_list<T> const values) {
  auto iter = values.begin();
  return make_unordered_array<T, N>(iter, std::make_index_sequence<N>{});
}

template <typename Iter, typename Compare>
constexpr auto min_element(Iter begin, const Iter &end,
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

template <typename T, std::size_t N, class Compare>
constexpr auto sort(std::array<T, N> const &data, Compare const &compare) {
  // get copy of incoming data
  auto retval = data;

  // Use std::get to get a non-const * to retval data
  const auto array_head = &std::get<0>(retval);
  const auto end = array_head + N;

  // Loop over each position, finding the appropriate element,
  // and swapping it into place
  for (std::size_t i = 0; i < N; ++i) {
    const auto begin = array_head + i;

    // Treat pointers as iterators, perfectly valid
    auto minelem = ::frozen::bits::min_element(begin, end, compare);

    // swap in the newly found minimum element
    const auto tmp = *minelem;
    *minelem = *begin;
    *begin = tmp;
  }

  return retval;
}
template <typename K, typename V, std::size_t N, class Compare>
constexpr auto sort(std::array<std::tuple<K, V>, N> const &data,
                    Compare const &compare) {
  // get copy of incoming data
  auto retval = data;

  // Use std::get to get a non-const * to retval data
  const auto array_head = &std::get<0>(retval);
  const auto end = array_head + N;

  // Loop over each position, finding the appropriate element,
  // and swapping it into place
  for (std::size_t i = 0; i < N; ++i) {
    const auto begin = array_head + i;

    // Treat pointers as iterators, perfectly valid
    auto minelem = ::frozen::bits::min_element(begin, end, compare);

    // swap in the newly found minimum element
    const auto tmp = *minelem;
    std::get<0>(*minelem) = std::get<0>(*begin);
    std::get<1>(*minelem) = std::get<1>(*begin);
    std::get<0>(*begin) = std::get<0>(tmp);
    std::get<1>(*begin) = std::get<1>(tmp);
  }

  return retval;
}
template <class T> constexpr void cswap(T &a, T &b) {
  auto tmp = a;
  a = b;
  b = tmp;
}
template <class T0, class T1>
constexpr void cswap(std::tuple<T0, T1> &a, std::tuple<T0, T1> &b) {
  std::tuple<T0, T1> t{std::get<0>(a), std::get<1>(a)};
  std::get<0>(a) = std::get<0>(b);
  std::get<1>(a) = std::get<1>(b);
  std::get<0>(b) = std::get<0>(t);
  std::get<1>(b) = std::get<1>(t);
}

template <typename T, std::size_t N, class Compare>
constexpr std::size_t partition(std::array<T, N> &array, std::size_t left,
                                std::size_t right, Compare const &compare) {
  auto pivot = left + (right - left) / 2;
  auto *arr = &std::get<0>(array);
  T value = arr[pivot];
  cswap(arr[right], arr[pivot]);
  std::size_t store = left;
  for (size_t i = left; i < right; ++i) {
    if (compare(arr[i], value)) {
      cswap(arr[i], arr[store]);
      store++;
    }
  }
  cswap(arr[right], arr[store]);
  return store;
}

template <typename T, std::size_t N, class Compare>
constexpr void quicksort(std::array<T, N> &array, std::size_t left,
                         std::size_t right, Compare const &compare) {
  if (left < right) {
    std::size_t new_pivot = partition(array, left, right, compare);
    quicksort(array, left, new_pivot, compare);
    quicksort(array, new_pivot + 1, right, compare);
  }
}

template <typename T, std::size_t N, class Compare>
constexpr std::array<T, N> quicksort(std::array<T, N> const &array,
                                     Compare const &compare) {
  std::array<T, N> res = array;
  quicksort(res, 0, N - 1, compare);
  return res;
}

template <class T, class Compare> struct LowerBound {
  T const &value;
  Compare const &compare;
  constexpr LowerBound(T const &value, Compare const &compare)
      : value(value), compare(compare) {}

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
    if (compare(*it, value)) {
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
