/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "axistohat.h"

namespace fredemmott::inputmapping::actions {

AxisToHat::AxisToHat(
  const HatEventHandler& next,
  uint8_t deadzone_percent
): mNext(next), mDeadzone(deadzone_percent) {
}

AxisToHat::~AxisToHat() {}

void AxisToHat::update() {
  const auto mid = 0xffff / 2;
  // Recenter around (0, 0)
  const auto x = mX - mid;
  const auto y = mY - mid;

  // Treat deadzone as combined distance to center - same amount of
  // needed for corners.
  const auto distance = sqrt((x*x) + (y*y));
  if ((distance * 100) / 0xffff < mDeadzone / 2) {
    mNext->map(0xffff); // center
    return;
  }
  const auto radians = atan2(y, x);
  const auto raw_degrees = ((180 / M_PI) * radians) + 90;
  const auto degrees = (raw_degrees < 0) ? raw_degrees + 360 : raw_degrees;
  mNext->map((long) degrees * 100);
}

} // namespace fredemmott::inputmapping::actions

#include "Mapper.h"
#include "MappableInput.h"
#include "MappableVJoyOutput.h"

namespace {
  using namespace fredemmott::inputmapping::actions;
  using namespace fredemmott::inputmapping;
  void static_test() {
    Mapper m;
    MappableInput i(nullptr);
    MappableVJoyOutput o(1);

    AxisToHat ignored(o.Hat1);
    AxisToHat ath(o.Hat2, 20); // custom deadzone percent
    m.map(i.XAxis, ath.XAxis);
    m.map(i.YAxis, ath.YAxis);
  }
} // namespace
