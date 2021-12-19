/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2, x360, ds4] = create_profile(
    VPC_MT50CM3_THROTTLE,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2,
    VIGEM_X360_PAD,
    VIGEM_DS4_PAD);

  // Copy all inputs to outputs
  throttle >> vj1;
  stick >> vj2;

  // Copy axis to virtual gamepads: throttle and twist on left stick, stick xy
  // on right stick
  throttle.RXAxis >> all(vj1.RXAxis, x360.LYAxis, ds4.LYAxis);
  stick.ZAxis >> all(vj2.ZAxis, x360.LXAxis, ds4.LXAxis);

  stick.XAxis >> all(vj2.XAxis, x360.RXAxis, ds4.RXAxis);
  stick.YAxis >> all(vj2.YAxis, x360.RYAxis, ds4.RYAxis);

  // Test at least one button works :)
  stick.Button3 >> all(vj1.Button1, x360.ButtonA, ds4.ButtonCross);
  p.run();
  return 0;
}
