/*
 * Frozen
 * Copyright 2022 Giel van Schijndel
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

#ifndef FROZEN_LETITGO_BITS_PIC_ARRAY_H
#define FROZEN_LETITGO_BITS_PIC_ARRAY_H

#include "frozen/bits/constexpr_assert.h"
#include "frozen/bits/defines.h"
#include "frozen/bits/mpl.h"
#include "frozen/string.h"

#include <memory>
#include <type_traits>
#include <utility>

#ifdef FROZEN_LETITGO_HAS_STRING_VIEW
#include <string_view>
#endif

namespace frozen {

namespace bits {

// A span/string_view that refers into an array with an integer index.
// This has the advantage of being memory location independent, with regards to the location of the
// data. As a result no relocations need to be generated. For position-independent-code this ensures
// this doesn't need relocations to happen when loading the executable or shared library containing
// instances of this class. For large data sets this can provide a significant performance
// improvement.
//
// Additionally, because no (dynamic) relocations need to happen, the binary representation is
// guaranteed to be the same in memory as on disk, allowing this to be mapped into memory directly
// from disk. This allows the OS to save the associated RAM (by regarding that RAM as a cache only).
//
// Note that a span/string_view variant with something like Boost's `offset_ptr` would be able to
// achieve the same, in a much easier way. That class, however, relies on undefined behavior *. And
// even in that case: neither GCC nor Clang generate code free of the runtime relocation
// requirement, they generate the code themselves instead of letting the dynamic linker do it. Which
// is even worse.
//
// *: boost::offset_ptr triggers undefined behavior when it subtracts two pointers not part of the
//    same array and again when it subsequently adds the integer result of that to a pointer,
//    producing a pointer outside of a (non-existant) array.
template <typename ElemT>
struct ext_array {
  std::ptrdiff_t data_pos;
  std::size_t data_size;
};

template <typename T, typename ElemT>
constexpr T& make_absolute(T& self, const ElemT* /* base */) noexcept {
  return self;
}

template <typename T, typename ElemT>
constexpr const T& make_absolute(const T& self, const ElemT* /* base */) noexcept {
  return self;
}

template <typename T, typename ElemT>
constexpr T make_absolute(const ext_array<ElemT>& self, const ElemT* base) noexcept {
  return T{base + self.data_pos, self.data_size};
}

template <typename T, typename ElemT, typename U, std::size_t... Is>
constexpr T make_absolute(U& self, const ElemT* base, std::index_sequence<Is...>) noexcept {
  return T{make_absolute<std::tuple_element_t<Is, T>>(std::get<Is>(self), base)...};
}

template <typename T, typename ElemT, typename U, std::enable_if_t<std::tuple_size<T>::value == std::tuple_size<U>::value>* = nullptr>
constexpr T make_absolute(U& self, const ElemT* base) noexcept {
  return make_absolute<T>(self, base, std::make_index_sequence<std::tuple_size<T>::value>());
}

template <typename T>
struct pic_store {
  using type = T;
};

template <typename CharT>
struct pic_store<frozen::basic_string<CharT>> {
  using type = ext_array<CharT>;
};

#if defined(FROZEN_LETITGO_HAS_STRING_VIEW)
template <typename CharT, typename Traits>
struct pic_store<std::basic_string_view<CharT, Traits>> {
  using type = ext_array<CharT>;
};
#endif

template <typename Key, typename Value>
struct pic_store<std::pair<Key, Value>> {
  using type = std::pair<typename pic_store<Key>::type, typename pic_store<Value>::type>;
};

// Helper to convert supported string_view/span-like types to ext_array
template <typename T>
using pic_store_type = typename pic_store<remove_cv_t<T>>::type;

template <typename T>
struct pic_reference {
  using type = T;
};

template <typename CharT>
struct pic_reference<frozen::basic_string<CharT>&> {
  using type = frozen::basic_string<CharT>;
};

template <typename CharT>
struct pic_reference<const frozen::basic_string<CharT>&> {
  using type = const frozen::basic_string<CharT>;
};

#if defined(FROZEN_LETITGO_HAS_STRING_VIEW)
template <typename CharT, typename Traits>
struct pic_reference<std::basic_string_view<CharT, Traits>&> {
  using type = std::basic_string_view<CharT, Traits>;
};

