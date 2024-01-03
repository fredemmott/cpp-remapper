/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/FAVHIDDevice.h>

#include <format>
#include <iostream>

namespace fredemmott::inputmapping {

namespace {
uint8_t gDeviceCount {};
std::optional<FAVHID::FAVJoyState2> gArduino;

void InitializeFAVHID() {
  if (gArduino) {
    return;
  }

  gArduino = FAVHID::FAVJoyState2::Open(gDeviceCount);
  if (!gArduino) {
    std::cerr << "Failed to initalize FAVHID Arduino" << std::endl;
  }
}

}// namespace

struct FAVHIDDevice::Impl final {
  uint8_t mID {};
  FAVHID::FAVJoyState2::Report mReport;
};

FAVHIDDevice::FAVHIDDevice(uint8_t id) {
  if (id >= FAVHID::FAVJoyState2::MAX_DEVICES) {
    std::cerr << std::format(
      "Ignoring FAVHIDDevice because requested ID {} >= MAX_DEVICES {}",
      id,
      FAVHID::FAVJoyState2::MAX_DEVICES)
              << std::endl;
    return;
  }

  p.reset(new Impl {.mID = id, .mReport = {}});
  gDeviceCount = std::max<uint8_t>(gDeviceCount, id + 1);
}
FAVHIDDevice::~FAVHIDDevice() = default;

void FAVHIDDevice::set(const FAVHID::FAVJoyState2::Report& report) {
  if (!p) {
    return;
  }
  p->mReport = report;
}

void FAVHIDDevice::flush() {
  if (!p) {
    return;
  }
  InitializeFAVHID();
  if (!gArduino) {
    return;
  }
  gArduino->WriteReport(p->mReport, p->mID);
}

}// namespace fredemmott::inputmapping
