/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "OutputDevice.h"

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ViGEm/Client.h>
// clang-format on

namespace fredemmott::inputmapping {

class X360Device final : public OutputDevice {
 public:
  X360Device();
  ~X360Device();

  X360Device(const X360Device&) = delete;
  X360Device& operator=(const X360Device&) = delete;

  virtual void flush() override;

  enum class Button {
    DPAD_UP = XUSB_GAMEPAD_DPAD_UP,
    DPAD_DOWN = XUSB_GAMEPAD_DPAD_DOWN,
    DPAD_LEFT = XUSB_GAMEPAD_DPAD_LEFT,
    DPAD_RIGHT = XUSB_GAMEPAD_DPAD_RIGHT,
    START = XUSB_GAMEPAD_START,
    BACK = XUSB_GAMEPAD_BACK,
    LEFT_STICK = XUSB_GAMEPAD_LEFT_THUMB,
    RIGHT_STICK = XUSB_GAMEPAD_RIGHT_THUMB,
    LEFT_SHOULDER = XUSB_GAMEPAD_LEFT_SHOULDER,
    RIGHT_SHOULDER = XUSB_GAMEPAD_RIGHT_SHOULDER,
    GUIDE = XUSB_GAMEPAD_GUIDE,
    A = XUSB_GAMEPAD_A,
    B = XUSB_GAMEPAD_B,
    X = XUSB_GAMEPAD_X,
    Y = XUSB_GAMEPAD_Y
  };

  X360Device* setButton(Button button, bool value);
  X360Device* setLXAxis(long value);
  X360Device* setLYAxis(long value);
  X360Device* setRXAxis(long value);
  X360Device* setRYAxis(long value);
  X360Device* setLTrigger(long value);
  X360Device* setRTrigger(long value);

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

}// namespace fredemmott::inputmapping
