/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include "actionsapi.h"

#include "vjoypp.h"

namespace fredemmott::inputmapping {

void ButtonTarget::set(bool value) const {
  device->setButton(button, value);
}

void HatTarget::set(int16_t value) const {
  device->setHat(hat, value);
}

void AxisTarget::set(long value) const {
  (device->*(setter))(value);
}

} // namespace fredemmott::inputmapping
