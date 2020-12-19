/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#define NOMINMAX

#include "axistobuttons.h"

#include <limits>

namespace fredemmott::inputmapping::actions {

AxisToButtons::AxisToButtons(std::initializer_list<Range> ranges) {
  const long max = std::numeric_limits<uint16_t>::max();
  for (const auto& range: ranges) {
    mRanges.push_back({(range.minPercent * max) / 100, (range.maxPercent * max) / 100, range.next});
  }
}

void AxisToButtons::map(long value) {
  for (const auto& range: mRanges) {
    bool active = range.min <= value && range.max >= value;
    range.next->map(active);
  }
}

} // namespace fredemmott::inputmapping

// --- Tests ---

#include "comboaction.h"
#include "mappabledevices.h"
using namespace fredemmott::inputmapping;
using namespace fredemmott::inputmapping::actions;

namespace {

// Check that the compiler lets us call it as intended
void static_test() {
  MappableOutput o(nullptr);
  AxisToButtons {
    { 0, 0, o.Button1 },
    { 0, 0, { o.Button1, o.Button2 } },
    { 100, 100, [](bool) {} },
  };
}

} // namespace
