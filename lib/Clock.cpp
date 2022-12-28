/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <cpp-remapper/Clock.h>

#include <cpp-remapper/EventLoop.h>

namespace fredemmott::inputmapping {

namespace {
std::shared_ptr<Clock> g_clock;
}

std::shared_ptr<Clock> Clock::get() {
  if (g_clock) {
    return g_clock;
  }
  g_clock.reset(new Clock());
  return g_clock;
}

void Clock::set(const std::shared_ptr<Clock>& clock) {
  g_clock = clock;
}

std::chrono::steady_clock::time_point Clock::now() noexcept {
  return std::chrono::steady_clock::now();
};

void Clock::setTimer(
  const std::chrono::steady_clock::duration& duration,
  const std::function<void()>& handler) noexcept {
  EventLoop::inject(duration, handler);
}

}// namespace fredemmott::inputmapping
