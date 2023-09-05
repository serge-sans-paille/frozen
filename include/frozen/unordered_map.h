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

#ifndef FROZEN_LETITGO_UNORDERED_MAP_H
#define FROZEN_LETITGO_UNORDERED_MAP_H

#include "frozen/bits/basic_types.h"
#include "frozen/bits/constexpr_assert.h"
#include "frozen/bits/elsa.h"
#include "frozen/bits/exceptions.h"
#include "frozen/bits/mpl.h"
#include "frozen/bits/pic_array.h"
#include "frozen/bits/pmh.h"
#include "frozen/bits/version.h"
#include "frozen/random.h"

#include <tuple>
#include <functional>
#include <type_traits>
#include <utility>

namespace frozen {

namespace bits {

struct GetKey {
  template <class KV> constexpr auto const &operator()(KV const &kv) const {
    return kv.first;
  }
};

} // namespace bits

template <class Key, class Value, std::size_t N, typename Hash = anna<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Container = bits::carray<std::pair<const Key, Value>, N>>
class unordered_map : private KeyEqual {
  static constexpr std::size_t storage_size =
      bits::next_highest_power_of_two(N) * (N < 32 ? 2 : 1); // size adjustment to prevent high collision rate for small sets
  using container_type = Container;
  using tables_type = bits::pmh_tables<storage_size, Hash>;

  container_type items_;
  tables_type tables_;

public:
  /* typedefs */
  using Self = unordered_map<Key, Value, N, Hash, KeyEqual, Container>;
  using key_type = Key;
  using mapped_type = Value;
  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using pointer = typename container_type::pointer;
  using const_pointer = typename container_type::const_pointer;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

public:
  /* constructors */
  unordered_map(unordered_map const &) = default;
  constexpr unordered_map(container_type items,
                          Hash const &hash, KeyEqual const &equal)
      : KeyEqual{equal}
      , items_{items}
      , tables_{
            bits::make_pmh_tables<storage_size>(
                items_, hash, bits::GetKey{}, default_prg_t{})} {}
  explicit constexpr unordered_map(container_type items)
      : unordered_map{items, Hash{}, KeyEqual{}} {}

  constexpr unordered_map(std::initializer_list<value_type> items,
                          Hash const & hash, KeyEqual const & equal)
      : unordered_map{container_type{items}, hash, equal} {
        constexpr_assert(items.size() == N, "Inconsistent initializer_list size and type size argument");
      }

  constexpr unordered_map(std::initializer_list<value_type> items)
      : unordered_map{items, Hash{}, KeyEqual{}} {}

  /* iterators */
  constexpr iterator begin() { return items_.begin(); }
  constexpr iterator end() { return items_.end(); }
  constexpr const_iterator begin() const { return items_.begin(); }
  constexpr const_iterator end() const { return items_.end(); }
  constexpr const_iterator cbegin() const { return items_.begin(); }
  constexpr const_iterator cend() const { return items_.end(); }

  /* capacity */
  constexpr bool empty() const { return !N; }
  constexpr size_type size() const { return N; }
  constexpr size_type max_size() const { return N; }

  /* lookup */
  template <class KeyType, class Hasher, class Equal>
  constexpr std::size_t count(KeyType const &key, Hasher const &hash, Equal const &equal) const {
    return find(key, hash, equal) != end();
  }
  template <class KeyType>
  constexpr std::size_t count(KeyType const &key) const {
    return count(key, hash_function(), key_eq());
  }

  template <class KeyType, class Hasher, class Equal>
  constexpr decltype(auto) at(KeyType const &key, Hasher const &hash, Equal const &equal) const {
    return at_impl(*this, key, hash, equal);
  }
  template <class KeyType, class Hasher, class Equal>
  constexpr decltype(auto) at(KeyType const &key, Hasher const &hash, Equal const &equal) {
    return at_impl(*this, key, hash, equal);
  }
  template <class KeyType>
  constexpr decltype(auto) at(KeyType const &key) const {
    return at(key, hash_function(), key_eq());
  }
  template <class KeyType>
  constexpr decltype(auto) at(KeyType const &key) {
    return at(key, hash_function(), key_eq());
  }

  template <class KeyType, class Hasher, class Equal>
  constexpr const_iterator find(KeyType const &key, Hasher const &hash, Equal const &equal) const {
    return find_impl(*this, key, hash, equal);
  }
  template <class KeyType, class Hasher, class Equal>
  constexpr iterator find(KeyType const &key, Hasher const &hash, Equal const &equal) {
    return find_impl(*this, key, hash, equal);
  }
  template <class KeyType>
  constexpr const_iterator find(KeyType const &key) const {
    return find(key, hash_function(), key_eq());
  }
  template <class KeyType>
  constexpr iterator find(KeyType const &key) {
    return find(key, hash_function(), key_eq());
  }
  
