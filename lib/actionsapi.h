/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>
#include <type_traits>

#include "Controls.h"
#include "Sink.h"
#include "SinkRef.h"
#include "Source.h"
#include "SourceRef.h"
#include "UnsafeRef.h"

namespace fredemmott::inputmapping {

/** A pipeline that receives input, but produces no output.
 *
 * This is the tail end of a pipeline; it ends with a sink,
 * but may also contain transformations that are applied
 * to values before they reach that sink.
 */
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

/** A pipeline that takes no input and produces no output.
 *
 * This pipeline starts with a source, ends with a sink, and may
 * contain compatible transformations in between the source and the sink.
 */
class ClosedPipeline final {
 public:
  ClosedPipeline() = delete;
  template <
    typename T,
    std::enable_if_t<std::is_base_of_v<AnySource, T>, bool> = true>
  ClosedPipeline(const UnsafeRef<T>& source) : mSource(source) {
  }

 private:
  UnsafeRef<AnySource> mSource;
};

/** A pipeline that takes no input, but produces output.
 *
 * This starts with a source, and may also contain transformations that are
 * applied to the pipeline's output.
 */
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

/** A pipeline that is open at both ends.
 *
 * It receives an input, and produces an output (potentially of a different
 * type).
 *
 * It starts with a sink, ends with a source, and may contain additional
 * transformations.
 */
template <std::derived_from<Control> TIn, std::derived_from<Control> TOut>
class TransformPipeline final : public Sink<TIn>, public Source<TOut> {
 public:
  TransformPipeline() = delete;
  TransformPipeline(const SinkRef<TIn>& first, const SourceRef<TOut>& last)
    : mFirst(first), mLast(last) {
  }

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
