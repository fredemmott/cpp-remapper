/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "axiscurve.h"
#include "axistobuttons.h"
#include "axistohat.h"
#include "ButtonToAxis.h"
#include "shortpresslongpress.h"
#include "squaredeadzone.h"

namespace fredemmott::inputmapping::actions {
  AxisEventHandler invert(AxisEventHandler next) {
    return [next](long value) { next->map(65535 - value); };
  }

  ButtonEventHandler invert(ButtonEventHandler next) {
    return [next](bool value) { next->map(!value); };
  }
}
