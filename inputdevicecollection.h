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
 private:
  std::vector<InputDevice*> mDevices;
};

} // namespace fredemmott::gameinput
