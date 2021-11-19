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

#include "devicespecifier.h"

namespace fredemmott::gameinput {
class InputDevice;

class HidHide {
 public:
  HidHide(const std::vector<DeviceSpecifier>& devices);

  ~HidHide();

 private:
  bool mInitialized = false;
  std::vector<InstanceID> mInstances;

  void init(const std::vector<DeviceSpecifier>& devices);
  void deinit();
};

}// namespace fredemmott::gameinput
