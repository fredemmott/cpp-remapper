/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "inputdevice.h"
#include "inputdevicecollection.h"

#include <cstdio>

int main() {
  fredemmott::gameinput::InputDeviceCollection idc;
  for (const auto& device: idc.getAllDevices()) {
    auto name = device->getProductName();
    auto vidpid = device->getVIDPID();
    auto id = device->getHardwareID();
    printf(
      "\"%s\"\n  VIDPID { 0x%04x, 0x%04x }\n  HID_ID { \"%s\" }\n",
      name.data(),
      vidpid ? vidpid->vid : 0,
      vidpid ? vidpid->pid : 0,
      id.data()
    );
    printf(
      "  Axes: %d\n  Buttons: %d\n  Hats: %d\n", 
      device->getAxisCount(),
      device->getButtonCount(),
      device->getHatCount()
    );
  }
  return 0;
}
