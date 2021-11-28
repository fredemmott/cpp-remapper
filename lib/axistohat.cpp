/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#define _USE_MATH_DEFINES
#include "axistohat.h"

#include <cmath>

namespace fredemmott::inputmapping {

AxisToHat::AxisToHat(uint8_t deadzone_percent) : mDeadzone(deadzone_percent) {
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
  if ((distance * 100) / Hat::CENTER < mDeadzone / 2) {
    emit(Hat::CENTER);
    return;
  }

  const auto radians = atan2(y, x);
  const auto centidegrees = std::lround((18000 * radians) / M_PI) + 9000;
  emit(centidegrees < 0 ? centidegrees + 36000 : centidegrees);
}

}// namespace fredemmott::inputmapping

#include "MappableInput.h"
#include "MappableVJoyOutput.h"
#include "connections.h"

namespace {
using namespace fredemmott::inputmapping;

void static_test() {
  MappableInput in(nullptr);
  MappableVJoyOutput out(nullptr);
  AxisToHat ath;
  // TODO: tuple assignments?
  in.XAxis >> ath.XAxis;
  in.YAxis >> ath.YAxis;
  ath >> out.Hat1;
}
}// namespace
