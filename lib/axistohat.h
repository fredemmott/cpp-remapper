/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <cstdint>

namespace fredemmott::inputmapping::actions {

/** Converts two axis to a continuous (360-degree) hat.
 *
 * Deflections > 327.67 degrees will not be shown correctly in the Windows
 * test app - use "Monitor vJoy" instead.
 */
class AxisToHat {
  public:
    static const uint8_t DEFAULT_DEADZONE_PERCENT = 90;
    const AxisEventHandler XAxis =
      [this](long x) { mX = x; update(); };
    const AxisEventHandler YAxis =
      [this](long y) { mY = y; update(); };

    AxisToHat(
      const HatEventHandler& next,
      uint8_t deadzone_percent = DEFAULT_DEADZONE_PERCENT
    );
    ~AxisToHat();
  private:
    HatEventHandler mNext;
    uint8_t mDeadzone = DEFAULT_DEADZONE_PERCENT;
    long mX = 0;
    long mY = 0;

    void update();
};

} // namespace fredemmott::inputmapping
