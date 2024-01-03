/*
 *r Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <Windows.h>
#include <cpp-remapper/HidHide.h>
#include <cpp-remapper/InputDevice.h>
#include <cpp-remapper/InputDeviceCollection.h>
#include <winioctl.h>

#include <cstdio>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>

#pragma comment(lib, "Ole32")
#pragma comment(lib, "WindowsApp")

namespace {
constexpr unsigned int IoControlDeviceType {32769};

// clang-format off
    constexpr auto IOCTL_GET_WHITELIST { CTL_CODE(IoControlDeviceType, 2048, METHOD_BUFFERED, FILE_READ_DATA) };
    constexpr auto IOCTL_SET_WHITELIST { CTL_CODE(IoControlDeviceType, 2049, METHOD_BUFFERED, FILE_READ_DATA) };
    constexpr auto IOCTL_GET_BLACKLIST { CTL_CODE(IoControlDeviceType, 2050, METHOD_BUFFERED, FILE_READ_DATA) };
    constexpr auto IOCTL_SET_BLACKLIST { CTL_CODE(IoControlDeviceType, 2051, METHOD_BUFFERED, FILE_READ_DATA) };
    constexpr auto IOCTL_GET_ACTIVE    { CTL_CODE(IoControlDeviceType, 2052, METHOD_BUFFERED, FILE_READ_DATA) };
    constexpr auto IOCTL_SET_ACTIVE    { CTL_CODE(IoControlDeviceType, 2053, METHOD_BUFFERED, FILE_READ_DATA) };
// clang-format on
}// namespace

namespace fredemmott::inputmapping {
HidHide::HidHide(const std::vector<DeviceSpecifier>& specifiers) {
  printf("Configuring HidHide...\n");
  try {
    init(specifiers);
    mInitialized = true;
  } catch (const std::runtime_error& e) {
    fprintf(
      stderr,
      "---\n"
      "!!! WARNING !!!\n"
      "HidHide failed to initialize: %s\n"
      "\n"
      "Close any other programs using HidHide (including the configuration "
      "utility) and try again.\n"
      "---\n",
      e.what());
  }
}

void HidHide::init(const std::vector<DeviceSpecifier>& specifiers) {
  InputDeviceCollection collection;
  if (!getControlDevice()) {
    std::cout << "Couldn't connect to HidHide, ignoring" << std::endl;
    return;
  }

  ensureApplicationIsWhitelisted();
  for (const auto& specifier: specifiers) {
    auto device = collection.get(specifier);
    if (!device) {
      continue;
    }
    auto instance = device->getInstanceID();
    mEntries.push_back({instance.toWString()});
  }
}

HidHide::~HidHide() {
  if (!mInitialized) {
    printf("Skipping HidHide cleanup - failed to initialize on startup.\n");
    return;
  }

  try {
    printf("Cleaning up HidHide configuration...\n");
    mEntries.clear();
    printf("...cleaned up HidHide.\n");
  } catch (const std::runtime_error& e) {
    fprintf(
      stderr,
      "---\n"
      "!!! WARNING !!!\n"
      "Failed to clean up HidHide: %s\n"
      "\n"
      "Close any other programs using HidHide (including the configuration "
      "utility) and try again.\n"
      "---\n",
      e.what());
  }
}

HidHide::ControlDevice HidHide::getControlDevice() {
  return ControlDevice {CreateFileW(
    L"\\\\.\\HidHide",
    GENERIC_READ,
    (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE),
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL)};
}

static std::set<std::wstring> bufferToWStringSet(std::string_view buffer) {
  std::wstring_view bufferDebug {
    reinterpret_cast<const wchar_t*>(buffer.data()),
    buffer.size() * sizeof(wchar_t)};

  std::set<std::wstring> ret;
  const auto begin = reinterpret_cast<const wchar_t* const>(buffer.data());
  const auto end
    = reinterpret_cast<const wchar_t* const>(buffer.data() + buffer.size());

  for (auto it = begin; it < end;) {
    if (!*it) {
      it++;
      continue;
    }
    const std::wstring string(it);
    ret.emplace(string);
    it += string.size() + 1;
  }
  return ret;
}

static std::string getIoctl(const HidHide::ControlDevice& device, DWORD ioctl) {
  DWORD size;
  winrt::check_bool(DeviceIoControl(
    device.get(), ioctl, nullptr, 0, nullptr, 0, &size, nullptr));
  std::string buffer(size, '\0');
  winrt::check_bool(DeviceIoControl(
    device.get(),
    ioctl,
    nullptr,
    0,
    buffer.data(),
    static_cast<DWORD>(buffer.size()),
    &size,
    nullptr));
  if (buffer.size() != size) {
    throw std::runtime_error(
      std::format("Mismatched HidHide size: {} != {}", size, buffer.size()));
  }
  return buffer;
}

std::set<std::wstring> HidHide::getDeviceBlacklist(
  const ControlDevice& device) {
  return bufferToWStringSet(getIoctl(device, IOCTL_GET_BLACKLIST));
}

static std::vector<char> wstringSetToBuffer(
  const std::set<std::wstring>& stringSet) {
  size_t wcharCount = stringSet.size() + 1;
  for (const auto& str: stringSet) {
    wcharCount += str.size();
  }
  std::vector<char> buffer(wcharCount * sizeof(wchar_t), L'\0');

  const auto begin = reinterpret_cast<wchar_t*>(buffer.data());
  const auto end = reinterpret_cast<wchar_t*>(buffer.data() + buffer.size());

  std::wstring_view bufferDebug {begin, static_cast<size_t>(end - begin)};
  auto it = begin;

  for (const auto& string: stringSet) {
    memcpy_s(
      it,
      (end - it) * sizeof(wchar_t),
      string.data(),
      string.size() * sizeof(wchar_t));
    it += string.size() + 1;
  }

  return buffer;
}

void HidHide::setDeviceBlacklist(
  const ControlDevice& device,
  const std::set<std::wstring>& blacklist) {
  auto buffer = wstringSetToBuffer(blacklist);
  winrt::check_bool(DeviceIoControl(
    device.get(),
    IOCTL_SET_BLACKLIST,
    buffer.data(),
    buffer.size(),
    nullptr,
    0,
    nullptr,
    nullptr));
}

HidHide::DeviceBlacklistEntry::DeviceBlacklistEntry(
  const std::wstring& device) {
  if (device.empty()) {
    return;
  }
  auto controlDevice = getControlDevice();
  auto blacklist = getDeviceBlacklist(controlDevice);
  if (blacklist.contains(device)) {
    return;
  }
  blacklist.emplace(device);
  setDeviceBlacklist(controlDevice, blacklist);
  mDevice = device;
}

HidHide::DeviceBlacklistEntry::DeviceBlacklistEntry(
  DeviceBlacklistEntry&& other) {
  *this = std::move(other);
}

HidHide::DeviceBlacklistEntry& HidHide::DeviceBlacklistEntry::operator=(
  DeviceBlacklistEntry&& other) {
  mDevice = other.mDevice;
  other.mDevice.clear();

  return *this;
}

HidHide::DeviceBlacklistEntry::~DeviceBlacklistEntry() {
  if (mDevice.empty()) {
    return;
  }
  auto controlDevice = getControlDevice();
  auto blacklist = getDeviceBlacklist(controlDevice);
  auto it = blacklist.find(mDevice);
  if (it == blacklist.end()) {
    return;
  }
  blacklist.erase(it);
  setDeviceBlacklist(controlDevice, blacklist);
}

std::set<HidHide::AbsoluteDosDevicePath> HidHide::getApplicationWhitelist(
  const ControlDevice& controlDevice) {
  return bufferToWStringSet(getIoctl(controlDevice, IOCTL_GET_WHITELIST));
}

void HidHide::setApplicationWhitelist(
  const ControlDevice& device,
  const std::set<AbsoluteDosDevicePath>& whitelist) {
  auto buffer = wstringSetToBuffer(whitelist);
  auto bufferDebug = std::wstring_view(
    reinterpret_cast<wchar_t*>(buffer.data()), buffer.size() * sizeof(wchar_t));
  winrt::check_bool(DeviceIoControl(
    device.get(),
    IOCTL_SET_WHITELIST,
    buffer.data(),
    buffer.size(),
    nullptr,
    0,
    nullptr,
    nullptr));
}

void HidHide::ensureApplicationIsWhitelisted() {
  wchar_t pathBuf[MAX_PATH];
  auto pathLen = GetModuleFileNameW(NULL, pathBuf, MAX_PATH);
  std::filesystem::path path {std::wstring_view {pathBuf, pathLen}};
  ensureApplicationIsWhitelisted(std::filesystem::canonical(path));
}

void HidHide::ensureApplicationIsWhitelisted(
  const std::filesystem::path& path) {
  if (path.empty()) {
    return;
  }

  if (!path.is_absolute()) {
    throw std::logic_error("Path must be absolute");
  }

  auto pathStr = path.wstring();

  wchar_t volumeName[MAX_PATH];

  using FindVolumeHandle = std::
    unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&FindVolumeClose)>;
  FindVolumeHandle findVolumeHandle {nullptr, &FindVolumeClose};
  {
    auto rawFindVolumeHandle
      = FindFirstVolumeW(volumeName, std::size(volumeName));
    if (rawFindVolumeHandle == INVALID_HANDLE_VALUE || !rawFindVolumeHandle) {
      return;
    }
    findVolumeHandle.reset(rawFindVolumeHandle);
  }

  size_t bestMountPointLength = 0;
  std::wstring bestVolumeName;
  std::wstring bestMountPoint;
  do {
    std::wstring pathNames(UNICODE_STRING_MAX_CHARS, L'\0');
    DWORD pathNamesLength;
    if (!GetVolumePathNamesForVolumeNameW(
          volumeName,
          pathNames.data(),
          UNICODE_STRING_MAX_CHARS,
          &pathNamesLength)) {
      continue;
    }
    std::string_view bufferView {
      reinterpret_cast<char*>(pathNames.data()),
      pathNamesLength * sizeof(wchar_t)};
    for (const auto& volumePath: bufferToWStringSet(bufferView)) {
      if (
        volumePath.size() > bestMountPointLength
        && pathStr.starts_with(volumePath)) {
        bestMountPointLength = volumePath.size();
        bestMountPoint = volumePath;
        bestVolumeName = volumeName;
      }
    }
  } while (
    FindNextVolumeW(findVolumeHandle.get(), volumeName, std::size(volumeName)));

  if (bestVolumeName.empty()) {
    return;
  }

  // volumeName is like `\\?\Volume{GUID}\\`
  // we just want "Volume{GUID}"
  std::wstring targetPathBuf(MAX_PATH, L'\0');
  auto targetPathLen = QueryDosDeviceW(
    bestVolumeName.substr(4, bestVolumeName.size() - 5).c_str(),
    targetPathBuf.data(),
    MAX_PATH);
  // Terminated by a null wchar (2 bytes)... then another null
  // wchar
  targetPathLen -= 2;

  auto withoutMount = pathStr.substr(bestMountPoint.size());
  AbsoluteDosDevicePath absolutePath = std::format(
    L"{}\\{}",
    std::wstring_view {targetPathBuf.data(), targetPathLen},
    withoutMount);

  std::wcout << std::format(
    L"  Adding '{}' to HidHide application whitelist", absolutePath)
             << std::endl;
  auto controlDevice = getControlDevice();
  auto whitelist = getApplicationWhitelist(controlDevice);
  if (whitelist.contains(absolutePath)) {
    return;
  }
  whitelist.emplace(absolutePath);
  setApplicationWhitelist(controlDevice, whitelist);
}

}// namespace fredemmott::inputmapping
