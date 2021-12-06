/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <memory>

#include "Source.h"
#include "maybe_shared_ptr.h"

namespace fredemmott::inputmapping {

template <std::derived_from<Control> TControl>
using SourcePtr = maybe_shared_ptr<Source<TControl>>;
using AxisSourcePtr = SourcePtr<Axis>;
using ButtonSourcePtr = SourcePtr<Button>;
using HatSourcePtr = SourcePtr<Hat>;

// clang-format off
template<typename T>
concept any_source_ptr =
  any_source<typename T::element_type>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TControl>
concept source_ptr =
  any_source_ptr<T>
  && source<typename T::element_type, TControl>;
// clang-format on

template <typename T>
auto convert_to_any_source_ptr(T&& in) {
  using DT = std::decay_t<T>;
  if constexpr (any_source_ptr<DT>) {
    return in;
  }

  if constexpr (any_source<DT>) {
    return maybe_shared_ptr<std::decay_t<T>>(std::forward<T>(in));
  }

  if constexpr (any_source<std::remove_pointer_t<DT>>) {
    return maybe_shared_ptr<std::remove_pointer_t<DT>>(in);
  }
}

// clang-format off
template <typename T>
concept convertible_to_any_source_ptr = requires(T x) {
	{ convert_to_any_source_ptr(x) } -> any_source_ptr;
};

template <typename T>
concept non_id_convertible_to_any_source_ptr =
  !any_source_ptr<std::decay_t<T>>
  && convertible_to_any_source_ptr<T>;

template <typename T, typename TControl>
concept convertible_to_source_ptr = requires(T x) {
	{ convert_to_any_source_ptr(x) } -> source_ptr<TControl>;
};

template <typename T, typename TControl>
concept non_id_convertible_to_source_ptr =
  !source_ptr<std::decay_t<T>, TControl>
  && convertible_to_source_ptr<T, TControl>;
// clang-format on

}// namespace fredemmott::inputmapping
