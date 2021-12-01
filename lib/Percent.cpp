/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "Percent.h"

namespace fredemmott::inputmapping {

Percent::Percent(uint8_t value) : mValue(value) {
}

uint8_t Percent::value() const noexcept {
  return mValue;
}

Percent operator"" _percent(unsigned long long value) {
  return Percent(value);
}

}// namespace fredemmott::inputmapping
