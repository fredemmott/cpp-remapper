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

  // TODO: convenience constructor deviding into equal-portion buttons.
  AxisToButtons(std::initializer_list<Range> ranges);
  void map(long value);

 private:
  struct RawRange {
    Axis::Value min;
    Axis::Value max;
    ButtonSinkRef next;
  };
  std::vector<RawRange> mRanges;
};

}// namespace fredemmott::inputmapping
