/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/Clock.h>
#include <map>

namespace fredemmott::inputmapping {

class FakeClock : public Clock {
 private:
  std::chrono::steady_clock::time_point mNow;
  std::map<std::chrono::steady_clock::time_point, std::function<void()>> mTimers;

 public:
  FakeClock();

  void advance(const std::chrono::steady_clock::duration& amount);

  virtual std::chrono::steady_clock::time_point now() noexcept override;

  virtual void setTimer(
    const std::chrono::steady_clock::duration& delay,
    const std::function<void()>& handler) noexcept override;
};

}// namespace fredemmott::inputmapping