template <typename CharT, typename Traits>
struct pic_reference<const std::basic_string_view<CharT, Traits>&> {
  using type = const std::basic_string_view<CharT, Traits>;
};
#endif

template <typename Key, typename Value>
struct pic_reference<std::pair<Key, Value>&> {
  using type = std::pair<typename pic_reference<Key&>::type, typename pic_reference<Value&>::type>;
};

template <typename Key, typename Value>
struct pic_reference<const std::pair<Key, Value>&> {
  using type = std::pair<typename pic_reference<const Key&>::type, typename pic_reference<const Value&>::type>;
};

// Helper to convert references to supported string_view/span-like types to by-value types to
// allow our proxy iterator to convert back from ext_array. This relies on lifetime extension
// of temporaries to be safe in range-based for loops.
template <typename T>
using pic_reference_type = typename pic_reference<T>::type;

template <typename T>
struct element_type {};

template <typename CharT>
struct element_type<basic_string<CharT>> {
  using type = CharT;
};

template <typename CharT>
struct element_type<std::pair<basic_string<CharT>, basic_string<CharT>>> {
  using type = CharT;
};

template <typename CharT, typename U>
struct element_type<std::pair<basic_string<CharT>, U>> {
  using type = CharT;
};

template <typename CharT, typename U>
struct element_type<std::pair<U, basic_string<CharT>>> {
  using type = CharT;
};

#if defined(FROZEN_LETITGO_HAS_STRING_VIEW)
template <typename CharT, typename Traits>
struct element_type<std::basic_string_view<CharT, Traits>> {
  using type = CharT;
};

template <typename CharT, typename TraitsL, typename TraitsR>
struct element_type<std::pair<std::basic_string_view<CharT, TraitsL>, std::basic_string_view<CharT, TraitsR>>> {
  using type = CharT;
};

template <typename CharT, typename Traits>
struct element_type<std::pair<basic_string<CharT>, std::basic_string_view<CharT, Traits>>> {
  using type = CharT;
};

template <typename CharT, typename Traits>
struct element_type<std::pair<std::basic_string_view<CharT, Traits>, basic_string<CharT>>> {
  using type = CharT;
};

template <typename CharT, typename Traits, typename U>
struct element_type<std::pair<std::basic_string_view<CharT, Traits>, U>> {
  using type = CharT;
};

template <typename CharT, typename Traits, typename U>
struct element_type<std::pair<U, std::basic_string_view<CharT, Traits>>> {
  using type = CharT;
};
#endif

// Helper to determine type of element we need to store in our storage array.
template <typename T>
using element_t = typename element_type<remove_cv_t<T>>::type;

template <class Reference>
struct arrow_operator_proxy {
  constexpr Reference* operator->() noexcept {
#if FROZEN_LETITGO_HAS_CXX17
    return std::addressof(reference_);
#else
    return &reference_;
#endif
  }

  constexpr operator Reference*() {
    // For compilers that fail to implement this requirement of the C++ standard:
    // > When `operator->` returns, the operator `->` is applied to the value returned, with the
    // > original second operand.
    //
    // I.e. this can lead to recursion of applying `operator->` until a raw pointer is returned.
    // Some compilers however fail to perform this recursion and expect the top-level call to
    // `operator->` to return a raw pointer.
    return this->operator->();
  }

  Reference reference_;
};

// Proxy iterator that, upon dereference, converts ext_array to value_type (string_view/span-like
// type).
template <typename T>
struct pic_iterator {
  // We're lying here, because 'reference' doesn't satisfy this requirement for ForwardIterator:
  // > ..., `reference` is a reference to `T`; ...
  // But in practice this doesn't matter as most STL implementations support proxy random access
  // iterators and select better algorithms for them. This is because std::vector<bool>::iterator is
  // also a proxy iterator claiming to be a RandomAccessIterator.
  // In C++20's ranges terminology: we're a random access iterator, but *not* a contiguous one.
  using iterator_category       = std::random_access_iterator_tag;
  using difference_type         = std::ptrdiff_t;
  using value_type              = std::remove_const_t<T>;
  using storage_type            = std::conditional_t<
    std::is_const<T>::value,
    const pic_store_type<value_type>,
    pic_store_type<value_type>
  >;
  using reference               = pic_reference_type<T&>;
  // A pointer doesn't make sense here, but it's necessary to satisfy std::iterator_traits.
  using pointer                 = std::conditional_t<
    std::is_const<T>::value,
    const void,
    void
  >;

