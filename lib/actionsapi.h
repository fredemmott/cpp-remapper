/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <type_traits>

#include "Controls.h"
#include "FunctionSink.h" // remove?
#include "Sink.h"
#include "Source.h"
#include "SourceRef.h"
#include "SinkRef.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class SinkPipeline final : public Sink<TControl> {
  static_assert(std::is_base_of_v<Control, TControl>);

 public:
  SinkPipeline() = delete;

  void map(typename TControl::Value value) override {
    mHead->map(value);
  };

 private:
  UnsafeRef<Sink<TControl>> mHead;
};

class Pipeline final {
 public:
  Pipeline() = delete;
  template<typename T, std::enable_if_t<std::is_base_of_v<AnySource, T>, bool> = true>
  Pipeline(const UnsafeRef<T>& source) : mSource(source) {
  }

 private:
  UnsafeRef<AnySource> mSource;
};

template <typename TControl>
class SourcePipeline final : public Source<TControl> {
 public:
  SourcePipeline() = delete;

  SourcePipeline(
    const UnsafeRef<AnySource>& first,
    const UnsafeRef<Source<TControl>>& last)
    : mFirst(first), mLast(last) {};

  virtual void setNext(const UnsafeRef<Sink<TControl>>& next) override {
    mLast->setNext(next);
  }

 private:
  UnsafeRef<AnySource> mFirst;
  UnsafeRef<Source<TControl>> mLast;
};

}// namespace fredemmott::inputmapping
