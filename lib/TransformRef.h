/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "function_traits.h"
#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

// clang-format off
template<typename T>
concept any_transform =
  std::derived_from<T, AnySource>
  && std::derived_from<T, AnySink>;

template<typename T, typename TIn, typename TOut>
concept transform =
  std::derived_from<T, Sink<TIn>>
  && std::derived_from<T, Source<TOut>>;

template<typename T, typename TIn>
concept transform_from =
  any_transform<T>
  && std::derived_from<T, Sink<TIn>>;

template<typename T, typename TIn>
concept transform_invocable_from =
  std::is_base_of_v<Control, TIn>
  && std::invocable<T, typename TIn::Value>
  && std::same_as<
    typename TIn::Value,
    typename ::fredemmott::inputmapping::detail::function_traits<T>::FirstArg>;

template<typename T, typename TIn, typename TOut>
concept transform_invocable =
  transform_invocable_from<T, TIn>
  && std::same_as<
    typename TOut::Value,
    std::invoke_result_t<T, typename TIn::Value>>;

template<typename T>
concept any_transform_ref =
  any_transform<typename T::element_type>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T, typename TIn>
concept transform_from_ref =
  transform_from<typename T::element_type, TIn>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T, typename TIn, typename TOut>
concept transform_ref =
  transform_from_ref<T, TIn>
  && std::derived_from<T, Source<TOut>>;

template<typename T>
concept any_sink_or_transform = std::derived_from<T, AnySink>;

template<typename T>
concept any_sink_or_transform_ref =
  any_sink_or_transform<typename T::element_type>;

template<typename T>
concept any_source_or_transform = std::derived_from<T, AnySource>;

template<typename T>
concept any_source_or_transform_ref =
  any_source_or_transform<typename T::element_type>;
// clang-format on

}// namespace fredemmott::inputmapping
