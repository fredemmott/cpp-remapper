/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "ButtonToAxis.h"

namespace fredemmott::inputmapping::actions {

ButtonToAxis::ButtonToAxis(AxisEventHandler next): mNext(next) {
}

void ButtonToAxis::map(bool value) {
  mNext->map(value ? 65535 : 0);
}

} // namespace fredemmott::inputmapping

// --- Tests ---

#include "comboaction.h"
#include "MappableVJoyOutput.h"
using namespace fredemmott::inputmapping;
using namespace fredemmott::inputmapping::actions;

namespace {

// Check that the compiler lets us call it as intended
void static_test() {
  MappableVJoyOutput o(nullptr);
  ButtonToAxis { o.RXAxis };
}

} // namespace
