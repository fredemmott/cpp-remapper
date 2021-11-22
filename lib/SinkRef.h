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
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <std::derived_from<Control> TControl>
class SinkRef : public std::shared_ptr<Sink<TControl>> {
 public:
  using std::shared_ptr<Sink<TControl>>::shared_ptr;

  template <sink_invocable<TControl> Func>
  SinkRef(const Func& fun) {
    this->reset(new FunctionSink<TControl>(fun));
  }
};
using AxisSinkRef = SinkRef<Axis>;
using ButtonSinkRef = SinkRef<Button>;
using HatSinkRef = SinkRef<Hat>;

// clang-format off
template <typename T, typename TControl>
concept convertible_to_sink_ref =
  sink<T, TControl>
  || sink_ref<T, TControl>
  || sink_invocable<T, TControl>;
// clang-format on

}// namespace fredemmott::inputmapping