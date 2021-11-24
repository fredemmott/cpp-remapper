/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "devicespecifier.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace fredemmott::gameinput {

struct AxisInformation;

class InputDevice final {
 public:
  InputDevice(IDirectInput8A* di, LPCDIDEVICEINSTANCEA device);
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

  // TODO move to Stateclass
  struct StateOffsets {
    off_t firstAxis;
    off_t firstButton;
    off_t firstHat;
  };

  class State final {
   private:
    StateOffsets offsets;
    std::vector<std::byte> buffer;
   public:
    State(const StateOffsets& offsets, const std::vector<std::byte>& buffer);
    ~State();

    long getAxis(uint8_t i) const;
    bool getButton(uint8_t i) const;
    uint16_t getHat(uint8_t i) const;
  };
  State getState();

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
  HANDLE mEventHandle = nullptr;
  bool mActivated = false;
  size_t mDataSize = 0;
  StateOffsets mOffsets {};
  void activate();
};
}// namespace fredemmott::gameinput
