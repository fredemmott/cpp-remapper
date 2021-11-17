/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "shortpresslongpress.h"

#include "mapper.h"

namespace fredemmott::inputmapping::actions {

namespace {
  const std::chrono::milliseconds INJECTED_PRESS_DURATION(100);
}

ShortPressLongPress::ShortPressLongPress(
  ButtonOutput s,
  ButtonOutput l,
  std::chrono::steady_clock::duration long_duration
) : mShortPress(s), mLongPress(l), mLongDuration(long_duration) {}

void ShortPressLongPress::map(bool pressed) {
  if (pressed) {
    mStart = std::chrono::steady_clock::now();
    return;
  }

  if (mStart == std::chrono::steady_clock::time_point()) {
    return;
  }

  if (std::chrono::steady_clock::now() - mStart < mLongDuration) {
    mShortPress.map(true);
    Mapper::inject(INJECTED_PRESS_DURATION, [=]() { mShortPress.map(false); });
  } else {
    mLongPress.map(true);
    Mapper::inject(INJECTED_PRESS_DURATION, [=]() { mLongPress.map(false); });
  }
}
namespace {
  void static_test() {
    ShortPressLongPress { [](bool) {}, [](bool) {} };
    ShortPressLongPress { [](bool) {}, [](bool) {}, std::chrono::seconds(2) };
  }
}

}
