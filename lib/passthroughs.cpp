/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "passthroughs.h"

namespace fredemmott::inputmapping {

VJoyAxis::VJoyAxis(
  const AxisTarget& target
): AxisAction(), mTarget(target) {
}

void VJoyAxis::map(long value) {
  mTarget.set(value);
}

VJoyButton::VJoyButton(
  const ButtonTarget& target
): ButtonAction(), mTarget(target) {
}

void VJoyButton::map(bool value) {
  mTarget.set(value);
}

VJoyHat::VJoyHat(
  const HatTarget& target
): HatAction(), mTarget(target) {
}

void VJoyHat::map(uint16_t value) {
  mTarget.set(value);
}

} // namespace fredemmott::inputmapping
