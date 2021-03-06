/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <memory>

#include <cstdint>

namespace fredemmott::vjoypp {

void init();

class OutputDevice final {
 public:
  OutputDevice(uint8_t id);
  ~OutputDevice();

  OutputDevice* setXAxis(long value);
  OutputDevice* setYAxis(long value);
  OutputDevice* setZAxis(long value);
  OutputDevice* setRXAxis(long value);
  OutputDevice* setRYAxis(long value);
  OutputDevice* setRZAxis(long value);
  OutputDevice* setSlider(long value);
  OutputDevice* setDial(long value);

  OutputDevice* setButton(uint8_t button, bool value);
  OutputDevice* setHat(uint8_t hat, uint16_t value);

  void send();
 private:
  struct Impl;
  std::unique_ptr<Impl> p;
};

} // namespace fredemmott::vjoypp
