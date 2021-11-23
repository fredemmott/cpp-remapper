/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <any>
#include <concepts>
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

template <std::derived_from<Control> TControl>
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

// TODO: rename to CompletePipeline
class Pipeline final {
 public:
  Pipeline() = delete;
  template<typename T, std::enable_if_t<std::is_base_of_v<AnySource, T>, bool> = true>
  Pipeline(const UnsafeRef<T>& source) : mSource(source) {
  }

 private:
  UnsafeRef<AnySource> mSource;
};

template <std::derived_from<Control> TControl>
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

// TODO: add test
template <std::derived_from<Control> TIn,
std::derived_from<Control> TOut>
class TransformPipeline final : public Sink<TIn>, public Source<TOut> {
  public:
    TransformPipeline() = delete;
    TransformPipeline(
      const SinkRef<TIn>& first,
      const SourceRef<TOut>& last): mFirst(first), mLast(last) {}

  virtual void setNext(const UnsafeRef<Sink<TIn>>& next) override {
    mLast->setNext(next);
  }

  virtual void map(typename TIn::Value value) override {
    mFirst->map(value);
  }
 private:
  UnsafeRef<Sink<TIn>> mFirst;
  UnsafeRef<Source<TOut>> mLast;
};

}// namespace fredemmott::inputmapping
