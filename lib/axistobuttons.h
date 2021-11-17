/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

#include <vector>
#include <cstdint>

namespace fredemmott::inputmapping::actions {

class AxisToButtons final : public AxisSink {
 public:
  struct Range {
    uint8_t minPercent;
    uint8_t maxPercent;
    ButtonOutput next;
  };

  AxisToButtons(std::initializer_list<Range> ranges);
  void map(long value);
 private:
  struct RawRange {
    long min;
    long max;
    ButtonOutput next;
  };
  std::vector<RawRange> mRanges;
};

} // namespace fredemmott::inputmapping
