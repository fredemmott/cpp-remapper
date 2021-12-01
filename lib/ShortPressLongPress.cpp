/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "ShortPressLongPress.h"

#include "Clock.h"

namespace fredemmott::inputmapping {

namespace {
const std::chrono::milliseconds INJECTED_PRESS_DURATION(100);
}

ShortPressLongPress::ShortPressLongPress(
  ButtonSinkPtr s,
  ButtonSinkPtr l,
  std::chrono::steady_clock::duration long_duration)
  : mShortPress(s), mLongPress(l), mLongDuration(long_duration) {
}

void ShortPressLongPress::map(bool pressed) {
  const auto clock = Clock::get();
  const auto now = clock->now();
  if (pressed) {
    mStart = now;
    return;
  }

  // Check for default-constructed invalid value
  if (mStart == std::chrono::steady_clock::time_point()) {
    return;
  }

  if (now - mStart < mLongDuration) {
    mShortPress->map(true);
    clock->setTimer(
      INJECTED_PRESS_DURATION, [=]() { mShortPress->map(false); });
  } else {
    mLongPress->map(true);
    clock->setTimer(INJECTED_PRESS_DURATION, [=]() { mLongPress->map(false); });
  }
}

}// namespace fredemmott::inputmapping
