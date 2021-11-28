/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#pragma once

#include <chrono>

#include "Sink.h"
#include "Source.h"

namespace fredemmott::inputmapping {

/** Turn a button that is maintained/latched on or off into a button that
 * triggers whenever the state changes.
 *
 * This is useful if a game has a binding for 'toggle on/off', but your
 * device has a latching on switch.
 */
class LatchedToMomentaryButton final : public Sink<Button>,
                                       public Source<Button> {
 public:
  LatchedToMomentaryButton(
    const std::chrono::steady_clock::duration& pressDuration
    = std::chrono::milliseconds(20));
  virtual void map(Button::Value value) override;

 private:
  const std::chrono::steady_clock::duration mPressDuration;
};

}// namespace fredemmott::inputmapping