  constexpr pic_iterator() noexcept
    : array_data_{nullptr}
    , pos_{nullptr}
  {}

  constexpr pic_iterator(const element_t<value_type>* array_data, storage_type* pos) noexcept
    : array_data_{array_data}
    , pos_{pos}
  {}

  constexpr reference operator*() const noexcept {
    return make_absolute<reference>(*pos_, array_data_);
  }

  constexpr auto operator->() const noexcept {
    return arrow_operator_proxy<reference>{**this};
  }

  constexpr pic_iterator& operator+=(difference_type n) noexcept {
    pos_ += n;
    return *this;
  }

  friend constexpr pic_iterator operator+(const pic_iterator& lhs, difference_type n) noexcept {
    auto r = lhs;
    r += n;
    return r;
  }

  friend constexpr pic_iterator operator+(difference_type n, const pic_iterator& rhs) noexcept {
    auto r = rhs;
    r += n;
    return r;
  }

  constexpr pic_iterator& operator++() noexcept {
    ++pos_;
    return *this;
  }

  constexpr pic_iterator operator++(int) noexcept {
    auto old = *this;
    ++pos_;
    return old;
  }

  constexpr pic_iterator& operator-=(difference_type n) noexcept {
    pos_ -= n;
    return *this;
  }

  friend constexpr pic_iterator operator-(const pic_iterator& lhs, difference_type n) noexcept {
    auto r = lhs;
    r -= n;
    return r;
  }

  friend constexpr pic_iterator operator-(difference_type n, const pic_iterator& rhs) noexcept {
    auto r = rhs;
    r -= n;
    return r;
  }

  constexpr pic_iterator& operator--() noexcept {
    --pos_;
    return *this;
  }

  constexpr pic_iterator operator--(int) noexcept {
    auto old = *this;
    --pos_;
    return old;
  }

  friend constexpr difference_type operator-(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    return lhs.pos_ - rhs.pos_;
  }

  constexpr reference operator[](std::size_t index) noexcept {
    return *(*this + index);
  }

  friend constexpr bool operator==(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return lhs.pos_ == rhs.pos_;
  }

  friend constexpr bool operator!=(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return !(lhs == rhs);
  }

  friend constexpr bool operator<(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return lhs.pos_ < rhs.pos_;
  }

  friend constexpr bool operator<=(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return lhs.pos_ <= rhs.pos_;
  }

  friend constexpr bool operator>(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return lhs.pos_ > rhs.pos_;
  }

  friend constexpr bool operator>=(const pic_iterator& lhs, const pic_iterator& rhs) noexcept {
    constexpr_assert(lhs.array_data_ == rhs.array_data_, "compared iterators must belong to the same range");
    return lhs.pos_ >= rhs.pos_;
  }

  // Sorting, ranges-style.
  friend constexpr void iter_swap(const pic_iterator& lhs, const pic_iterator& rhs) {
    iter_swap(lhs.pos_, rhs.pos_);
  }

private:
  const element_t<value_type>* array_data_ = nullptr;
  storage_type* pos_ = nullptr;
};

template <typename T, std::size_t N, std::size_t storage_size>
struct pic_array {
  using iterator                = pic_iterator<      T>;
  using const_iterator          = pic_iterator<const T>;

  using value_type              = typename iterator::value_type;
  using storage_type            = typename iterator::storage_type;

  using size_type               = std::size_t;
  using difference_type         = std::ptrdiff_t;

  using reference               = typename iterator::reference;
  using const_reference         = typename const_iterator::reference;
  using pointer                 = typename iterator::pointer;
  using const_pointer           = typename const_iterator::pointer;
private:
  storage_type data_[N] = {};
  element_t<value_type> array_data_[storage_size] = {};

  struct appender;

  template <typename Iter, std::size_t... I, std::size_t... J>
  constexpr pic_array(Iter iter, std::index_sequence<I...>, std::index_sequence<J...>, appender append = {})
    : data_{((void)I, append(*iter++))...}
    , array_data_{append.array_data[J]...}
  {
  }

public:

