/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "inputdevicecollection.h"

#include "axistypes.h"
#include "inputdevice.h"
#pragma comment(lib, "Ole32.lib") // GUID

namespace fredemmott::gameinput {

namespace {
  struct DeviceEnumeratorState {
    IDirectInput8* di8;
    std::vector<InputDevice*> devices;
  };

  BOOL CALLBACK enum_device_callback(
    LPCDIDEVICEINSTANCE didevinst,
    LPVOID vpRef
  ) {
    auto state = reinterpret_cast<DeviceEnumeratorState*>(vpRef);
    auto device = new InputDevice(state->di8, didevinst);
    state->devices.push_back(device);

    return DIENUM_CONTINUE;
  }

  IDirectInput8* gDI8 = nullptr;
} // namespace

InputDeviceCollection::InputDeviceCollection() {
  if (!gDI8) {
    DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (LPVOID*) &gDI8,
        nullptr
    );
  }
  auto di8 = gDI8;
  DeviceEnumeratorState state { di8, {} };
  di8->EnumDevices(
      DI8DEVCLASS_GAMECTRL,
      &enum_device_callback,
      &state,
      DIEDFL_ATTACHEDONLY
  );
  mDevices = state.devices;
}

InputDevice* InputDeviceCollection::get(const DeviceID& id) {
  for (auto device: mDevices) {
    if (id.matches(device)) {
      return device;
    }
  }
  auto desc = id.getHumanReadable();
  printf("WARNING: Failed to find device '%s'\n", desc.c_str());
  return nullptr;
}

std::vector<InputDevice*> InputDeviceCollection::getAllDevices() {
  return mDevices;
}

InputDeviceCollection::~InputDeviceCollection() {
  for (auto device: mDevices) {
    delete device;
  }
}

} // namespace fredemmott::gameinput
