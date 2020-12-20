/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once
#include "deviceid.h"

#include "inputdevice.h"

namespace fredemmott::gameinput {

DeviceSpecifierBase::~DeviceSpecifierBase() {}

VIDPID::~VIDPID() {}

bool VIDPID::matches(InputDevice* device) const {
  auto dev_vidpid = device->getVIDPID();
  if (!dev_vidpid) {
    return false;
  }
  return mVID == dev_vidpid->mVID && mPID == dev_vidpid->mPID;
}

std::string VIDPID::getHumanReadable() const {
  char buf[sizeof("VID: 0x1234, PID: 0x5678")];
  snprintf(buf, sizeof(buf), "VID: 0x%04hX, PID:: 0x%04hX", mVID, mPID);
  return buf;

}

HID_ID::~HID_ID() {}

bool HID_ID::matches(InputDevice* device) const {
  return mID == device->getHardwareID().mID;
}

std::string HID_ID::getHumanReadable() const {
  return "Hardware ID: " + mID;
}

namespace {
  const DeviceSpecifierBase& unwrap_impl(const DeviceSpecifier::Impl& impl) {
    return std::visit(
      [&](const DeviceSpecifierBase& base) -> const DeviceSpecifierBase& {
        return base;
      },
      impl
    );
  }
} // namespace

DeviceSpecifier::~DeviceSpecifier() {}
bool DeviceSpecifier::matches(InputDevice* device) const {
  return unwrap_impl(p).matches(device);
}
std::string DeviceSpecifier::getHumanReadable() const {
  return unwrap_impl(p).getHumanReadable();
}

} // namespace fredemmott::gameinput
