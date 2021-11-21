/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

#include <concepts>

namespace fredemmott::inputmapping {

template<typename T>
concept is_source =
  std::derived_from<T, AnySource> &&
  (!std::derived_from<T, AnySink>);

template<typename T>
concept is_sink =
  (!std::derived_from<T, AnySource>) &&
  std::derived_from<T, AnySink>;

template<typename T>
concept is_transform =
  std::derived_from<T, AnySource> &&
  std::derived_from<T, AnySink>;

template<typename T>
concept is_sink_ref = is_sink<typename T::element_type> && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T>
concept is_source_ref = is_source<typename T::element_type> && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T>
concept is_transform_ref = is_transform<typename T::element_type> && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template <
  is_source_ref SourceRef,
  is_sink_ref SinkRef
>
Pipeline operator>>(SourceRef left, SinkRef right) {
  left.setNext(right);
  return Pipeline(UnsafeRef<typename SourceRef::element_type>(left));
}

}// namespace fredemmott::inputmapping
