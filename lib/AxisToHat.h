/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>

#include "Percent.h"
#include "SinkPtr.h"
#include "Source.h"

namespace fredemmott::inputmapping {

/** Converts two axis to a continuous (360-degree) hat.
 *
 * Deflections > 327.67 degrees will not be shown correctly in the Windows
 * test app - use "Monitor vJoy" instead.
 */
class AxisToHat final : public HatSource {
 public:
  static const Percent DEFAULT_DEADZONE;

  AxisSinkPtr XAxis = [this](Axis::Value x) {
    mX = x;
    update();
  };
  AxisSinkPtr YAxis = [this](Axis::Value y) {
    mY = y;
    update();
  };

  AxisToHat(Percent deadzone_percent = DEFAULT_DEADZONE);
  ~AxisToHat();

 private:
  Percent mDeadzone = DEFAULT_DEADZONE;
  Axis::Value mX = Axis::MID;
  Axis::Value mY = Axis::MID;

  void update();
};

}// namespace fredemmott::inputmapping
