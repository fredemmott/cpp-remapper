/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "inputdevice.h"

namespace fredemmott::gameinput::devicedb {
  const VIDPID TM_WARTHOG_THROTTLE { 0x044f, 0x0404 };
  const VIDPID TM_TPR { 0x044f, 0xb68f };
  const HardwareID VPC_LEFT_WARBRD { "HID\\VID_3344&PID_80CC&Col01" };
  const HardwareID VPC_RIGHT_WARBRD { "HID\\VID_3344&PID_40CC&Col01" };
  const HardwareID VPC_MT50CM3_THROTTLE { "HID\\VID_3344&PID_0194&Col01" };
}
