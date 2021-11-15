/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "OutputDevice.h"

#include <memory>

#include <cstdint>

namespace fredemmott::inputmapping {

class VJoyOutput final : public OutputDevice {
 public:
  VJoyOutput(uint8_t id);
  ~VJoyOutput();

  virtual void flush() override;

  VJoyOutput* setXAxis(long value);
  VJoyOutput* setYAxis(long value);
  VJoyOutput* setZAxis(long value);
  VJoyOutput* setRXAxis(long value);
  VJoyOutput* setRYAxis(long value);
  VJoyOutput* setRZAxis(long value);
  VJoyOutput* setSlider(long value);
  VJoyOutput* setDial(long value);

  VJoyOutput* setButton(uint8_t button, bool value);
  VJoyOutput* setHat(uint8_t hat, uint16_t value);

 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

} // namespace fredemmott::inputmapping
