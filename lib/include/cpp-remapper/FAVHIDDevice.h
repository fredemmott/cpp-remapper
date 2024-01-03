/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cpp-remapper/OutputDevice.h>

#include <cstdint>
#include <favhid/FAVJoyState2.hpp>
#include <memory>

namespace fredemmott::inputmapping {

/** This is an alternative to VJoy using an
 * Arduino Micro.
 *
 * See https://github.com/fredemmott/favhid
 */
class FAVHIDDevice final : public OutputDevice {
 public:
  FAVHIDDevice() = delete;

  FAVHIDDevice(uint8_t id);
  virtual ~FAVHIDDevice();

  void set(const FAVHID::FAVJoyState2::Report&);

  virtual void flush() override;

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

}// namespace fredemmott::inputmapping