/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "hidguardian.h"

#include "inputdevice.h"
#include "inputdevicecollection.h"

#include <iomanip>
#include <sstream>
#include <cstdio>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#include <setupapi.h>
#pragma comment(lib, "Advapi32.lib")

namespace fredemmott::gameinput {

namespace {
  std::vector<HardwareID> get_hid_ids_from_device_ids(
    const std::vector<DeviceSpecifier>& device_ids
  ) {
    InputDeviceCollection collection;
    std::vector<HardwareID> hid_ids;
    for (const auto& id: device_ids) {
      auto device = collection.get(id);
      if (!device) {
        continue;
      }
      hid_ids.push_back(device->getHardwareID());
    }
    return hid_ids;
  }

  void set_devices(const std::vector<HardwareID>& ids) {
    std::stringstream ss;
    ss << std::hex;

    for (const auto& id: ids) {
      ss << id.toString() << '\0';
    }
    ss << '\0';
    const auto value = ss.str();
    HKEY key;
    RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, 
      "SYSTEM\\CurrentControlSet\\Services\\HidGuardian\\Parameters",
      0,
      KEY_SET_VALUE,
      &key
    );
    auto res = RegSetValueEx(
      key,
      "AffectedDevices",
      NULL,
      REG_MULTI_SZ,
      (const BYTE*) value.data(),
      value.size()
    );
  }
  // Only works as administrator :'(
  void restart_devices(const std::vector<HardwareID>& ids) {
    printf("Restarting devices...\n");
    auto dil = SetupDiCreateDeviceInfoList(
      nullptr,
      NULL
    );
    SP_DEVINFO_DATA did;
    did.cbSize = sizeof(SP_DEVINFO_DATA);

    InputDeviceCollection collection;
    for (const auto& id: ids) {
      auto device = collection.get(id);
      if (!device) {
        continue;
      }
      auto instance_id = device->getInstanceID().toString();
      SetupDiOpenDeviceInfo(dil, instance_id.data(), NULL, 0, &did);
      SetupDiRestartDevices(dil, &did);
      SetupDiDeleteDeviceInfo(dil, &did);
    }
    SetupDiDestroyDeviceInfoList(dil);
    printf("Waiting for devices to settle...\n");
    Sleep(1000);
  }

  std::string process_whitelist_key() {
    const char* prefix = "SYSTEM\\CurrentControlSet\\Services\\HidGuardian\\Parameters";
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\\Whitelist\\%d", prefix, GetCurrentProcessId());
    return buf;
  }

  void whitelist_this_process() {
    auto subkey = process_whitelist_key();
    HKEY regkey;
    auto ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey.data(), NULL, NULL, REG_OPTION_VOLATILE,
        KEY_ALL_ACCESS, 
        nullptr, &regkey, nullptr
        );
    if (ret == ERROR_ACCESS_DENIED) {
      printf(
        "Access denied to HidGuardian whitelist. If you're Using HidGuardian,\n"
        "Open RegEdit as administrator, create the following key, and give\n"
        "your non-administrator user full permissions to it:\n"
        "  HKEY_LOCAL_MACHINE\\%s\nPress any key to continue.\n",
        subkey.data() 
      );
      _getch();
    }
    RegCloseKey(regkey);
  }
} // namespace

  HidGuardian::HidGuardian(const std::vector<DeviceSpecifier>& ids) {
    printf("Configuring HidGuardian...\n");
    whitelist_this_process();

    auto hid_ids = get_hid_ids_from_device_ids(ids);
    mDevices = hid_ids; 
    set_devices(hid_ids);
    restart_devices(hid_ids);
  }

  HidGuardian::~HidGuardian() {
    printf("Cleaning up HidGuardian configuration...\n");
    set_devices({});
    restart_devices(mDevices);
    auto key = process_whitelist_key();
    RegDeleteKey(HKEY_LOCAL_MACHINE, key.data());
  }

} // fredemmott::gameinput
