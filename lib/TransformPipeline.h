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
#include "maybe_shared_ptr.h"

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
  TransformPipeline(const SinkRef<TIn>& first, const SourceRef<TOut>& last)
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
