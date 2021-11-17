/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#define _NO_MIN_MAX
#include "squaredeadzone.h"

namespace fredemmott::inputmapping::actions {

SquareDeadzone::SquareDeadzone(
  uint8_t percent
): mPercent(percent) {}

SquareDeadzone::~SquareDeadzone() {}

void SquareDeadzone::map(long value) {
  // Re-scale to 100x to avoid dividing by 100 all over the place when dealing
  // with percentages
  value *= 100;
  const uint32_t MAX = 0xffff * 100;
  const uint32_t MID = MAX / 2;
  const uint32_t DEAD = (mPercent * MID) / 100;
  if (value > (MID - DEAD) && value < (MID + DEAD)) {
    emit(MID / 100);
    return;
  }
  const uint32_t NEW_MAX = MAX - (2 * DEAD);
  const uint32_t without_deadzone = (value < MID) ? value : value - (2 * DEAD);
  // We end up with a temporary where the value is multiplied by 100 * MAX
  // * NEW_MAX; if the percentage is large, this overflows the uint32
  const uint32_t scaled = (without_deadzone * (uint64_t) MAX) / NEW_MAX;
  emit(scaled / 100);
}

} // namespace fredemmott::inputmapping::actions
