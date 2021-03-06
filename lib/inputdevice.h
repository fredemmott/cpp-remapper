/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "devicespecifier.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


namespace fredemmott::gameinput {

struct AxisInformation;

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
  InstanceID getInstanceID() const;
  HardwareID getHardwareID() const;

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
