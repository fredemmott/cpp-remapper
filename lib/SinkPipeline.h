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

}// namespace fredemmott::inputmapping
