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
#include "FunctionSink.h"
#include "Source.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <std::derived_from<Control> TControl>
class SinkRef final : public Sink<TControl> {
 public:
  using element_type = Sink<TControl>;
  using Impl = UnsafeRef<element_type>;

  explicit SinkRef(const Impl& impl) : p(impl) {};

  template <typename T>
  SinkRef(T impl) requires std::convertible_to<T, Impl> &&(
    !std::convertible_to<typename T::element_type, AnySource>)
    : p(impl) {};

  template <sink_invocable<TControl> T>
  SinkRef(T impl) /* : p(std::make_shared<FunctionSink<TControl>>(impl))*/
  {
    // TODO
  }

  virtual void map(typename TControl::Value value) override {
    p->map(value);
  }

  operator UnsafeRef<Sink<TControl>>() const {
    return p;
  }

 private:
  Impl p;
};
using AxisSinkRef = SinkRef<Axis>;
using ButtonSinkRef = SinkRef<Button>;
using HatSinkRef = SinkRef<Hat>;

// clang-format off
template<typename T>
concept is_sink_ref =
  is_sink<typename T::element_type>
  && std::convertible_to<T, UnsafeRef<typename T::element_type>>;

template<typename T>
concept is_nonref_sink =
  is_sink<typename T::element_type>
  && !is_sink_ref<T>;
// clang-format on

}// namespace fredemmott::inputmapping
