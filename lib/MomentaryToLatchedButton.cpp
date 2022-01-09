/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "MomentaryToLatchedButton.h"

namespace fredemmott::inputmapping {

MomentaryToLatchedButton::MomentaryToLatchedButton(bool pressed)
  : mPressed(pressed) {
}

MomentaryToLatchedButton::~MomentaryToLatchedButton() {
}

void MomentaryToLatchedButton::map(Button::Value value) {
  if (!value) {
    return;
  }
  mPressed = !mPressed;
  emit(mPressed);
}

}// namespace fredemmott::inputmapping
