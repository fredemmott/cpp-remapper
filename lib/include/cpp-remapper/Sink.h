/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/Controls.h>

#include <concepts>
#include <functional>
#include <type_traits>

namespace fredemmott::inputmapping {

class AnySource;
class AnySink {};

template <std::derived_from<Control> TControl>
class Sink : public AnySink {
 protected:
  Sink() = default;

 public:
  virtual ~Sink() = default;
  using InControl = TControl;
  using In = typename TControl::Value;
  virtual void map(In value) = 0;
};
using AxisSink = Sink<Axis>;
using ButtonSink = Sink<Button>;
using HatSink = Sink<Hat>;

// clang-format off
template<typename T>
concept any_sink =
  (!std::derived_from<T, AnySource>)
  && std::derived_from<T, AnySink>;

template<typename T, typename TControl>
concept sink =
  any_sink<T>
  && std::derived_from<T, Sink<TControl>>;
// clang-format on

}// namespace fredemmott::inputmapping
