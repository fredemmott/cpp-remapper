/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "FakeClock.h"

namespace fredemmott::inputmapping {

FakeClock::FakeClock() {
  mNow = std::chrono::steady_clock::now();
}

void FakeClock::advance(const std::chrono::steady_clock::duration& amount) {
  mNow += amount;
  for (auto it = mTimers.begin(); it != mTimers.end(); /* nothing */) {
    auto [when, handler] = *it;
    if (when > mNow) {
      ++it;
      continue;
    }
    handler();
    it = mTimers.erase(it);
  }
}

std::chrono::steady_clock::time_point FakeClock::now() noexcept {
  return mNow;
}

void FakeClock::setTimer(
  const std::chrono::steady_clock::duration& delay,
  const std::function<void()>& handler) noexcept {
  mTimers.emplace(mNow + delay, handler);
}

}// namespace fredemmott::inputmapping
