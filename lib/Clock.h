/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace fredemmott::inputmapping {

class Clock {
 protected:
  Clock() = default;

 public:
  static std::shared_ptr<Clock> get();
  static void set(const std::shared_ptr<Clock>&);

  virtual std::chrono::steady_clock::time_point now() noexcept;

  virtual void setTimer(
    const std::chrono::steady_clock::duration& delay,
    const std::function<void()>& handler) noexcept;
};

}// namespace fredemmott::inputmapping
