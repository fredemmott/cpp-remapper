/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>
#include <vector>

#include "SinkRef.h"

namespace fredemmott::inputmapping {

class AxisToButtons final : public AxisSink {
 public:
  struct Range {
    uint8_t minPercent;
    uint8_t maxPercent;
    ButtonSinkRef next;
  };

  AxisToButtons(const std::vector<Range>& ranges);
  AxisToButtons(std::initializer_list<Range> ranges);

  // clang-format off
  template <typename First, convertible_to_sink_ptr<Button>... Rest>
  requires
    // clang needs this as the first constraint on First to avoid a
    // template/concept recursion bug
    (!std::same_as<AxisToButtons, std::decay_t<First>>)
    && convertible_to_sink_ptr<First, Button>
  // clang-format on
  AxisToButtons(First&& first, Rest... rest) {
    std::vector<ButtonSinkRef> buttons {
      convert_to_any_sink_ptr(std::forward<First>(first)),
      convert_to_any_sink_ptr(std::forward<Rest>(rest))...};
    const auto step = ((long double)Axis::MAX) / buttons.size();
    for (auto i = 0; i < buttons.size(); ++i) {
      mRanges.push_back(
        {std::lround(i * step) + (i == 0 ? 0 : 1),
         std::lround((i + 1) * step),
         buttons[i]});
    }
  }

  virtual void map(long value) override;

 private:
  struct RawRange {
    Axis::Value min;
    Axis::Value max;
    ButtonSinkRef next;
  };
  std::vector<RawRange> mRanges;
};

}// namespace fredemmott::inputmapping
