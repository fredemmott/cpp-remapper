/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/Controls.h>
#include <cpp-remapper/maybe_shared_ptr.h>

namespace fredemmott::inputmapping {

class AnySource;

/** A pipeline that takes no input and produces no output.
 *
 * This pipeline starts with a source, ends with a sink, and may
 * contain compatible transformations in between the source and the sink.
 */
class ClosedPipeline final {
 public:
  ClosedPipeline() = delete;
  ClosedPipeline(const maybe_shared_ptr<AnySource>& source) : mSource(source) {
  }

 private:
  maybe_shared_ptr<AnySource> mSource;
};
}// namespace fredemmott::inputmapping
