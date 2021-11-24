/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "Controls.h"
#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

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

}// namespace fredemmott::inputmapping
