/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include <cpp-remapper/Controls.h>
#include <cpp-remapper/Sink.h>
#include <cpp-remapper/maybe_shared_ptr.h>

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

  Out value() const {
    return mValue;
  }

 protected:
  void emit(Out value) {
    if (!mNext.isValid()) {
      return;
    }
    mValue = value;
    mNext->map(value);
  }

 private:
  maybe_shared_ptr<Sink<TControl>> mNext;
  Out mValue;
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
