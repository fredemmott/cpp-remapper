/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cstdio>

#include "InputDevice.h"
#include "InputDeviceCollection.h"

int main() {
  fredemmott::inputmapping::InputDeviceCollection idc;
  for (const auto& device: idc.getAllDevices()) {
    auto name = device->getProductName();
    printf("\"%s\"\n", name.data());

    printf(
      "  Axes: %d\n  Buttons: %d\n  Hats: %d\n",
      device->getAxisCount(),
      device->getButtonCount(),
      device->getHatCount());

    auto vidpid = device->getVIDPID();
    if (vidpid) {
      auto buf = vidpid->getHumanReadable();
      printf("  %s\n", buf.c_str());
    }

    auto iid = device->getInstanceID().getHumanReadable();
    printf("  %s\n", iid.c_str());

    auto hid = device->getHardwareID().getHumanReadable();
    printf("  %s\n", hid.c_str());
  }
  return 0;
}