  template <class KeyType>
  constexpr bool contains(KeyType const &key) const {
    return this->find(key) != this->end();
  }

  template <class KeyType, class Hasher, class Equal>
  constexpr std::pair<const_iterator, const_iterator> equal_range(KeyType const &key, Hasher const &hash, Equal const &equal) const {
    return equal_range_impl(*this, key, hash, equal);
  }
  template <class KeyType, class Hasher, class Equal>
  constexpr std::pair<iterator, iterator> equal_range(KeyType const &key, Hasher const &hash, Equal const &equal) {
    return equal_range_impl(*this, key, hash, equal);
  }
  template <class KeyType>
  constexpr std::pair<const_iterator, const_iterator> equal_range(KeyType const &key) const {
    return equal_range(key, hash_function(), key_eq());
  }
  template <class KeyType>
  constexpr std::pair<iterator, iterator> equal_range(KeyType const &key) {
    return equal_range(key, hash_function(), key_eq());
  }

  /* bucket interface */
  constexpr std::size_t bucket_count() const { return storage_size; }
  constexpr std::size_t max_bucket_count() const { return storage_size; }

  /* observers*/
  constexpr const hasher& hash_function() const { return tables_.hash_function(); }
  constexpr const key_equal& key_eq() const { return static_cast<KeyEqual const&>(*this); }

private:
  template <class This, class KeyType, class Hasher, class Equal>
  static inline constexpr decltype(auto) at_impl(This&& self, KeyType const &key, Hasher const &hash, Equal const &equal)
  {
    using return_type = bits::copy_cv_iter_ref_t<
        This&&
      , typename std::decay_t<This>::reference
      , decltype(std::forward<This>(self).find(key, hash, equal)->second)
      >;

    auto it = std::forward<This>(self).find(key, hash, equal);
    if (it == self.end())
      // early escape to ensure type deduction works even when std::abort() isn't properly marked as [[noreturn]]
      FROZEN_THROW_OR_ABORT(std::out_of_range("unknown key"));
    return static_cast<return_type>(it->second);
  }

  template <class This, class KeyType, class Hasher, class Equal>
  static inline constexpr auto find_impl(This&& self, KeyType const &key, Hasher const &hash, Equal const &equal) {
    auto const pos = self.tables_.lookup(key, hash);
    auto it = self.items_.begin() + pos;
    if (it != self.items_.end() && equal(it->first, key))
      return it;
    else
      return self.items_.end();
  }

