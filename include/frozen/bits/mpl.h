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

#ifndef FROZEN_LETITGO_BITS_MPL_H
#define FROZEN_LETITGO_BITS_MPL_H

#include <utility>

namespace frozen {

namespace bits {

// Forward declarations
template <class, std::size_t>
class carray;
template <typename T, std::size_t N, std::size_t string_size>
struct pic_array;

template <typename T>
struct remove_cv : std::remove_cv<T> {};

template <typename... T>
struct remove_cv<std::pair<T...>> {
  using type = std::pair<typename remove_cv<T>::type...>;
};

template <typename T, std::size_t N>
struct remove_cv<carray<T, N>> {
  using type = carray<typename remove_cv<T>::type, N>;
};

template <typename T, std::size_t N, std::size_t string_size>
struct remove_cv<pic_array<T, N, string_size>> {
  using type = pic_array<typename remove_cv<T>::type, N, string_size>;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename Container, typename ElemRef, typename Value>
class copy_cv_iter_ref
{
private:
  using cv_container = std::remove_reference_t<Container>;
  using c_value = std::conditional_t<std::is_const<cv_container>::value, std::add_const_t<Value>, Value>;
  using cv_value = std::conditional_t<std::is_volatile<cv_container>::value, std::add_volatile_t<c_value>, c_value>;
  using cv_ref_value = std::conditional_t<std::is_lvalue_reference<ElemRef>::value, std::add_lvalue_reference_t<cv_value>, cv_value>;
  using cv_rvref_value = std::conditional_t<
      std::is_lvalue_reference<cv_ref_value>::value
   && std::is_rvalue_reference<Container>::value
   , std::add_rvalue_reference_t<cv_value>
   , cv_ref_value>;
public:
  using type = cv_rvref_value;
};

template <typename Container, typename ElemRef, typename Value>
using copy_cv_iter_ref_t = typename copy_cv_iter_ref<Container, ElemRef, Value>::type;

} // namespace bits

} // namespace frozen

#endif
