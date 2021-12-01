/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>

#include "Controls.h"
#include "FunctionSink.h"
#include "Source.h"
#include "maybe_shared_ptr.h"

namespace fredemmott::inputmapping {

// clang-format off
template<typename T>
concept any_sink_ptr =
  any_sink<typename T::element_type>
  && std::convertible_to<T, maybe_shared_ptr<typename T::element_type>>;

template<typename T, typename TControl>
concept sink_ptr =
  any_sink_ptr<T>
  && sink<typename T::element_type, TControl>;
// clang-format on

template <typename T>
auto convert_to_any_sink_ptr(T&& in) {
  using DT = std::decay_t<T>;

  if constexpr (any_sink_ptr<DT>) {
    return in;
  }

  if constexpr (any_sink<DT>) {
    return maybe_shared_ptr<DT>(std::forward<T>(in));
  }

  if constexpr (any_sink<std::remove_pointer_t<DT>>) {
    return maybe_shared_ptr<DT>(in);
  }

  // Can't infer the inner generic :(
  if constexpr (sink_invocable<DT, Axis>) {
    return std::make_shared<FunctionSink<Axis>>(in);
  }
  if constexpr (sink_invocable<DT, Button>) {
    return std::make_shared<FunctionSink<Button>>(in);
  }
  if constexpr (sink_invocable<DT, Hat>) {
    return std::make_shared<FunctionSink<Hat>>(in);
  }

  if constexpr (std::same_as<DT, Axis::Value*>) {
    return std::make_shared<FunctionSink<Axis>>(
      [in](Axis::Value v) { *in = v; });
  }

  if constexpr (std::same_as<DT, Button::Value*>) {
    return std::make_shared<FunctionSink<Button>>(
      [in](Button::Value v) { *in = v; });
  }

  if constexpr (std::same_as<DT, Hat::Value*>) {
    return std::make_shared<FunctionSink<Hat>>([in](Hat::Value v) { *in = v; });
  }
}

// clang-format off
template <typename T>
concept convertible_to_any_sink_ptr = requires(T x) {
  { convert_to_any_sink_ptr(x) } -> any_sink_ptr;
};

template<typename T>
concept non_id_convertible_to_any_sink_ptr =
  !any_sink_ptr<std::decay_t<T>>
  && convertible_to_any_sink_ptr<T>;

template <typename T, typename TControl>
concept convertible_to_sink_ptr = requires(T x) {
  { convert_to_any_sink_ptr(x) } -> sink_ptr<TControl>;
};

template<typename T, typename TControl>
concept non_id_convertible_to_sink_ptr =
  !any_sink_ptr<T>
  && convertible_to_sink_ptr<T, TControl>;
// clang-format on

template <std::derived_from<Control> TControl>
class SinkRef : public maybe_shared_ptr<Sink<TControl>> {
 public:
  using maybe_shared_ptr<Sink<TControl>>::maybe_shared_ptr;

  template <typename T>
    requires non_id_convertible_to_sink_ptr<T, TControl> SinkRef(T in) {
      *this = convert_to_any_sink_ptr(std::forward<T>(in));
    }
};

using AxisSinkRef = SinkRef<Axis>;
using ButtonSinkRef = SinkRef<Button>;
using HatSinkRef = SinkRef<Hat>;

}// namespace fredemmott::inputmapping
