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

template <typename TIn, typename TOut>
class FunctionTransform final : public Sink<TIn>, public Source<TOut> {
  static_assert(std::is_base_of_v<Control, TIn>);
  static_assert(std::is_base_of_v<Control, TOut>);

 public:
  using Impl = std::function<typename TOut::Value(typename TIn::Value)>;

  FunctionTransform() = delete;
  FunctionTransform(const Impl& impl) : impl(impl) {};
  void map(typename TIn::Value value) final override {
    emit(impl(value));
  };

 private:
  Impl impl;
};

}// namespace fredemmott::inputmapping
