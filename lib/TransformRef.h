/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

// clang-format off
template<typename T>
concept is_any_transform =
  std::derived_from<T, AnySource>
  && std::derived_from<T, AnySink>;

template<typename T, typename TIn, typename TOut>
concept transform =
  std::derived_from<T, Sink<typename TIn::InControl>>
  && std::derived_from<T, Source<typename TOut::OutControl>>;

template<typename T, typename TIn>
concept is_transform_from =
  is_any_transform<T>
  && std::derived_from<T, Sink<TIn>>;

template<typename T, typename TIn>
concept transform_invocable_from =
  std::is_base_of_v<Control, TIn>
  && std::invocable<T, typename TIn::Value>
  && !std::same_as<void, std::invoke_result_t<T, typename TIn::Value>>;

template<typename T, typename TIn, typename TOut>
concept transform_invocable =
  transform_invocable_from<T, TIn>
  && std::same_as<
    typename TOut::Value,
    std::invoke_result_t<T, typename TIn::Value>>;

template<typename T>
concept is_transform_ref =
  is_any_transform<typename T::element_type>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T, typename TIn>
concept is_transform_from_ref =
  is_transform_from<typename T::element_type, TIn>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;
// clang-format on

}// namespace fredemmott::inputmapping
