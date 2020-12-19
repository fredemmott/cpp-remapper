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
