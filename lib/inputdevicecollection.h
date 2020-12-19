/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

namespace fredemmott::gameinput {

class InputDevice;
struct DeviceID;

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
