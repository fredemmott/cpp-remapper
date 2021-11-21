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
#include <type_traits>

#include "Controls.h"

namespace fredemmott::inputmapping {

class AnySource;
class AnySink {};

template <std::derived_from<Control> TControl>
class Sink : public AnySink {
 protected:
  Sink() = default;

 public:
  using InControl = TControl;
  using In = typename TControl::Value;
  virtual void map(In value) = 0;
};
using AxisSink = Sink<Axis>;
using ButtonSink = Sink<Button>;
using HatSink = Sink<Hat>;

// clang-format off
template<typename T>
concept is_sink =
  (!std::derived_from<T, AnySource>)
  && std::derived_from<T, AnySink>;

template<typename T, typename TControl>
concept sink_invocable =
  std::is_base_of_v<Control, TControl>
  && std::invocable<T, typename TControl::Value>
  && std::same_as<void, std::invoke_result_t<T, typename TControl::Value>>;
// clang-format on

}// namespace fredemmott::inputmapping
