/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <functional>
#include <type_traits>

#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

template <std::derived_from<Control> TIn, std::derived_from<Control> TOut>
class FunctionTransform final : public Sink<TIn>, public Source<TOut> {
 public:
  using Impl = std::function<typename TOut::Value(typename TIn::Value)>;

  FunctionTransform() = delete;
  FunctionTransform(const Impl& impl) : impl(impl) {};
  void map(typename TIn::Value value) final override {
    this->emit(impl(value));
  };

 private:
  Impl impl;
};

}// namespace fredemmott::inputmapping
