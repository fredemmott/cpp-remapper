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

namespace fredemmott::inputmapping {

class VJoyDevice final : public OutputDevice {
 public:
  VJoyDevice(uint8_t id);
  ~VJoyDevice();

  virtual void flush() override;

  VJoyDevice* setXAxis(long value);
  VJoyDevice* setYAxis(long value);
  VJoyDevice* setZAxis(long value);
  VJoyDevice* setRXAxis(long value);
  VJoyDevice* setRYAxis(long value);
  VJoyDevice* setRZAxis(long value);
  VJoyDevice* setSlider(long value);
  VJoyDevice* setDial(long value);

  VJoyDevice* setButton(uint8_t button, bool value);
  VJoyDevice* setHat(uint8_t hat, uint16_t value);

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

}// namespace fredemmott::inputmapping
