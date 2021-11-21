/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>

#include "Sink.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class FunctionSink : public Sink<TControl> {
 public:
  using Impl = std::function<void(typename TControl::Value)>;
  FunctionSink() = delete;
  FunctionSink(const Impl& impl) : impl(impl) {};
  virtual void map(typename TControl::Value value) override {
    impl(value);
  }

 private:
  Impl impl;
};

}// namespace fredemmott::inputmapping
