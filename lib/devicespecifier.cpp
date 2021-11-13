/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once
#include "devicespecifier.h"

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
  snprintf(buf, sizeof(buf), "VID: 0x%04hX, PID: 0x%04hX", mVID, mPID);
  return buf;

}

StringBasedID::StringBasedID(const std::string& impl) : mID(impl) {}

StringBasedID::~StringBasedID() {}

std::string StringBasedID::toString() const {
  return mID;
}

std::wstring StringBasedID::toWString() const {
  wchar_t buf[UNICODE_STRING_MAX_CHARS];
  const auto wchar_count = ::MultiByteToWideChar(
    1252,
    0,
    mID.data(),
    mID.size(),
    buf,
    sizeof(buf)
  );
  return std::wstring(buf, wchar_count);
}

bool HardwareID::matches(InputDevice* device) const {
  return mID == device->getHardwareID().mID;
}

std::string HardwareID::getHumanReadable() const {
  return "HardwareID: " + mID;
}

bool InstanceID::matches(InputDevice* device) const {
  return mID == device->getInstanceID().mID;
}

std::string InstanceID::getHumanReadable() const {
  return "InstanceID: " + mID;
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
