/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#define _USE_MATH_DEFINES
#include "AxisToHat.h"

#include <cmath>

namespace fredemmott::inputmapping {

const Percent AxisToHat::DEFAULT_DEADZONE = 90_percent;

AxisToHat::AxisToHat(Percent deadzone) : mDeadzone(deadzone) {
}

AxisToHat::~AxisToHat() {
}

void AxisToHat::update() {
  // Recenter around (0, 0)
  const auto x = mX - Axis::MID;
  const auto y = mY - Axis::MID;

  // Treat deadzone as combined distance to center - same amount of
  // needed for corners.
  const auto distance = sqrt((x * x) + (y * y));
  if ((distance * 100) / Hat::CENTER < mDeadzone.value() / 2) {
    emit(Hat::CENTER);
    return;
  }

  const auto radians = atan2(y, x);
  const auto centidegrees = std::lround((18000 * radians) / M_PI) + 9000;
  emit(centidegrees < 0 ? centidegrees + 36000 : centidegrees);
}

}// namespace fredemmott::inputmapping
