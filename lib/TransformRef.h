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

template<typename T, typename TOut>
concept transform_to =
  any_transform<T>
  && std::derived_from<T, Source<TOut>>;

template<typename T, typename TIn>
concept transform_invocable_from =
  std::is_base_of_v<Control, TIn>
  && std::invocable<T, typename TIn::Value>
  && detail::exact_arguments<T, typename TIn::Value>;

template<typename T, typename TIn, typename TOut>
concept transform_invocable =
  transform_invocable_from<T, TIn>
  && detail::exact_return_type<T, typename TOut::Value>;

template<typename T>
concept any_transform_ref =
  any_transform<typename T::element_type>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TIn>
concept transform_from_ref =
  transform_from<typename T::element_type, TIn>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TOut>
concept transform_to_ref =
  transform_to<typename T::element_type, TOut>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TIn, typename TOut>
concept transform_ref =
  transform_from_ref<T, TIn>
  && std::derived_from<typename T::element_type, Source<TOut>>;

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

template <typename T>
auto convert_to_any_transform_ref(T&& in) {
  using DT = std::decay_t<T>;
	if constexpr (any_transform_ref<DT>) {
		return in;
	}

	if constexpr (any_transform<DT>) {
		return maybe_shared_ptr<DT>(std::forward<T>(in));
	}

	if constexpr (any_transform<std::remove_pointer_t<DT>>) {
		return maybe_shared_ptr<std::remove_pointer_t<DT>>(in);
	}

	if constexpr (transform_invocable<DT, Axis, Axis>) {
		return std::make_shared<FunctionTransform<Axis, Axis>>(in);
	}

	if constexpr (transform_invocable<DT, Button, Button>) {
		return std::make_shared<FunctionTransform<Button, Button>>(in);
	}

	if constexpr (transform_invocable<DT, Hat, Hat>) {
		return std::make_shared<FunctionTransform<Hat, Hat>>(in);
	}
}

// clang-format off
template <typename T>
concept convertible_to_any_transform_ref = requires(T x) {
	{ convert_to_any_transform_ref(x) } -> any_transform_ref;
};

template <typename T>
concept non_id_convertible_to_any_transform_ref =
  !any_transform_ref<T>
  && convertible_to_any_transform_ref<T>;

template <typename T, typename TIn>
concept convertible_to_transform_from_ref = requires(T x) {
	{ convert_to_any_transform_ref(x) } -> transform_from_ref<TIn>;
};

template <typename T, typename TIn>
concept non_id_convertible_to_transform_from_ref =
  !any_transform_ref<std::decay_t<T>>
  && convertible_to_transform_from_ref<T, TIn>;

template <typename T, typename TOut>
concept convertible_to_transform_to_ref = requires(T x) {
	{ convert_to_any_transform_ref(x) } -> transform_to_ref<TOut>;
};

template <typename T, typename TOut>
concept non_id_convertible_to_transform_to_ref =
  !any_transform_ref<std::decay_t<T>>
  && convertible_to_transform_to_ref<T, TOut>;
// clang-format on

}// namespace fredemmott::inputmapping
