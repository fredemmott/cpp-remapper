/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>

#include "actionsapi.h"

namespace fredemmott::inputmapping {

/** Converts two axis to a continuous (360-degree) hat.
 *
 * Deflections > 327.67 degrees will not be shown correctly in the Windows
 * test app - use "Monitor vJoy" instead.
 */
class AxisToHat {
 public:
  static const uint8_t DEFAULT_DEADZONE_PERCENT = 90;
  AxisSinkRef XAxis = [this](Axis::Value x) {
    mX = x;
    update();
  };
  AxisSinkRef YAxis = [this](Axis::Value y) {
    mY = y;
    update();
  };

  AxisToHat(
    const HatSinkRef& next,
    uint8_t deadzone_percent = DEFAULT_DEADZONE_PERCENT);
  ~AxisToHat();

 private:
  HatSinkRef mNext;
  uint8_t mDeadzone = DEFAULT_DEADZONE_PERCENT;
  Axis::Value mX = 0;
  Axis::Value mY = 0;

  void update();
};

}// namespace fredemmott::inputmapping
