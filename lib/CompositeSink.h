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
#include "connections.h"// TODO: move convert_to_any_sink_ref etc out

namespace fredemmott::inputmapping {

template <typename TControl>
class CompositeSink final : public Sink<TControl> {
 public:
  CompositeSink(std::vector<SinkRef<TControl>> sinks) : mSinks(sinks) {
  }

  virtual void map(typename TControl::Value value) override {
    for (auto& inner: mSinks) {
      inner->map(value);
    }
  };

 private:
  std::vector<SinkRef<TControl>> mSinks;
};

template <convertible_to_any_sink_ref TFirst, typename... TRest>
auto all(TFirst first, TRest... rest) {
  auto first_ref = convert_to_any_sink_ref(std::forward<TFirst>(first));
  using TControl = typename decltype(first_ref)::element_type::InControl;
  return CompositeSink<TControl>(
    {first_ref, convert_to_any_sink_ref(std::forward<TRest>(rest))...});
}

}// namespace fredemmott::inputmapping
