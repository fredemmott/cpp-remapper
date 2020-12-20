/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "deviceid.h"

#include <vector>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace fredemmott::gameinput {

class InputDevice;

class InputDeviceCollection {
 public:
  InputDeviceCollection();
  ~InputDeviceCollection();

  InputDevice* get(const DeviceID& device);
  std::vector<InputDevice*> getAllDevices();
 private:
  std::vector<InputDevice*> mDevices;
};

} // namespace fredemmott::gameinput
