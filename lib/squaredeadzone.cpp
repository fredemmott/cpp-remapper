/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "squaredeadzone.h"

#include <cmath>

namespace fredemmott::inputmapping::actions {

SquareDeadzone::SquareDeadzone(
  uint8_t percent,
  const AxisEventHandler&
  next
): mPercent(percent), mNext(next) {}

SquareDeadzone::~SquareDeadzone() {}

void SquareDeadzone::map(long value) {
  // Recenter around (0, 0), then scale. The original mid
  // point is now the max
  const long MID = 0x7fff;
  value -= MID;
  const long boundary = (MID * mPercent) / 100;
  if (abs(value) < boundary) {
    mNext->map(MID);
    return;
  }
  const long new_scale = MID - boundary;
  const long scaled = ((value > 0 ? value - boundary : value + boundary) * MID) / new_scale;
  mNext->map(scaled + MID); // re-center to (MID, MID)
}

} // namespace fredemmott::inputmapping::actions
