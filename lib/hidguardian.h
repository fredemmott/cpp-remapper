/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <vector>

#include "inputdevice.h"
namespace fredemmott::gameinput {
  class HidGuardian {
   public:
     HidGuardian(const std::vector<DeviceID>& devices);

     ~HidGuardian();
    private:
     void whitelistThisProcess();
     void setDevices(const std::vector<DeviceID>& devices);
     std::vector<DeviceID> mDevices;
  };

} // namespace fredemmott::gameinput
