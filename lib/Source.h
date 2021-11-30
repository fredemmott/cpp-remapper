/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Controls.h"
#include "Sink.h"
#include "maybe_shared_ptr.h"

#include <concepts>

namespace fredemmott::inputmapping {

// Just so we can store a shared_ptr without worrying about the inner generics
class AnySource {
 protected:
  AnySource() = default;

 public:
  virtual ~AnySource();
};

template <std::derived_from<Control> TControl>
class Source : public AnySource {

 public:
  using OutControl = TControl;
  using Out = typename TControl::Value;

  virtual void setNext(const maybe_shared_ptr<Sink<TControl>>& next) {
    mNext = next;
  }

 protected:
  void emit(Out value) {
    if (!mNext.isValid()) {
      return;
    }
    mNext->map(value);
  }

 private:
  maybe_shared_ptr<Sink<TControl>> mNext;
};

using AxisSource = Source<Axis>;
using ButtonSource = Source<Button>;
using HatSource = Source<Hat>;

// clang-format off
template<typename T>
concept any_source =
  std::derived_from<T, AnySource>
  && (!std::derived_from<T, AnySink>);

template<typename T, typename TControl>
concept source =
  any_source<T>
  && std::derived_from<T, Source<TControl>>;
// clang-format on

}// namespace fredemmott::inputmapping
