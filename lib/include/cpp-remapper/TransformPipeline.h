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

#include <cpp-remapper/Controls.h>
#include <cpp-remapper/Sink.h>
#include <cpp-remapper/SinkPtr.h>
#include <cpp-remapper/Source.h>
#include <cpp-remapper/SourcePtr.h>
#include <cpp-remapper/maybe_shared_ptr.h>

namespace fredemmott::inputmapping {

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
  TransformPipeline(const SinkPtr<TIn>& first, const SourcePtr<TOut>& last)
    : mFirst(first), mLast(last) {
  }

  virtual void setNext(const maybe_shared_ptr<Sink<TIn>>& next) override {
    mLast->setNext(next);
  }

  virtual void map(typename TIn::Value value) override {
    mFirst->map(value);
  }

 private:
  maybe_shared_ptr<Sink<TIn>> mFirst;
  maybe_shared_ptr<Source<TOut>> mLast;
};

}// namespace fredemmott::inputmapping
