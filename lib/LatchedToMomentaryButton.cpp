/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "LatchedToMomentaryButton.h"

#include "Clock.h"

namespace fredemmott::inputmapping {

LatchedToMomentaryButton::LatchedToMomentaryButton(
  const std::chrono::steady_clock::duration& pressDuration)
  : mPressDuration(pressDuration) {
}

void LatchedToMomentaryButton::map(Button::Value value) {
  emit(true);
  Clock::get()->setTimer(mPressDuration, [this]() { emit(false); });
}

}// namespace fredemmott::inputmapping

#include "MappableInput.h"
#include "MappableVJoyOutput.h"
#include "connections.h"

namespace {
using namespace fredemmott::inputmapping;

void static_test() {
  MappableInput stick(nullptr);
  MappableVJoyOutput vj(nullptr);
  stick.Button1 >> LatchedToMomentaryButton() >> vj.Button1;
  stick.Button2 >> LatchedToMomentaryButton(std::chrono::milliseconds(500))
    >> vj.Button2;
}

}// namespace
