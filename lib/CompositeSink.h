/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <concepts>

#include "Sink.h"
#include "SinkRef.h"

namespace fredemmott::inputmapping {

template <typename TControl>
class CompositeSink final : public Sink<TControl> {
 public:
  CompositeSink(std::vector<SinkRef<TControl>> sinks): mSinks(sinks) {}

  virtual void map(typename TControl::Value value) override {
    for (auto& inner: mSinks) {
      inner->map(value);
    }
  };

 private:
  std::vector<SinkRef<TControl>> mSinks;

  template <
    std::convertible_to<SinkRef<TControl>> TFirst,
    std::convertible_to<SinkRef<TControl>>... TRest>
  void fill_sinks(const TFirst& first, TRest... rest) {
    mSinks.push_back(first);
    if constexpr (sizeof...(rest) > 0) {
      fill_sinks(rest...);
    }
  }
};

// clang-format off
template <
  convertible_to_sink_ref<Axis> TFirst,
  convertible_to_sink_ref<Axis>... TRest
>
auto all(TFirst first, TRest... rest) {
  return CompositeSink<Axis>({first, rest...});
}

template <
  convertible_to_sink_ref<Button> TFirst,
  convertible_to_sink_ref<Button>... TRest
>
auto all(TFirst first, TRest... rest) {
  return CompositeSink<Button>({first, rest...});
}

template <
  convertible_to_sink_ref<Hat> TFirst,
  convertible_to_sink_ref<Hat>... TRest
>
auto all(TFirst first, TRest... rest) {
  return CompositeSink<Hat>({first, rest...});
}
// clang-format on

}// namespace fredemmott::inputmapping