  // Construct from array of NUL-terminated string constants
  template <std::size_t... Ns>
  constexpr pic_array(const value_type (&... values)[Ns]) 
    : pic_array({ value_type{values, Ns > 0 ? Ns - 1 : Ns}... })
  {
  }

  // Constructor intended for converting between different variations of const
  template <typename U, std::enable_if_t<std::is_convertible<U, T>::value, std::size_t> M, std::size_t other_storage_size>
  constexpr pic_array(const pic_array<U, M, other_storage_size>& rhs)
    : pic_array(rhs.begin(), std::make_index_sequence<N>(), std::make_index_sequence<storage_size>()) {}

  constexpr pic_array(std::initializer_list<value_type> items)
    : pic_array(items.begin(), std::make_index_sequence<N>(), std::make_index_sequence<storage_size>()) {}

  // Iterators
  constexpr       iterator begin()       noexcept { return {array_data_, std::begin(data_)}; }
  constexpr const_iterator begin() const noexcept { return {array_data_, std::begin(data_)}; }
  constexpr       iterator end()         noexcept { return {array_data_, std::end(data_)  }; }
  constexpr const_iterator end()   const noexcept { return {array_data_, std::end(data_)  }; }

  // Capacity
  constexpr size_type size() const noexcept { return N; }
  constexpr size_type max_size() const noexcept { return N; }

  // Element access
  constexpr       reference operator[](std::size_t index)       { return begin()[index]; }
  constexpr const_reference operator[](std::size_t index) const { return begin()[index]; }

private:
  friend iterator;
  friend const_iterator;

  // Intermediate functor, required because value_type might not be constexpr default constructible
  // or constexpr copy assignable. This way it only needs to be constexpr copy or move assignable.
  struct appender {
    std::ptrdiff_t store_pos = 0;
    element_t<value_type> array_data[storage_size] = {};

    template <typename View>
    constexpr auto operator()(const View& item)
      -> std::enable_if_t<
        std::is_same<element_t<View>, element_t<value_type>>::value
      , ext_array<typename View::value_type>
      >
    {
      const bool is_nul_terminated = !item.empty() && item.back() == '\0';

      pic_store_type<View> str_value{store_pos, item.size()};

      // constexpr std::copy before C++20
      for (const auto& val : item) {
        array_data[store_pos++] = val;
      }
      if (is_nul_terminated)
        str_value.data_size -= 1;
      else
        store_pos += 1; // NUL-terminator

      return str_value;
    }

    template <typename ViewL, typename ViewR>
    constexpr auto operator()(const std::pair<ViewL, ViewR>& item)
      -> std::enable_if_t<
        std::is_same<element_t<ViewL>, element_t<value_type>>::value
        && std::is_same<element_t<ViewL>, element_t<ViewR>>::value
      , storage_type
      >
    {
      return storage_type{(*this)(item.first), (*this)(item.second)};
    }

    template <typename View, typename U>
    constexpr auto operator()(const std::pair<View, U>& item)
      -> std::enable_if_t<
        std::is_same<element_t<View>, element_t<value_type>>::value
        && !has_type<element_type<remove_cv_t<U>>>::value
      , storage_type
      >
    {
      return storage_type{(*this)(item.first), item.second};
    }

    template <typename View, typename U>
    constexpr auto operator()(const std::pair<U, View>& item)
      -> std::enable_if_t<
        std::is_same<element_t<View>, element_t<value_type>>::value
        && !has_type<element_type<remove_cv_t<U>>>::value
      , storage_type
      >
    {
      return storage_type{item.first, (*this)(item.second)};
    }
  };
};

} // namespace bits

// Helpers to preserve arrays in type information, instead of letting them decay to pointers
template <typename T, typename U, std::size_t TN, std::size_t UN>
constexpr std::pair<T (&)[TN], U (&)[UN]> kv_pair(T (& key)[TN], U (& val)[UN]) {
  return {key, val};
}

template <typename T, typename U, std::size_t N>
constexpr std::pair<T (&)[N], U> kv_pair(T (& key)[N], U val) {
  return {key, val};
}

template <typename T, typename U, std::size_t N>
constexpr std::pair<T, U (&)[N]> kv_pair(T key, U (& val)[N]) {
  return {key, val};
}

template <typename T, typename U>
constexpr std::pair<T, U> kv_pair(T key, U val) {
  return {key, val};
}

} // namespace frozen

#endif
