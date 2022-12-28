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
#include <cpp-remapper/maybe_shared_ptr.h>

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
  SinkPipeline(const SinkPtr<TControl>& first) : mFirst(first) {
  }

  void map(typename TControl::Value value) override {
    mFirst->map(value);
  };

 private:
  maybe_shared_ptr<Sink<TControl>> mFirst;
};

}// namespace fredemmott::inputmapping
