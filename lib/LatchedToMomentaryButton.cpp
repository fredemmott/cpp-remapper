/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <cpp-remapper/LatchedToMomentaryButton.h>

#include <cpp-remapper/Clock.h>

namespace fredemmott::inputmapping {

LatchedToMomentaryButton::LatchedToMomentaryButton(
  const std::chrono::steady_clock::duration& pressDuration)
  : mPressDuration(pressDuration) {
}

LatchedToMomentaryButton::~LatchedToMomentaryButton() {
}

void LatchedToMomentaryButton::map(Button::Value value) {
  emit(true);
  Clock::get()->setTimer(mPressDuration, [this]() { emit(false); });
}

}// namespace fredemmott::inputmapping
