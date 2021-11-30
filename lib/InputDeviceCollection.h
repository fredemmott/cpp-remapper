/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <memory>
#include <vector>

#include "DeviceSpecifier.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace fredemmott::inputmapping {

class InputDevice;

class InputDeviceCollection {
 public:
  InputDeviceCollection();
  ~InputDeviceCollection();

  std::shared_ptr<InputDevice> get(const DeviceSpecifier& device);
  std::vector<std::shared_ptr<InputDevice>> getAllDevices();

 private:
  std::vector<std::shared_ptr<InputDevice>> mDevices;
};

}// namespace fredemmott::inputmapping
