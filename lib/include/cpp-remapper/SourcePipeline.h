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
#include <cpp-remapper/Source.h>

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
    const maybe_shared_ptr<AnySource>& first,
    const maybe_shared_ptr<Source<TControl>>& last)
    : mFirst(first), mLast(last) {};

  virtual void setNext(const maybe_shared_ptr<Sink<TControl>>& next) override {
    mLast->setNext(next);
  }

 private:
  maybe_shared_ptr<AnySource> mFirst;
  maybe_shared_ptr<Source<TControl>> mLast;
};

}// namespace fredemmott::inputmapping