  template <class This, class KeyType, class Hasher, class Equal>
  static inline constexpr auto equal_range_impl(This&& self, KeyType const &key, Hasher const &hash, Equal const &equal) {
    auto const it = self.find(key, hash, equal);
    if (it != self.end())
      return std::make_pair(it, it + 1);
    else
      return std::make_pair(self.end(), self.end());
  }
};

template <typename T, typename U, std::size_t N>
constexpr auto make_unordered_map(std::pair<T, U> const (&items)[N]) {
  return unordered_map<T, U, N>{items};
}

template <typename T, typename U, std::size_t N, typename Hasher, typename Equal>
constexpr auto make_unordered_map(
        std::pair<T, U> const (&items)[N],
        Hasher const &hash = elsa<T>{},
        Equal const &equal = std::equal_to<T>{}) {
  return unordered_map<T, U, N, Hasher, Equal>{items, hash, equal};
}

template <typename T, typename U, std::size_t N>
constexpr auto make_unordered_map(std::array<std::pair<T, U>, N> const &items) {
  return unordered_map<T, U, N>{items};
}

template <typename T, typename U, std::size_t N, typename Hasher, typename Equal>
constexpr auto make_unordered_map(
        std::array<std::pair<T, U>, N> const &items,
        Hasher const &hash = elsa<T>{},
        Equal const &equal = std::equal_to<T>{}) {
  return unordered_map<T, U, N, Hasher, Equal>{items, hash, equal};
}

template <
    typename T
  , typename U
  , typename Hasher
  , typename Equal
  , typename ElemT
  , std::enable_if_t<
      !bits::has_type<bits::element_type<U>>::value
   && !bits::is_pair<Hasher>::value
   && !bits::is_pair<Equal>::value
   && std::is_same<ElemT, bits::element_t<T>>::value
    , std::size_t>... KNs
  >
constexpr auto make_unordered_map(
  Hasher const &hash,
  Equal const &equal,
  std::pair<
    bits::array_ref<const ElemT, KNs>
  , U> const&... items) {
  constexpr const auto key_storage_size = bits::accumulate({KNs...});
  using container_type = bits::pic_array<std::pair<const T, U>, sizeof...(KNs), key_storage_size>;
  using value_type = typename container_type::value_type;
  return unordered_map<T, U, sizeof...(KNs), Hasher, Equal, container_type>{
    container_type{value_type(T(items.first.array), U(items.second))...},
    hash,
    equal,
  };
}

template <
    typename T
  , typename U
  , typename ElemT
  , std::enable_if_t<
      !bits::has_type<bits::element_type<U>>::value
   && std::is_same<ElemT, bits::element_t<T>>::value
    , std::size_t>... KNs
  >
constexpr auto make_unordered_map(
  std::pair<
    bits::array_ref<const ElemT, KNs>
  , U> const&... items) {
  return make_unordered_map<T, U>(anna<T>{}, std::equal_to<T>{}, items...);
}

template <
    typename T
  , typename U
  , typename Hasher
  , typename Equal
  , typename ElemT
  , std::enable_if_t<
      !bits::has_type<bits::element_type<T>>::value
   && !bits::is_pair<Hasher>::value
   && !bits::is_pair<Equal>::value
   && std::is_same<ElemT, bits::element_t<U>>::value
    , std::size_t>... VNs
  >
constexpr auto make_unordered_map(
  Hasher const &hash,
  Equal const &equal,
  std::pair<
    T
  , bits::array_ref<const ElemT, VNs>
  > const&... items) {
  constexpr const auto val_storage_size = bits::accumulate({VNs...});
  using container_type = bits::pic_array<std::pair<const T, U>, sizeof...(VNs), val_storage_size>;
  using value_type = typename container_type::value_type;
  return unordered_map<T, U, sizeof...(VNs), Hasher, Equal, container_type>{
    container_type{value_type(T(items.first), U(items.second.array))...},
    hash,
    equal,
  };
}

template <
    typename T
  , typename U
  , typename ElemT
  , std::enable_if_t<
      !bits::has_type<bits::element_type<T>>::value
   && std::is_same<ElemT, bits::element_t<U>>::value
    , std::size_t>... VNs
  >
constexpr auto make_unordered_map(
  std::pair<
    T
  , bits::array_ref<const ElemT, VNs>
  > const&... items) {
  return make_unordered_map<T, U>(anna<T>{}, std::equal_to<T>{}, items...);
}

template <
    typename T
  , typename U
  , typename Hasher
  , typename Equal
  , typename ElemT
  , typename ElemU
  , std::size_t... KNs
  , std::enable_if_t<
      !bits::is_pair<Hasher>::value
   && !bits::is_pair<Equal>::value
   && std::is_same<ElemT, bits::element_t<T>>::value
   && std::is_same<ElemU, bits::element_t<U>>::value
    , std::size_t>... VNs
  >
constexpr auto make_unordered_map(
  Hasher const &hash,
  Equal const &equal,
  std::pair<
    bits::array_ref<const ElemT, KNs>
  , bits::array_ref<const ElemU, VNs>
  > const&... items) {
  constexpr const auto key_storage_size = bits::accumulate({KNs...});
  constexpr const auto val_storage_size = bits::accumulate({VNs...});
  using container_type = bits::pic_array<std::pair<const T, U>, sizeof...(KNs), key_storage_size + val_storage_size>;
  using value_type = typename container_type::value_type;
  return unordered_map<T, U, sizeof...(KNs), Hasher, Equal, container_type>{
    container_type{value_type(T(items.first.array), U(items.second.array))...},
    hash,
    equal,
  };
}

template <
    typename T
  , typename U
  , typename ElemT
  , typename ElemU
  , std::size_t... KNs
  , std::enable_if_t<
      std::is_same<ElemT, bits::element_t<T>>::value
   && std::is_same<ElemU, bits::element_t<U>>::value
    , std::size_t>... VNs
  >
constexpr auto make_unordered_map(std::pair<
    bits::array_ref<const ElemT, KNs>
  , bits::array_ref<const ElemU, VNs>
  > const&... items) {
  return make_unordered_map<T, U>(anna<T>{}, std::equal_to<T>{}, items...);
}

} // namespace frozen

#endif
