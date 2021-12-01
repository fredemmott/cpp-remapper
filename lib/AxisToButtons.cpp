/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "AxisToButtons.h"

#include <limits>

namespace fredemmott::inputmapping {

AxisToButtons::AxisToButtons(std::initializer_list<AxisToButtons::Range> ranges)
  : AxisToButtons(std::vector(ranges)) {
}

AxisToButtons::AxisToButtons(const std::vector<Range>& ranges) {
  const long max = Axis::MAX;
  for (const auto& range: ranges) {
    mRanges.push_back(
      {(range.min.value() * max) / 100,
       (range.max.value() * max) / 100,
       range.next});
  }
}

void AxisToButtons::map(long value) {
  for (auto& range: mRanges) {
    bool active = range.min <= value && range.max >= value;
    range.next->map(active);
  }
}

}// namespace fredemmott::inputmapping
