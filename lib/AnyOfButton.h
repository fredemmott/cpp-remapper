/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "Sink.h"
#include "SinkPtr.h"
#include "Source.h"
#include "SourcePtr.h"
#include "connections.h"

namespace fredemmott::inputmapping {

class AnyOfButton final : public Sink<Button>, public Source<Button> {
 private:
  uint16_t mPressed = 0;

 public:
  virtual void map(bool pressed) override;
};

template <typename First, typename... Rest>
auto any(First&& first, Rest&&... rest) {
  std::vector<ButtonSourcePtr> inners {
    std::forward<First>(first), std::forward<Rest>(rest)...};
  auto impl = std::make_shared<AnyOfButton>();
  auto as_sink = std::static_pointer_cast<ButtonSink>(impl);
  for (auto& inner: inners) {
    inner >> as_sink;
  }
  return std::static_pointer_cast<ButtonSource>(impl);
}

}// namespace fredemmott::inputmapping
