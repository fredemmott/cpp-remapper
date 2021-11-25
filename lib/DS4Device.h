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

class DS4Device final : public OutputDevice {
 public:
  DS4Device();
  ~DS4Device();

  DS4Device(const DS4Device&) = delete;
  DS4Device& operator=(const DS4Device&) = delete;

  virtual void flush() override;

  enum class Button {
    LEFT_THUMB = DS4_BUTTON_THUMB_LEFT,
    RIGHT_THUMB = DS4_BUTTON_THUMB_RIGHT,
    OPTIONS = DS4_BUTTON_OPTIONS,
    SHARE = DS4_BUTTON_SHARE,
    LEFT_TRIGGER = DS4_BUTTON_TRIGGER_LEFT,
    RIGHT_TRIGGER = DS4_BUTTON_TRIGGER_RIGHT,
    LEFT_SHOULDER = DS4_BUTTON_SHOULDER_LEFT,
    RIGHT_SHOULDER = DS4_BUTTON_SHOULDER_LEFT,
    TRIANGLE = DS4_BUTTON_TRIANGLE,
    CIRCLE = DS4_BUTTON_CIRCLE,
    CROSS = DS4_BUTTON_CROSS,
    SQUARE = DS4_BUTTON_SQUARE,
  };

  enum class SpecialButton {
    PS = DS4_SPECIAL_BUTTON_PS,
    TOUCHPAD = DS4_SPECIAL_BUTTON_TOUCHPAD,
  };

  enum class DPadDirection {
    NONE = DS4_BUTTON_DPAD_NONE,
    NORTHWEST = DS4_BUTTON_DPAD_NORTHWEST,
    WEST = DS4_BUTTON_DPAD_WEST,
    SOUTHWEST = DS4_BUTTON_DPAD_SOUTHWEST,
    SOUTH = DS4_BUTTON_DPAD_SOUTH,
    SOUTHEAST = DS4_BUTTON_DPAD_SOUTHEAST,
    EAST = DS4_BUTTON_DPAD_EAST,
    NORTHEAST = DS4_BUTTON_DPAD_NORTHEAST,
    NORTH = DS4_BUTTON_DPAD_NORTH,
  };

  DS4Device* setButton(Button button, bool value);
  DS4Device* setButton(SpecialButton button, bool value);
  DS4Device* setDPad(DPadDirection direction);

  DS4Device* setLXAxis(long value);
  DS4Device* setLYAxis(long value);
  DS4Device* setRXAxis(long value);
  DS4Device* setRYAxis(long value);
  DS4Device* setLTrigger(long value);
  DS4Device* setRTrigger(long value);

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

}// namespace fredemmott::inputmapping
