/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <cstdint>

namespace fredemmott::inputmapping {

class Percent final {
 private:
  uint8_t mValue;

 public:
  explicit Percent(uint8_t value);
  uint8_t value() const noexcept;
};

Percent operator"" _percent(unsigned long long value);

}// namespace fredemmott::inputmapping
