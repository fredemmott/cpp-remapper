/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <filesystem>
#include <memory>
#include <set>
#include <vector>

#include "DeviceSpecifier.h"

// clang-format off
#include <Unknwn.h>
#include <winrt/base.h>
// clang-format on

namespace fredemmott::inputmapping {
class InputDevice;

class HidHide {
 public:
  using ControlDevice = winrt::file_handle;
  using AbsoluteDosDevicePath = std::wstring;

  HidHide(const std::vector<DeviceSpecifier>& devices);

  ~HidHide();

 private:
  bool mInitialized = false;

  void init(const std::vector<DeviceSpecifier>& devices);

  static ControlDevice getControlDevice();

  static std::set<std::wstring> getDeviceBlacklist(const ControlDevice&);
  static void setDeviceBlacklist(
    const ControlDevice&,
    const std::set<std::wstring>&);

  static std::set<AbsoluteDosDevicePath> getApplicationWhitelist(
    const ControlDevice&);
  static void setApplicationWhitelist(
    const ControlDevice&,
    const std::set<AbsoluteDosDevicePath>&);
  static void ensureApplicationIsWhitelisted();
  static void ensureApplicationIsWhitelisted(const std::filesystem::path&);

  class DeviceBlacklistEntry {
   public:
    DeviceBlacklistEntry(const std::wstring&);
    DeviceBlacklistEntry(DeviceBlacklistEntry&&);
    ~DeviceBlacklistEntry();

    DeviceBlacklistEntry& operator=(DeviceBlacklistEntry&&);

    DeviceBlacklistEntry(const DeviceBlacklistEntry&) = delete;
    auto operator=(const DeviceBlacklistEntry&) = delete;

   private:
    std::wstring mDevice;
  };

  std::vector<DeviceBlacklistEntry> mEntries;
};

}// namespace fredemmott::inputmapping
