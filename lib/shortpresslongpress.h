/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <chrono>

#include "SinkRef.h"

namespace fredemmott::inputmapping {

/// Different actions for if a button is short-pressed or long-pressed
class ShortPressLongPress : public ButtonSink {
 public:
  ShortPressLongPress(
    ButtonSinkRef short_handler,
    ButtonSinkRef long_handle,
    const std::chrono::steady_clock::duration duration
    = std::chrono::milliseconds(300));
  virtual void map(Button::Value state) override;

 private:
  ButtonSinkRef mShortPress;
  ButtonSinkRef mLongPress;
  std::chrono::steady_clock::duration mLongDuration;

  std::chrono::time_point<std::chrono::steady_clock> mStart;
};

}// namespace fredemmott::inputmapping
