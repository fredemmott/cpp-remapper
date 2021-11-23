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

namespace fredemmott::inputmapping {

AxisToButtons::AxisToButtons(std::initializer_list<Range> ranges) {
  const long max = Axis::MAX;
  for (const auto& range: ranges) {
    mRanges.push_back(
      {(range.minPercent * max) / 100,
       (range.maxPercent * max) / 100,
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

// --- Tests ---

#include "MappableVJoyOutput.h"
#include "CompositeSink.h"
#include "connections.h"

using namespace fredemmott::inputmapping;

namespace {

// Check that the compiler lets us call it as intended
void static_test() {
  MappableVJoyOutput o(nullptr);
  AxisToButtons::Range range1 { 0, 0, o.Button1 };

  AxisToButtons::Range range2 { 0, 0, all(o.Button1, o.Button2) };
  // TODO AxisToButtons foo {
   // {0, 0, o.Button1}, {0, 0, {o.Button1, o.Button2}}, {100, 100, [](bool) {}}};
}

}// namespace
