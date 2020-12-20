/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace fredemmott::gameinput {

struct AxisInformation;

struct VIDPID {
  uint16_t vid;
  uint16_t pid;
};

struct HID_ID {
  std::string id;
};

struct DeviceID {
  uint16_t vid;
  uint16_t pid;
  std::string id;

  DeviceID(const VIDPID& vidpid): vid(vidpid.vid), pid(vidpid.pid) {
    char buf[MAX_PATH];
    snprintf(buf, sizeof(buf), "HID\\VID_%04hX&PID_%04hX", vid, pid);
    id = buf;
  }

  DeviceID(const HID_ID& id): id(id.id) {
    sscanf_s(id.id.data(), "HID\\VID_%hx&PID_%hx", &vid, &pid);
  }
};


class InputDevice final {
 public:
  InputDevice(IDirectInput8* di, LPCDIDEVICEINSTANCE device);
  InputDevice() = delete;
  InputDevice(const InputDevice&) = delete;
  void operator=(const InputDevice&) = delete;
  ~InputDevice();

  std::string getInstanceName() const;
  std::string getProductName() const;
  std::optional<VIDPID> getVIDPID() const;
  std::string getInstanceID() const;
  std::string getHardwareID() const;

  uint32_t getAxisCount();
  std::vector<AxisInformation> getAxisInformation();
  uint32_t getButtonCount();
  uint32_t getHatCount();

  HANDLE getEvent();

  std::vector<uint8_t> getState();
 private:
  struct Impl;
  std::unique_ptr<Impl> p;
  HANDLE mEventHandle = nullptr;
  bool mActivated = false;
  size_t mDataSize = 0;
  void activate();

};
} // namespace fredemmott::gameinput
