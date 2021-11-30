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

template<std::derived_from<Control> TControl>
using SourceRef = maybe_shared_ptr<Source<TControl>>;
using AxisSourceRef = SourceRef<Axis>;
using ButtonSourceRef = SourceRef<Button>;
using HatSourceRef = SourceRef<Hat>;

// clang-format off
template<typename T>
concept any_source_ref =
  any_source<typename T::element_type>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TControl>
concept source_ref =
  any_source_ref<T>
  && source<typename T::element_type, TControl>;
// clang-format on

template <typename T>
auto convert_to_any_source_ref(T&& in) {
  if constexpr (any_source_ref<std::decay_t<T>>) {
    return in;
  }

  if constexpr (any_source<std::decay_t<T>>) {
    return maybe_shared_ptr<std::decay_t<T>>(std::forward<T>(in));
  }
}

// clang-format off
template <typename T>
concept convertible_to_any_source_ref = requires(T x) {
	{ convert_to_any_source_ref(x) } -> any_source_ref;
};

template <typename T>
concept non_id_convertible_to_any_source_ref =
  !any_source_ref<std::decay_t<T>>
  && convertible_to_any_source_ref<T>;

template <typename T, typename TControl>
concept convertible_to_source_ref = requires(T x) {
	{ convert_to_any_source_ref(x) } -> source_ref<TControl>;
};

template <typename T, typename TControl>
concept non_id_convertible_to_source_ref =
  !source_ref<std::decay_t<T>, TControl>
  && convertible_to_source_ref<T, TControl>;
// clang-format on


}// namespace fredemmott::inputmapping
