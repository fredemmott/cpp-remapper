/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <vector>

#include "deviceid.h"

namespace fredemmott::gameinput {
  class InputDevice;

  class HidGuardian {
   public:
     HidGuardian(const std::vector<DeviceID>& devices);

     ~HidGuardian();
    private:
     std::vector<HID_ID> mDevices;
  };

} // namespace fredemmott::gameinput
