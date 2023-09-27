/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/InputDevice.h>

namespace fredemmott::inputmapping::devicedb {
const VIDPID TM_WARTHOG_THROTTLE {0x044f, 0x0404};
const VIDPID TM_TPR {0x044f, 0xb68f};
const HardwareID VPC_LEFT_WARBRD {"HID\\VID_3344&PID_80CC&Col01"};
const HardwareID VPC_RIGHT_WARBRD {"HID\\VID_3344&PID_40CC&Col01"};
const HardwareID VPC_MT50CM3_THROTTLE {"HID\\VID_3344&PID_0194&Col01"};
const HardwareID VPC_MT50CM2_SHARKA_CYCLIC {"HID\\VID_3344&PID_012E&Col01"};
const HardwareID VPC_MT50CM2_TM_STICK {"HID\\VID_03EB&PID_2045"};
const HardwareID VPC_ROTOR_TCS_PLUS {"HID\\VID_3344&PID_02C3&Col01"};

// Useful either for interacting with other software, or for hiding unused VJoy
// devices
const HardwareID VJOY_1_IN {"HID\\HIDCLASS&Col01"},
  VJOY_2_IN {"HID\\HIDCLASS&Col02"}, VJOY_3_IN {"HID\\HIDCLASS&Col03"},
  VJOY_4_IN {"HID\\HIDCLASS&Col04"}, VJOY_5_IN {"HID\\HIDCLASS&Col05"},
  VJOY_6_IN {"HID\\HIDCLASS&Col06"}, VJOY_7_IN {"HID\\HIDCLASS&Col07"},
  VJOY_8_IN {"HID\\HIDCLASS&Col08"}, VJOY_9_IN {"HID\\HIDCLASS&Col09"},
  VJOY_10_IN {"HID\\HIDCLASS&Col10"}, VJOY_11_IN {"HID\\HIDCLASS&Col11"},
  VJOY_12_IN {"HID\\HIDCLASS&Col12"}, VJOY_13_IN {"HID\\HIDCLASS&Col13"},
  VJOY_14_IN {"HID\\HIDCLASS&Col14"}, VJOY_15_IN {"HID\\HIDCLASS&Col15"},
  VJOY_16_IN {"HID\\HIDCLASS&Col16"};
}// namespace fredemmott::inputmapping::devicedb
