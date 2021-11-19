/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  const InstanceID TM_THROTTLE_INSTANCE {
    "HID\\VID_044F&PID_0404\\9&1e87ca54&1&0000"};
  auto [p, throttle, stick, vj1, vj2]
    = create_profile(TM_THROTTLE_INSTANCE, VPC_RIGHT_WARBRD, VJOY_1, VJOY_2);

  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  // Replace the ministick mappings on both devices to additional buttons
  throttle.XAxis >> AxisToButtons {
    {0, 0, vj1.button(throttle.getButtonCount() + 1)},
    {100, 100, vj1.button(throttle.getButtonCount() + 2)}};
  throttle.YAxis >> AxisToButtons {
    {0, 0, vj1.button(throttle.getButtonCount() + 3)},
    {100, 100, vj1.button(throttle.getButtonCount() + 4)}};
  stick.RXAxis >> AxisToButtons {
    {0, 0, vj2.button(stick.getButtonCount() + 1)},
    {100, 100, vj2.button(stick.getButtonCount() + 2)}};
  stick.RYAxis >> AxisToButtons {
    {0, 0, vj2.button(stick.getButtonCount() + 3)},
    {100, 100, vj2.button(stick.getButtonCount() + 4)}};

  stick.Button12 >> ShortPressLongPress {vj2.Button2, vj2.Button3};
  p->run();
  return 0;
}
