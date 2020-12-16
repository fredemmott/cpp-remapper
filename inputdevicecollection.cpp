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
    printf("Product Name: %s\n", device->getProductName().c_str());
    printf("Instance Name: %s\n", device->getInstanceName().c_str());
  OLECHAR* guid;
  StringFromCLSID(didevinst->guidInstance, &guid);
  printf("GUID: %S\n", guid);
  device->activate();
    auto vidpid = device->getVIDPID();
    if (vidpid) {
      printf("VID: 0x%.4x\nPID: 0x%.4x\n", vidpid->vid, vidpid->pid);
    }
    printf("Axes: %lu\n", device->getAxisCount());
    for (const auto& axis: device->getAxisInformation()) {
      printf("  - %s\n", axis.name.c_str());
    }
    printf("Buttons: %lu\n", device->getButtonCount());
    printf("Hats: %lu\n", device->getHatCount());
    printf("---\n");
    state->devices.push_back(device);

    return DIENUM_CONTINUE;
  }

} // namespace

InputDeviceCollection::InputDeviceCollection() {
  IDirectInput8* di8;
  DirectInput8Create(
      GetModuleHandle(nullptr),
      DIRECTINPUT_VERSION,
      IID_IDirectInput8,
      (LPVOID*) &di8,
      nullptr
  );
  DeviceEnumeratorState state { di8, {} };
  di8->EnumDevices(
      DI8DEVCLASS_GAMECTRL,
      &enum_device_callback,
      &state,
      DIEDFL_ATTACHEDONLY
  );
  mDevices = state.devices;
}

InputDevice* InputDeviceCollection::get(const VIDPID& vidpid) {
  for (auto device: mDevices) {
    auto dvp = device->getVIDPID();
    if (dvp && *dvp == vidpid) {
      return device;
    }
  }
  return nullptr;
}

InputDeviceCollection::~InputDeviceCollection() {
}

} // namespace fredemmott::gameinput
