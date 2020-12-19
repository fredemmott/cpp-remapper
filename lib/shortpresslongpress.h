/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"
#include "eventhandler.h"

#include <chrono>

namespace fredemmott::inputmapping::actions {

/// Different actions for if a button is short-pressed or long-pressed
class ShortPressLongPress: public ButtonAction {
 public:
  ShortPressLongPress(
    const ButtonEventHandler& short_handler,
    const ButtonEventHandler& long_handle,
    const std::chrono::steady_clock::duration duration = std::chrono::milliseconds(300)
  );
  virtual void map(bool state);
 private:
  ButtonEventHandler mShortPress;
  ButtonEventHandler mLongPress;
  std::chrono::steady_clock::duration mLongDuration;

  std::chrono::time_point<std::chrono::steady_clock> mStart;
};

} // namespace fredemmott::inputmapping::actions
