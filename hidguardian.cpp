#include "hidguardian.h"

#include <conio.h>
#include <winreg.h>
#pragma comment(lib, "Advapi32.lib")

#include <iomanip>
#include <sstream>
#include <cstdio>

#include "inputdevicecollection.h"
#include <setupapi.h>

namespace fredemmott::gameinput {

namespace {
  // Only works as administrator :'(
  void restart_devices(const std::vector<DeviceID>& devices) {
    InputDeviceCollection collection;
    auto dil = SetupDiCreateDeviceInfoList(
      nullptr,
      NULL
    );
    SP_DEVINFO_DATA did;
    did.cbSize = sizeof(SP_DEVINFO_DATA);

    for (const auto& id: devices) {
      auto device = collection.get(id);
      if (!device) {
        continue;
      }
      auto instance_id = device->getInstanceID();
      SetupDiOpenDeviceInfo(dil, instance_id.data(), NULL, 0, &did);
      SetupDiRestartDevices(dil, &did);
      SetupDiDeleteDeviceInfo(dil, &did);
    }
    Sleep(1000);
  }

  std::string process_whitelist_key() {
    const char* prefix = "SYSTEM\\CurrentControlSet\\Services\\HidGuardian\\Parameters";
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\\Whitelist\\%d", prefix, GetCurrentProcessId());
    return buf;
  }
}

  HidGuardian::HidGuardian(const std::vector<DeviceID>& devices): mDevices(devices) {
    whitelistThisProcess();
    setDevices(devices);
    restart_devices(devices);
  }

  HidGuardian::~HidGuardian() {
    setDevices({});
    restart_devices(mDevices);
    auto key = process_whitelist_key();
    RegDeleteKey(HKEY_LOCAL_MACHINE, key.data());
  }

  void HidGuardian::whitelistThisProcess() {
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

  void HidGuardian::setDevices(const std::vector<DeviceID>& devices) {
    std::stringstream ss;
    ss << std::hex;

    for (const auto& device: devices) {
      ss << device.id << '\0';
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

} // fredemmott::gameinput
