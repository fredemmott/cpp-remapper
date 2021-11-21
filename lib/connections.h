/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

namespace fredemmott::inputmapping {

// TODO: use C++20 concepts instead
template <typename T, typename = void>
struct is_transform : std::false_type {};

template <typename T>
struct is_transform<
  T,
  std::enable_if_t<
    std::is_base_of_v<AnySink, T> && std::is_base_of_v<AnySource, T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_transform<T>::value_type is_transform_v
  = is_transform<T>::value;

template <typename T, typename = void>
struct is_sink : std::false_type {};
template <typename T>
struct is_sink<
  T,
  std::enable_if_t<std::is_base_of_v<AnySink, T> && !is_transform_v<T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_sink<T>::value_type is_sink_v = is_sink<T>::value;

template <typename T, typename = void>
struct is_source : std::false_type {};
template <typename T>
struct is_source<
  T,
  std::enable_if_t<std::is_base_of_v<AnySource, T> && !is_transform_v<T>>>
  : std::true_type {};

template <typename T>
constexpr typename is_source<T>::value_type is_source_v = is_source<T>::value;

template <
  typename SourceRef,
  typename SinkRef,
  std::enable_if_t<is_source_v<typename SourceRef::element_type>, bool> = true,
  std::enable_if_t<is_sink_v<typename SinkRef::element_type>, bool> = true>
Pipeline operator>>(SourceRef left, SinkRef right) {
  left.setNext(right);
  return Pipeline(UnsafeRef<typename SourceRef::element_type>(left));
}

}// namespace fredemmott::inputmapping
